        fastafs - file system for DNA sequences

DNA sequences are typically stored in the FASTA format. Although very commonly used and easy to read, FASTA files consume vast amounts of diskspace.
It is possible to use compression (gzip or 2bit) instead, but such files are binary and not as practical as FASTA. Also, this requires other software to support each of these compressed formats.

Here we propose a solution; a mixture of 2bit and random access compression on FASTA files in a FUSE file system. By simply mounting the highly compressed archives as a FASTA file, we only virtualize the large FASTA file. Additional advantages are the interface that will crc check the files and search for duplicate entries.
