#!/usr/bin/env python3
"""
Mount benchmark (UNTHREADED / single-threaded FUSE) voor fastafs virtuele
bestandsformaten (fa, 2bit, dict, fai).

Identiek aan run_mount_format_benchmarks.py, met één verschil: er wordt gemount
met 'fastafs mount -s', wat de multi-threaded FUSE-operatie uitschakelt. Aparte
variant zodat je snel alleen de unthreaded prestaties kunt draaien. Single-threaded
lezen serialiseert de reads -> deterministischer, lager-ruis metingen waarin
performance-verschillen (per-read CPU-werk) beter zichtbaar zijn dan onder threading,
dat die verschillen kan effenen via scheduling/contentie/readahead.

Genereert dezelfde testbestanden als run_view_benchmarks.sh, monteert elk
fastafs-archief en leest elk virtueel bestandstype met drie toegangspatronen:
  - forward : sequentieel van begin naar einde
  - backward: sequentieel van einde naar begin
  - random  : willekeurige seeks (seed=42), met een vast patroon van
              sluiten/heropenen van de file-handle

Encodings : dna, rna, iupac, protein
Compressie: plain, zstd   (als kolom; beide gaan in hetzelfde uitvoerbestand)
Formaten  : fa, 2bit, dict, fai

Uitvoer: benchmarks/<user>-<machine-id>_mount_<enc>_<fmt>_unthreaded.txt  (TSV, appended)
         -> dus één bestand per (encoding x bestandsformaat), gescheiden van de
            threaded resultaten.

Draaien vanuit de repo-root (release build vereist):
  python3 benchmarks/run_mount_format_benchmarks_unthreaded.py
"""

import datetime
import getpass
import hashlib
import os
import random
import re
import socket
import subprocess
import sys
import time

FASTAFS      = './build-release/bin/fastafs'
TMP_DIR      = 'tmp/benchmark'
BENCH_DIR    = 'benchmarks'
CHUNK_SIZE   = 65536
N_RANDOM     = 200        # aantal random seeks per meting
RANDOM_SEED  = 42
REOPEN_EVERY = 50         # vast patroon: elke N-de random-seek de handle heropenen
N_RUNS       = 3

# Extra fastafs-mount-argumenten voor deze variant: '-s' schakelt multi-threaded
# FUSE uit (single-threaded). Wordt na het 'mount'-subcommando ingevoegd.
MOUNT_EXTRA_ARGS = ['-s']

N_NUCLEOTIDES    = 10_000_000
LINE_WIDTH       = 60
N_BLOCKS_N       = 64
N_BLOCKS_M       = 64
N_BLOCKS_OVERLAP = 32

ENCODINGS    = ['dna', 'rna', 'iupac', 'protein']
COMPRESSIONS = ['plain', 'zstd']
PATTERNS     = ['forward', 'backward', 'random']

# Bestandsformaat -> suffix waarmee het virtuele bestand in de mount herkenbaar is.
# 'fa' matcht exact op '.fa' (de '.fa.fai' index eindigt op '.fai', niet op '.fa').
FILE_TYPE_SUFFIX = {
    'fa':   '.fa',
    '2bit': '.2bit',
    'dict': '.dict',
    'fai':  '.fai',
}

BASE_PATTERNS = {
    'dna':     ('ACGT'                * LINE_WIDTH)[:LINE_WIDTH],
    'rna':     ('ACGU'                * LINE_WIDTH)[:LINE_WIDTH],
    'iupac':   ('BKAHMDCUWGSYVTRHGWV' * LINE_WIDTH)[:LINE_WIDTH],
    'protein': ('ACDEFGHIKLMPQRSTVWY' * LINE_WIDTH)[:LINE_WIDTH],
}


# ---------- omgevings-helpers ----------

def get_machine_id():
    try:
        with open('/etc/machine-id') as fh:
            return fh.read().strip()
    except FileNotFoundError:
        return hashlib.md5(socket.gethostname().encode()).hexdigest()

def get_git_commit():
    try:
        return subprocess.check_output(
            ['git', 'rev-parse', 'HEAD'], stderr=subprocess.DEVNULL
        ).decode().strip()
    except Exception:
        return 'unknown'


def get_fastafs_version():
    try:
        return subprocess.check_output(
            [FASTAFS, '--version'], stderr=subprocess.STDOUT
        ).decode().split('\n')[0].strip()
    except Exception:
        return 'unknown'


