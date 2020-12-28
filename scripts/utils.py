#!/usr/bin/env python

import wget
import os
from tqdm import tqdm
import datetime
import subprocess



def generate_ACTG_fa(output_file):
    if not os.path.exists(output_file):
        with open(output_file, 'w') as fh:
            for chrom in tqdm(['chr1', 'chr2', 'chr3', 'chr4']):
                fh.write(">" + chrom + "\n")
                buf = ''
                for i in tqdm(range(1337 * 1337)):
                    buf += "AAAAACCCCCTTTTTGGGGGGTCAGCTAGCTACGATCGATCGACTACGA"

                    chunksize = 40
                    while len(buf) >= chunksize:
                        fh.write(buf[0:chunksize] + "\n")
                        buf = buf[chunksize:]

                if len(buf) > 0:
                    fh.write(buf + "\n")



def get_sys_id():
    import socket
    sid = socket.gethostname()

    with open('/etc/machine-id') as fh:
        mid = fh.read().strip()
        
        return (sid + "_" + mid)
    
    import sys
    exit(1)




