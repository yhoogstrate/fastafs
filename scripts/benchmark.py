#!/usr/bin/env python

import os
import datetime

from utils import *
from test_utils import *

import logging


PATH = 'tmp/benchmark'


def check_benchmark_dir():
    logging.info('Checking whether the benchmark directory exists')
    
    if not os.path.exists(PATH):
        logging.info('Directory is missing, creating it')
        os.mkdir(PATH)
    
    if not os.path.exists(PATH + "/mnt"):
        logging.info('Directory is missing, creating it')
        os.mkdir(PATH + "/mnt")


def main():
    check_benchmark_dir()
    generate_ACTG_fa(PATH + "/test.fa")
    check_cache_file_plain('./bin/fastafs', PATH + "/test.fa", PATH + "/test.fastafs")
    check_cache_file_zstd('./bin/fastafs', PATH + "/test.fa", PATH + "/test.zst")

    FASTAFS_REV = get_fastafs_rev()
    GIT_REV = get_git_revision()

    if FASTAFS_REV.find(GIT_REV[0].replace('git-commit:','')) == -1:
        raise Exception("Used FASTAFS version differs from CODEBASE - cross-check FASTAFS path and binary:\nbinary:       " + FASTAFS_REV + "\ngit revision:        " + GIT_REV[0].replace('git-commit:',''))
        import sys
        exit(1)

    TIMESTAMP = str(datetime.datetime.now())

    RESULTS_FILE = "benchmarks/" + get_sys_id() + "_mount_ZSTD.txt"
    check_benchmark_file(RESULTS_FILE)
    write_benchmark_file(RESULTS_FILE, diff_fasta_with_mounted(PATH + "/test.fa", "tmp/benchmark/test", "test", 40, './bin/fastafs', True, 'tmp/benchmark/mnt'), TIMESTAMP, FASTAFS_REV, GIT_REV)

    RESULTS_FILE = "benchmarks/" + get_sys_id() + "_mount_plain.txt"
    check_benchmark_file(RESULTS_FILE)
    write_benchmark_file(RESULTS_FILE, diff_fasta_with_mounted(PATH + "/test.fa", "tmp/benchmark/test", "test", 40, './bin/fastafs', False, 'tmp/benchmark/mnt'), TIMESTAMP, FASTAFS_REV, GIT_REV)

    RESULTS_FILE = "benchmarks/" + get_sys_id() + "_view_ZSTD.txt"
    check_benchmark_file(RESULTS_FILE)
    write_benchmark_file(RESULTS_FILE, diff_fasta_with_view(PATH + "/test.fa", "tmp/benchmark/test", 40, './bin/fastafs', True, 'tmp/benchmark/mnt'), TIMESTAMP, FASTAFS_REV, GIT_REV)

    RESULTS_FILE = "benchmarks/" + get_sys_id() + "_view_plain.txt"
    check_benchmark_file(RESULTS_FILE)
    write_benchmark_file(RESULTS_FILE, diff_fasta_with_view(PATH + "/test.fa", "tmp/benchmark/test", 40, './bin/fastafs', False, 'tmp/benchmark/mnt'), TIMESTAMP, FASTAFS_REV, GIT_REV)



if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    main()






"""
./bin/fastafs cache -o tmp/benchmark/test tmp/benchmark/test.fa
./bin/fastafs check -f tmp/benchmark/test.zst


./bin/fastafs mount -f -p 40 tmp/benchmark/test.zst tmp/benchmark/mnt/

./bin/fastafs mount -p 40 tmp/benchmark/test.zst tmp/benchmark/mnt/
cat tmp/benchmark/mnt/test.fa > tmp/test.fa.x
fusermount -u tmp/benchmark/mnt/


ll tmp/benchmark/test.fa tmp/test.fa.x


 {'cmd': ['perf', 'stat', '-e', 'cycles', './bin/fastafs', 'mount', '-d', '-f', '-p', '40', '-f', 'tmp/benchmark/test.zst', 'tmp/benchmark/mnt/'],
  'stdout': "\n---\nprocessing argv[0] = './bin/fastafs'     [current argument=0]\nprocessing argv[1] = 'mount'     ",
  'stderr': '\n---\nFUSE library version: 2.9.9\nnullpath_ok: 0\nnopath: 0\nutime_omit_ok: 0\nunique: 1, opcode: INIT (',
  'perf': {'cycles': 213834032495,
  'total_time': 45.750253598, 'user_time': 76.734195, 'sys_time': 0.899997}}
  
       
# time to convert to fastafs+GZ
# perf stat -r 4 ./bin/fastafs cache -o tmp/benchmark/test.fastafs.gz tmp/benchmark/test.fa

## exporting to ZSTD is faster than regular writing?



# thread 1:
# perf stat ./bin/fastafs mount -f tmp/benchmark/test.fastafs.gz.zst tmp/benchmark/mnt 
# ./bin/fastafs mount -f tmp/benchmark/test.fastafs.gz.zst tmp/benchmark/mnt

# thread 2:
# cat tmp/benchmark/mnt/test.fastafs.gz.fa > /dev/null ; sudo umount tmp/benchmark/mnt
# 

"""

