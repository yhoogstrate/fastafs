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

# ---------- valgrind helper function ----------
run_valgrind_metrics() {
    local cmd="$1"
    local valgrind_out
    valgrind_out=$(valgrind --leak-check=full -s $cmd 2>&1 | tail -50)

    local total_allocs=0
    local total_bytes=0
    local definitely_lost=0

    if [[ $valgrind_out =~ total\ heap\ usage:\ ([0-9,]+)\ allocs,\ [0-9,]+\ frees,\ ([0-9,]+)\ bytes ]]; then
        total_allocs=$(echo "${BASH_REMATCH[1]}" | tr -d ',')
        total_bytes=$(echo "${BASH_REMATCH[2]}" | tr -d ',')
    fi

    if [[ $valgrind_out =~ definitely\ lost:\ ([0-9,]+)\ bytes ]]; then
        definitely_lost=$(echo "${BASH_REMATCH[1]}" | tr -d ',')
    fi

    echo "$total_allocs|$total_bytes|$definitely_lost"
}

# ---------- machine / version fingerprint ----------
MACHINE_ID=$(cat /etc/machine-id 2>/dev/null || hostname | md5sum | cut -c1-32)
USER_NAME=$(whoami)
GIT_COMMIT=$(git rev-parse HEAD 2>/dev/null || echo "unknown")
FASTAFS_VERSION=$(${FASTAFS} --version 2>&1 | head -1 || echo "unknown")
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


# ---------- run cache benchmarks ----------
for ENC in dna rna iupac protein; do
    TSV="${BENCH_DIR}/${USER_NAME}-${MACHINE_ID}_cache_${ENC}.txt"
    FA="${TMP_DIR}/bench_${ENC}.fa"

    if [ ! -f "${TSV}" ]; then
        printf 'timestamp\tfastafs-version\tinstructions\tcycles\ttotal_allocs\ttotal_bytes\tdefinitely_lost\tnt_per_cpu_sec\twall_time\tuser_time\tsys_time\tnucleotides\tencoding\tcompression\tcmd\trun\tgit-commit\n' \
            > "${TSV}"
    fi

    for COMPRESSION in plain zstd; do
        if [ "${COMPRESSION}" = "plain" ]; then
            FFS_OUT="${TMP_DIR}/bench_${ENC}_plain.fastafs"
            CACHE_FLAGS="-f -o ${FFS_OUT}"
        else
            FFS_OUT="${TMP_DIR}/bench_${ENC}_zstd.fastafs"
            CACHE_FLAGS="-o ${FFS_OUT}"
        fi

        CMD="${FASTAFS} cache ${CACHE_FLAGS} ${FA}"
        echo "Benchmarking cache ${ENC} (${COMPRESSION}) ..."

        for RUN in 1 2 3; do
            rm -f "${FFS_OUT}" "${FFS_OUT}.zst"
            PERF_OUT=$(perf stat -e instructions,cycles \
                sh -c "${CMD}" 2>&1)
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

            VG_METRICS=$(run_valgrind_metrics "${CMD}")
            IFS='|' read -r TOTAL_ALLOCS TOTAL_BYTES DEFINITELY_LOST <<< "$VG_METRICS"

            printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
                "${TIMESTAMP}" "${FASTAFS_VERSION}" \
                "${INSTRUCTIONS}" "${CYCLES}" \
                "${TOTAL_ALLOCS}" "${TOTAL_BYTES}" "${DEFINITELY_LOST}" \
                "${NT_PER_SEC}" "${WALL}" "${USER_T}" "${SYS}" \
                "${N_NUCLEOTIDES}" "${ENC}" "${COMPRESSION}" "${CMD}" "${RUN}" "${GIT_COMMIT}" \
                >> "${TSV}"

            USER_T_R=$(printf "%.3f" "${USER_T}")
            WALL_R=$(printf "%.3f" "${WALL}")
            SYS_R=$(printf "%.3f" "${SYS}")
            echo "  run ${RUN}: instrs=${INSTRUCTIONS}  ${NT_PER_SEC} nt/cpu-sec  user=${USER_T_R}s  wall=${WALL_R}s  sys=${SYS_R}s  allocs=${TOTAL_ALLOCS}  bytes=${TOTAL_BYTES}  lost=${DEFINITELY_LOST}"
        done
    done
done

# ---------- run view benchmarks ----------
for ENC in dna rna iupac protein; do
    TSV="${BENCH_DIR}/${USER_NAME}-${MACHINE_ID}_view_${ENC}.txt"

    # write TSV header if file is new
    if [ ! -f "${TSV}" ]; then
        printf 'timestamp\tfastafs-version\tinstructions\tcycles\ttotal_allocs\ttotal_bytes\tdefinitely_lost\tnt_per_cpu_sec\twall_time\tuser_time\tsys_time\tnucleotides\tencoding\tcompression\tcmd\trun\tgit-commit\n' \
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

            VG_METRICS=$(run_valgrind_metrics "${CMD} > /dev/null")
            IFS='|' read -r TOTAL_ALLOCS TOTAL_BYTES DEFINITELY_LOST <<< "$VG_METRICS"

            printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
                "${TIMESTAMP}" "${FASTAFS_VERSION}" \
                "${INSTRUCTIONS}" "${CYCLES}" \
                "${TOTAL_ALLOCS}" "${TOTAL_BYTES}" "${DEFINITELY_LOST}" \
                "${NT_PER_SEC}" "${WALL}" "${USER_T}" "${SYS}" \
                "${N_NUCLEOTIDES}" "${ENC}" "${COMPRESSION}" "${CMD}" "${RUN}" "${GIT_COMMIT}" \
                >> "${TSV}"

            USER_T_R=$(printf "%.3f" "${USER_T}")
            WALL_R=$(printf "%.3f" "${WALL}")
            SYS_R=$(printf "%.3f" "${SYS}")
            echo "  run ${RUN}: instrs=${INSTRUCTIONS}  ${NT_PER_SEC} nt/cpu-sec  user=${USER_T_R}s  wall=${WALL_R}s  sys=${SYS_R}s  allocs=${TOTAL_ALLOCS}  bytes=${TOTAL_BYTES}  lost=${DEFINITELY_LOST}"
        done
    done
done

echo ""
echo "Results appended to ${BENCH_DIR}/<machine>_cache_<encoding>.txt  (cache)"
echo "Results appended to ${BENCH_DIR}/<machine>_view_<encoding>.txt   (view)"
echo "Metric: nucleotides per CPU-second (higher = faster)"