# ---------- gedeelde testdata-generatie (zelfde logica als run_view_benchmarks.sh) ----------

def _make_blocks_dna_rna(enc, n_N, n_M, n_overlap):
    rng  = random.Random(42)
    base = 'acgt' if enc == 'dna' else 'acgu'
    blocks = []
    for _ in range(n_N):
        blocks.append('N' * rng.randint(30, 300))
    for _ in range(n_M):
        blocks.append(''.join(rng.choice(base) for _ in range(rng.randint(30, 300))))
    for _ in range(n_overlap):
        blocks.append('n' * rng.randint(30, 300))
    rng.shuffle(blocks)
    return blocks

def _make_blocks_other(enc, n_N, n_M):
    rng = random.Random(42)
    if enc == 'iupac':
        n_fill  = '-'
        m_chars = 'bkahmdcuwgsyvtrhgwv'
    else:  # protein
        n_fill  = '?'
        m_chars = 'acdefghiklmpqrstvwy'
    blocks = []
    for _ in range(n_N):
        blocks.append(n_fill * rng.randint(30, 300))
    for _ in range(n_M):
        blocks.append(''.join(rng.choice(m_chars) for _ in range(rng.randint(30, 300))))
    rng.shuffle(blocks)
    return blocks

def generate_fasta(enc, path):
    if not os.path.exists(path):
        pattern    = BASE_PATTERNS[enc]
        full_lines = N_NUCLEOTIDES // LINE_WIDTH
        remainder  = N_NUCLEOTIDES  % LINE_WIDTH
        lines      = [pattern] * full_lines
        if remainder:
            lines.append(pattern[:remainder])

        n_lines_orig = len(lines)
        if enc in ('dna', 'rna'):
            blocks = _make_blocks_dna_rna(enc, N_BLOCKS_N, N_BLOCKS_M, N_BLOCKS_OVERLAP)
        else:
            blocks = _make_blocks_other(enc, N_BLOCKS_N, N_BLOCKS_M)
        nb = len(blocks)
        for i, block_line in enumerate(reversed(blocks)):
            pos = int(n_lines_orig * (nb - i) / (nb + 1))
            lines[pos:pos] = [block_line]

        with open(path, 'w') as fh:
            fh.write(f'>{enc}-benchmark\n')
            fh.write('\n'.join(lines))
            fh.write('\n')
        print(f'  Gegenereerd {path} ({os.path.getsize(path):,} bytes)')

def ffs_path_for(enc, compression):
    if compression == 'plain':
        return os.path.join(TMP_DIR, f'bench_{enc}_plain.fastafs')
    return os.path.join(TMP_DIR, f'bench_{enc}_zstd.fastafs.zst')

