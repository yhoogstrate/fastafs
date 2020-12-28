#!/usr/bin/env python

import os
import datetime

from utils import *
from test_utils import *


RESULTS_FILE = "benchmarks/" + get_sys_id() + ".txt"
if not os.path.exists(RESULTS_FILE):
    with open(RESULTS_FILE,'w') as fh:
        fh.write(
            "\t".join(
            ["timestamp", "fastafs-version", "git-commit", "perf:cycles", "perf:total_time", "perf:user_time", "perf:sys_time", "cmd", "git-mod-status"]
            ) + "\n"
        )

FASTAFS_REV = get_fastafs_rev()
GIT_REV = get_git_revision()

if FASTAFS_REV.find(GIT_REV[0].replace('git-commit:','')) == -1:
    raise Exception("Used FASTAFS version differs from CODEBASE - cross-check FASTAFS path and binary:\nbinary:       " + FASTAFS_REV + "\ngit revision:        " + GIT_REV[0].replace('git-commit:',''))
    import sys
    exit(1)

TIMESTAMP = str(datetime.datetime.now())


PATH = 'tmp/benchmark'
if not os.path.exists(PATH):
    os.mkdir(PATH)


generate_ACTG_fa(PATH + "/test.fa")
# time to convert to fastafs
# perf stat -r 4 ./bin/fastafs cache --fastafs-only -o tmp/benchmark/test.fastafs tmp/benchmark/test.fa

# should find diff + ccyles
difference = diff_fasta_with_mounted(PATH + "/test.fa", "tmp/benchmark/test", "test", 40, './bin/fastafs', 'tmp/benchmark/mnt')
"""
./bin/fastafs cache -o tmp/benchmark/test tmp/benchmark/test.fa
./bin/fastafs check -f tmp/benchmark/test.zst


./bin/fastafs mount -f -p 40 tmp/benchmark/test.zst tmp/benchmark/mnt/

./bin/fastafs mount -p 40 tmp/benchmark/test.zst tmp/benchmark/mnt/
cat tmp/benchmark/mnt/test.fa > tmp/test.fa.x
fusermount -u tmp/benchmark/mnt/


ll tmp/benchmark/test.fa tmp/test.fa.x

"""

# {'cmd': ['perf', 'stat', '-e', 'cycles', './bin/fastafs', 'mount', '-d', '-f', '-p', '40', '-f', 'tmp/benchmark/test.zst', 'tmp/benchmark/mnt/'],
#  'stdout': "\n---\nprocessing argv[0] = './bin/fastafs'     [current argument=0]\nprocessing argv[1] = 'mount'     ",
#  'stderr': '\n---\nFUSE library version: 2.9.9\nnullpath_ok: 0\nnopath: 0\nutime_omit_ok: 0\nunique: 1, opcode: INIT (',
#   'perf': {'cycles': 213834032495,
#  'total_time': 45.750253598, 'user_time': 76.734195, 'sys_time': 0.899997}}



with open(RESULTS_FILE, 'a') as fh:
    print(" >> difference: " )
    print(difference)
    if(difference['diff']):
        raise Exception("ERROR - DIFFERENCE DETECTED")
        import sys
        sys.exit(1)
    else:
        fh.write(
            "\t".join([TIMESTAMP,
                       FASTAFS_REV,
                       GIT_REV[0].replace('git-commit:',''),
                       str(difference['perf']['cycles']),
                       str(difference['perf']['total_time']),
                       str(difference['perf']['user_time']),
                       str(difference['perf']['sys_time']),
                       " ".join(difference['cmd']),
                       GIT_REV[1]]) + "\n"
        )


 
# time to convert to fastafs+GZ
# perf stat -r 4 ./bin/fastafs cache -o tmp/benchmark/test.fastafs.gz tmp/benchmark/test.fa

## exporting to ZSTD is faster than regular writing?



# thread 1:
# perf stat ./bin/fastafs mount -f tmp/benchmark/test.fastafs.gz.zst tmp/benchmark/mnt 
# ./bin/fastafs mount -f tmp/benchmark/test.fastafs.gz.zst tmp/benchmark/mnt

# thread 2:
# cat tmp/benchmark/mnt/test.fastafs.gz.fa > /dev/null ; sudo umount tmp/benchmark/mnt
# 


