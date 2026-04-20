#!/bin/bash
# Benchmark fastafs view throughput for 2bit DNA/RNA, 4bit IUPAC, 5bit protein.
# Each encoding is tested plain and ZSTD-seekable compressed.
#
# Metric: nucleotides per CPU-second  (nt / user_time)
# Timing via perf stat: hardware instruction/cycle counts + wall/user/sys time.
#
# Run from the repository root on a release build:
#   ./build-release.sh
#   cd <repo-root>
#   bash benchmarks/run_view_benchmarks.sh
#
# Output: benchmarks/<user>-<machineid>_view_encoding.txt  (TSV, appended)

set -euo pipefail

FASTAFS=./build-release/bin/fastafs
N_NUCLEOTIDES=10000000   # 10M nt per encoding type
LINE_WIDTH=60
TMP_DIR=tmp/benchmark
BENCH_DIR=benchmarks

mkdir -p "${TMP_DIR}"

# ---------- machine / version fingerprint ----------
MACHINE_ID=$(cat /etc/machine-id 2>/dev/null || hostname | md5sum | cut -c1-32)
USER_NAME=$(whoami)
GIT_COMMIT=$(git rev-parse HEAD 2>/dev/null || echo "unknown")
FASTAFS_VERSION=$(${FASTAFS} --version 2>&1 | head -1 || echo "unknown")
GIT_STATUS=$(git status --porcelain -b 2>/dev/null \
    | tr '\n' '|' | sed 's/ /_/g' | sed 's/|$//' || echo "unknown")
TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S.%6N')

# ---------- generate large FASTA files on-the-fly ----------
echo "Generating ${N_NUCLEOTIDES} nt FASTA files in ${TMP_DIR}/ ..."

python3 - <<PYEOF
import os, sys

n   = ${N_NUCLEOTIDES}
lw  = ${LINE_WIDTH}
tmp = '${TMP_DIR}'

encodings = {
    'dna':     'ACGT' * (lw // 4),                      # 60 chars
    'rna':     'ACGU' * (lw // 4),                      # 60 chars, U -> twobit_rna
    'iupac':   'NBKAHMDCUWGSYVTRHGWV' * (lw // 20),     # 60 chars, 4bit IUPAC
    'protein': 'ACDEFGHIKLMNPQRSTVWY' * (lw // 20),     # 60 chars, E -> fivebit
}

N_STRETCH = 'N' * lw + '\n' + 'N' * lw

for enc, pattern in encodings.items():
    full_lines  = n // lw
    remainder   = n  % lw
    lines = [pattern] * full_lines
    if remainder:
        lines.append(pattern[:remainder])
    if enc in ('dna', 'rna'):
        n_lines = len(lines)
        for pos in reversed([1/4, 2/4, 3/4]):
            lines.insert(int(n_lines * pos), N_STRETCH)
    body = '\n'.join(lines)
    content = f'>{enc}-benchmark\n{body}\n'
    path = os.path.join(tmp, f'bench_{enc}.fa')
    with open(path, 'w') as f:
        f.write(content)
    print(f'  {path}: {os.path.getsize(path):,} bytes')
PYEOF

# ---------- convert to fastafs (plain and zstd) ----------
echo "Converting to fastafs ..."
for ENC in dna rna iupac protein; do
    FA="${TMP_DIR}/bench_${ENC}.fa"

    FFS_PLAIN="${TMP_DIR}/bench_${ENC}_plain.fastafs"
    ${FASTAFS} cache -f -o "${FFS_PLAIN}" "${FA}" 2>/dev/null
    echo "  ${FFS_PLAIN}: $(du -sh "${FFS_PLAIN}" | cut -f1)"

    FFS_ZSTD="${TMP_DIR}/bench_${ENC}_zstd.fastafs"
    ${FASTAFS} cache -o "${FFS_ZSTD}" "${FA}" 2>/dev/null
    echo "  ${FFS_ZSTD}.zst: $(du -sh "${FFS_ZSTD}.zst" | cut -f1)"
done

# ---------- run benchmarks ----------
for ENC in dna rna iupac protein; do
    TSV="${BENCH_DIR}/${USER_NAME}-${MACHINE_ID}_view_${ENC}.txt"

    # write TSV header if file is new
    if [ ! -f "${TSV}" ]; then
        printf 'timestamp\tfastafs-version\tgit-commit\tinstructions\tcycles\tnt_per_cpu_sec\twall_time\tuser_time\tsys_time\tnucleotides\tencoding\tcompression\tcmd\trun\tgit-mod-status\n' \
            > "${TSV}"
    fi

    for COMPRESSION in plain zstd; do
        if [ "${COMPRESSION}" = "plain" ]; then
            FFS="${TMP_DIR}/bench_${ENC}_plain.fastafs"
        else
            FFS="${TMP_DIR}/bench_${ENC}_zstd.fastafs.zst"
        fi

        CMD="${FASTAFS} view -f ${FFS}"
        echo "Benchmarking ${ENC} (${COMPRESSION}) ..."

        # run 3 times, append each result
        for RUN in 1 2 3; do
            PERF_OUT=$(perf stat -e instructions,cycles \
                sh -c "${CMD} > /dev/null" 2>&1)
            INSTRUCTIONS=$(echo "$PERF_OUT" | grep instructions \
                | awk '{gsub(/,/,""); print $1}')
            CYCLES=$(echo "$PERF_OUT" | grep cycles \
                | awk '{gsub(/,/,""); print $1}')
            WALL=$(echo "$PERF_OUT" | grep 'seconds time elapsed' \
                | awk '{print $1}')
            USER_T=$(echo "$PERF_OUT" | grep 'seconds user' \
                | awk '{print $1}')
            SYS=$(echo "$PERF_OUT" | grep 'seconds sys' \
                | awk '{print $1}')

            if python3 -c "exit(0 if float('${USER_T}') > 0 else 1)" 2>/dev/null; then
                NT_PER_SEC=$(python3 -c "print(int(${N_NUCLEOTIDES} / float('${USER_T}')))")
            else
                NT_PER_SEC="N/A"
            fi

            printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
                "${TIMESTAMP}" "${FASTAFS_VERSION}" "${GIT_COMMIT}" \
                "${INSTRUCTIONS}" "${CYCLES}" \
                "${NT_PER_SEC}" "${WALL}" "${USER_T}" "${SYS}" \
                "${N_NUCLEOTIDES}" "${ENC}" "${COMPRESSION}" "${CMD}" "${RUN}" "${GIT_STATUS}" \
                >> "${TSV}"

            USER_T_R=$(printf "%.3f" "${USER_T}")
            WALL_R=$(printf "%.3f" "${WALL}")
            SYS_R=$(printf "%.3f" "${SYS}")
            echo "  run ${RUN}: intrucs=${INSTRUCTIONS}  ${NT_PER_SEC} nt/cpu-sec  user=${USER_T_R}s  wall=${WALL_R}s  sys=${SYS_R}s  cycles=${CYCLES})"
        done
    done
done

echo ""
echo "Results appended to ${BENCH_DIR}/<machine>_view_<encoding>.txt"
echo "Metric: nucleotides decoded per CPU-second (higher = faster)"