def cache_to_fastafs(fa_path, enc, compression):
    ffs_path = ffs_path_for(enc, compression)
    if not os.path.exists(ffs_path):
        if compression == 'plain':
            flags = ['-f', '-o', ffs_path]
        else:
            flags = ['-o', os.path.join(TMP_DIR, f'bench_{enc}_zstd.fastafs')]
        cmd = [FASTAFS, 'cache'] + flags + [fa_path]
        print(f'  Cachen: {" ".join(cmd)}')
        subprocess.run(cmd, check=True,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return ffs_path

def ensure_test_data():
    """Genereert alle FASTA-bestanden en fastafs-archieven indien nog niet aanwezig."""
    os.makedirs(TMP_DIR,   exist_ok=True)
    os.makedirs(BENCH_DIR, exist_ok=True)
    for enc in ENCODINGS:
        fa_path = os.path.join(TMP_DIR, f'bench_{enc}.fa')
        generate_fasta(enc, fa_path)
        for comp in COMPRESSIONS:
            cache_to_fastafs(fa_path, enc, comp)


# ---------- toegangspatroon-lezers ----------

def read_forward(path):
    total = 0
    t0    = time.perf_counter()
    with open(path, 'rb') as fh:
        data = fh.read(CHUNK_SIZE)
        while data:
            total += len(data)
            data = fh.read(CHUNK_SIZE)
    return total, time.perf_counter() - t0

def read_backward(path):
    total = 0
    t0    = time.perf_counter()
    size  = os.path.getsize(path)
    with open(path, 'rb') as fh:
        pos = size
        while pos > 0:
            read_size = min(CHUNK_SIZE, pos)
            pos      -= read_size
            fh.seek(pos)
            total += len(fh.read(read_size))
    return total, time.perf_counter() - t0

def read_random(path):
    rng   = random.Random(RANDOM_SEED)
    total = 0
    size  = os.path.getsize(path)
    if size == 0:
        return 0, 0.0

    t0 = time.perf_counter()
    fh = open(path, 'rb')
    for i in range(N_RANDOM):
        pos = rng.randint(0, max(0, size - CHUNK_SIZE))
        fh.seek(pos)
        data   = fh.read(CHUNK_SIZE)
        total += len(data)
        # Vast patroon: elke REOPEN_EVERY-de seek de handle sluiten en heropenen,
        # zodat de FUSE file-handle lifecycle ook wordt geraakt (reproduceerbaar).
        if (i + 1) % REOPEN_EVERY == 0:
            fh.close()
            fh = open(path, 'rb')
    fh.close()
    return total, time.perf_counter() - t0

READERS = {
    'forward':  read_forward,
    'backward': read_backward,
    'random':   read_random,
}


# ---------- perf stat + peak RSS ----------

def parse_perf(text):
    def _val(pat, default='0'):
        m = re.search(pat, text, re.IGNORECASE)
        return m.group(1).replace(',', '') if m else default
    return {
        'instructions': int(_val(r'([0-9,]+)\s+instructions')),
        'cycles':       int(_val(r'([0-9,]+)\s+cycles')),
        'wall':   float(_val(r'([0-9.]+)\s+seconds time elapsed')),
        'user':   float(_val(r'([0-9.]+)\s+seconds user')),
        'sys':    float(_val(r'([0-9.]+)\s+seconds sys')),
    }

def find_child_pid(parent_pid):
    """De fastafs-PID is een kind van het perf-stat-proces."""
    try:
        out = subprocess.check_output(
            ['pgrep', '-P', str(parent_pid)], stderr=subprocess.DEVNULL
        ).decode().split()
        return int(out[0]) if out else None
    except Exception:
        return None

def get_peak_rss_kb(pid):
    """Lees VmHWM (kernel high-water mark) uit /proc/<pid>/status."""
    if pid is not None:
        try:
            with open(f'/proc/{pid}/status') as fh:
                for line in fh:
                    if line.startswith('VmHWM:'):
                        return int(line.split()[1])
        except (OSError, ValueError):
            pass
    return 0


# ---------- valgrind: cumulatief geheugengebruik van de mount-daemon ----------

WITH_VALGRIND = True   # zet op False om de (trage) valgrind-meting over te slaan

def parse_valgrind(text):
    def _num(pat):
        m = re.search(pat, text)
        return int(m.group(1).replace(',', '')) if m else 0
    return {
        'allocs': _num(r'total heap usage:\s*([0-9,]+)\s+allocs'),
        'bytes':  _num(r'total heap usage:\s*[0-9,]+\s+allocs,\s*[0-9,]+\s+frees,\s*([0-9,]+)\s+bytes'),
        'lost':   _num(r'definitely lost:\s*([0-9,]+)\s+bytes'),
    }

def check_valgrind_target(text):
    """Verifieer dat valgrind daadwerkelijk de fastafs-mount tracet en niet een
    shell-wrapper of de verkeerde (geforkte) parent. valgrind print de getracede
    opdracht als '==PID== Command: <argv>'. Geeft de Command-regel terug, of
    waarschuwt en geeft None als die niet 'fastafs' + 'mount' bevat."""
    m = re.search(r'==\d+==\s*Command:\s*(.+)', text)
    command = m.group(1).strip() if m else None
    if command is None:
        print('  WAARSCHUWING: geen valgrind Command-regel gevonden; '
              'allocatie-cijfers mogelijk onbetrouwbaar.')
    elif 'fastafs' not in command or 'mount' not in command:
        print(f'  WAARSCHUWING: valgrind tracet niet "fastafs mount" maar: {command!r}')
        command = None
    return command

def valgrind_mount_metrics(ffs_path, mnt_dir, suffix, pattern):
    """Meet cumulatieve heap-allocaties van de fastafs mount-daemon onder valgrind.
    Aparte meting (niet onder perf): valgrind tracet alle allocaties van mount + de
    reads die het lezen van het virtuele bestand uitlokt. Deterministisch -> eenmalig.
    Geeft total_allocs, total_bytes (cumulatief gealloceerd) en definitely_lost bytes."""
    vg_tmp = os.path.join(TMP_DIR, f'valgrind_mount_{pattern}_unthreaded.txt')
    ensure_mountpoint(mnt_dir)

    # valgrind's eigen '-s' (toon foutenlijst) staat vóór FASTAFS; fastafs' '-s'
    # (single-threaded) staat na het 'mount'-subcommando via MOUNT_EXTRA_ARGS.
    cmd = ['valgrind', '--leak-check=full', '-s',
           FASTAFS, 'mount', *MOUNT_EXTRA_ARGS, '-f', ffs_path, mnt_dir]
    vg_fh = open(vg_tmp, 'w')
    proc  = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=vg_fh)

    virtual_file = None
    deadline     = time.time() + 60.0   # valgrind is traag -> ruimere deadline
    while (virtual_file is None and time.time() < deadline
           and proc.poll() is None):
        try:
            hits = [f for f in os.listdir(mnt_dir) if f.endswith(suffix)]
        except OSError:
            hits = []
        if hits:
            virtual_file = os.path.join(mnt_dir, hits[0])
        else:
            time.sleep(0.05)

    if virtual_file is not None:
        try:
            READERS[pattern](virtual_file)   # resultaat doet er niet toe; lokt allocaties uit
        except OSError:
            pass

    unmount(mnt_dir)
    proc.wait()
    vg_fh.close()

    text = open(vg_tmp).read()
    check_valgrind_target(text)   # waarschuwt als valgrind niet de fastafs-mount tracet
    return parse_valgrind(text)


# ---------- TSV-uitvoer ----------

HEADER = [
    'timestamp', 'fastafs-version', 'git-commit',
    'encoding', 'compression', 'file-type', 'pattern', 'run',
    'bytes-read-by-client', 'elapsed-s', 'throughput-MB-s',
    'instructions', 'cycles', 'wall-s', 'user-s', 'sys-s',
    'peak-rss-kb',
    'total-allocs', 'total-bytes', 'definitely-lost-bytes',
    'threading',
]

def ensure_tsv(path):
    if not os.path.exists(path):
        with open(path, 'w') as fh:
            fh.write('\t'.join(HEADER) + '\n')

def append_tsv(path, row):
    with open(path, 'a') as fh:
        fh.write('\t'.join(str(v) for v in row) + '\n')


# ---------- mountpoint-beheer ----------

def unmount(mnt_dir):
    subprocess.run(['fusermount', '-u', mnt_dir],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

def ensure_mountpoint(mnt_dir):
    """Maak een eventueel achtergebleven (kapotte) FUSE-mount los en zorg dat
    mnt_dir een bruikbare map is. Vangt de 'Transport endpoint is not
    connected'-toestand af die na een gecrashte run blijft hangen: een lazy
    unmount (-z) detacht de stale mount zodat de map weer normaal te stat'en is."""
    subprocess.run(['fusermount', '-u', '-z', mnt_dir],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        if not os.path.isdir(mnt_dir):
            os.makedirs(mnt_dir, exist_ok=True)
    except FileExistsError:
        pass


# ---------- één benchmark-run ----------

def run_one(ffs_path, mnt_dir, enc, compression, file_type, pattern,
            timestamp, version, git_commit, tsv_path):
    """Geeft het aantal mislukte runs terug (0..N_RUNS)."""
    suffix   = FILE_TYPE_SUFFIX[file_type]
    perf_tmp = os.path.join(TMP_DIR, f'perf_mount_{enc}_{compression}_{file_type}_unthreaded.txt')
    failures = 0

    # Cumulatief geheugengebruik is deterministisch -> eenmalig meten (los van de perf-runs).
    vg = {'allocs': 0, 'bytes': 0, 'lost': 0}
    if WITH_VALGRIND:
        vg = valgrind_mount_metrics(ffs_path, mnt_dir, suffix, pattern)
        print(f'  valgrind: {vg["bytes"]:,} bytes  lost={vg["lost"]}')

    for run in range(1, N_RUNS + 1):
        ensure_mountpoint(mnt_dir)   # ruim eventuele kapotte mount van een vorige run op

        cmd = ['perf', 'stat', '-e', 'instructions,cycles',
               FASTAFS, 'mount', *MOUNT_EXTRA_ARGS, '-f', ffs_path, mnt_dir]
        perf_fh    = open(perf_tmp, 'w')
        mount_proc = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=perf_fh)

        # Wacht tot het virtuele bestand verschijnt; stop ook als mount vroegtijdig eindigt.
        virtual_file = None
        deadline     = time.time() + 10.0
        while (virtual_file is None and time.time() < deadline
               and mount_proc.poll() is None):
            try:
                hits = [f for f in os.listdir(mnt_dir) if f.endswith(suffix)]
            except OSError:
                hits = []
            if hits:
                virtual_file = os.path.join(mnt_dir, hits[0])
            else:
                time.sleep(0.05)

        bytes_read = elapsed = peak_rss = None
        if virtual_file is not None:
            try:
                fastafs_pid         = find_child_pid(mount_proc.pid)
                bytes_read, elapsed = READERS[pattern](virtual_file)
                peak_rss            = get_peak_rss_kb(fastafs_pid)
            except OSError as exc:
                print(f'  FOUT bij lezen van *{suffix} ({pattern}): {exc}')
                bytes_read = elapsed = peak_rss = None
                failures += 1

        # Altijd opruimen, ook na een leesfout, zodat er geen kapotte mount achterblijft.
        unmount(mnt_dir)
        mount_proc.wait()
        perf_fh.close()

        if bytes_read is not None:
            perf = parse_perf(open(perf_tmp).read())
            mb_s = bytes_read / elapsed / 1e6 if elapsed > 0 else 0.0
            print(f'  run {run}: {bytes_read:,} bytes  {mb_s:.1f} MB/s'
                  f'  elapsed={elapsed:.3f}s'
                  f'  instr={perf["instructions"]}  rss={peak_rss}kB')
            append_tsv(tsv_path, [
                timestamp, version, git_commit,
                enc, compression, file_type, pattern, run,
                bytes_read, f'{elapsed:.6f}', f'{mb_s:.3f}',
                perf['instructions'], perf['cycles'],
                f'{perf["wall"]:.6f}', f'{perf["user"]:.6f}', f'{perf["sys"]:.6f}',
                peak_rss,
                vg['allocs'], vg['bytes'], vg['lost'],
                'unthreaded',
            ])
        elif virtual_file is None:
            print(f'  OVERGESLAGEN: *{suffix} niet gevonden in {mnt_dir} (mount mislukt?)')
            failures += 1

    return failures


# ---------- main ----------

def main():
    print(f'fastafs binary: {os.path.realpath(FASTAFS)}')
    print('Modus: UNTHREADED (fastafs mount -s)')
    print('Testdata voorbereiden ...')
    ensure_test_data()

    machine_id = get_machine_id()
    username   = getpass.getuser()
    git_commit = get_git_commit()
    version    = get_fastafs_version()
    timestamp  = str(datetime.datetime.now())

    total_failures = 0

    for enc in ENCODINGS:
        for compression in COMPRESSIONS:
            ffs_path = ffs_path_for(enc, compression)
            mnt_dir  = os.path.join(TMP_DIR, f'mnt_{enc}_{compression}')
            ensure_mountpoint(mnt_dir)

            for file_type in FILE_TYPE_SUFFIX:
                # Eén uitvoerbestand per (encoding x bestandsformaat); beide
                # compressies en alle patronen komen er als kolommen in.
                # '_unthreaded'-suffix houdt deze resultaten gescheiden van de
                # threaded run (run_mount_format_benchmarks.py).
                tsv_path = os.path.join(
                    BENCH_DIR,
                    f'{username}-{machine_id}_mount_{enc}_{file_type}_unthreaded.txt'
                )
                ensure_tsv(tsv_path)

                for pattern in PATTERNS:
                    print(f'\nBenchmarking [unthreaded] {enc} ({compression})  '
                          f'{file_type}  patroon={pattern} ...')
                    total_failures += run_one(ffs_path, mnt_dir, enc, compression,
                                             file_type, pattern,
                                             timestamp, version, git_commit, tsv_path)

    print(f'\nResultaten toegevoegd aan '
          f'{BENCH_DIR}/{username}-{machine_id}_mount_<enc>_<fmt>_unthreaded.txt')

    if total_failures:
        RED   = '\033[1;31m'
        RESET = '\033[0m'
        print(f'\n{RED}WAARSCHUWING: {total_failures} benchmark-run(s) mislukt '
              f'(OVERGESLAGEN of leesfout).{RESET}')
        sys.exit(1)


if __name__ == '__main__':
    main()
