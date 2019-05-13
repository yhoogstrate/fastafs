# fastafs: fuse layer and file system for storing FASTA files
in a compressed, non-redundant and (randomly) accessible manner
-------------

DNA sequences are typically stored in the FASTA format. Although very commonly used and easy to read, FASTA files consume vast amounts of diskspace. It is possible to use compression (gzip or 2bit) instead, but such files are binary and not as practical as FASTA. Also, this requires all other software to support each compressed formats which is unfortunately not the case.

Here we propose a solution; a mixture of 2bit and random access compression on FASTA files in a readonly FUSE file system. By simply mounting the highly compressed archives as a FASTA file, we only virtualize the large FASTA file. Additional advantages are the interface that will crc check the files and search for duplicate entries.

FASTAFS is not compatible with 2bit views, so is this thefamous 15th standard (<https://xkcd.com/927/>)?
Partially, yes. It simply is yet another format. 
Backwards compatibility was impossible as fastafs tries to also do other filesystem related things, such as adding checksums and preserving other metadata that is important, that are not within 2bit.
However, the fastafs file is not intended for analysis but purely for storage and can then virtualize both FA and 2bit files.

## installation and compilation

Currently the package uses cmake for compilation
Required dependencies are:

- libboost (only for unit testing, will be come an optional dependency soon)
- libopenssl (for generating SHA hashes)
- libfuse (for access to the fuse layer system and file virtualization)

## usage
### fastafs cache: adding files to fastafs
We can add files to the fastafs database by running:
```
$ fastafs cache test ./test.fa
```

### fastafs list: overview of fastafs db
```
$ fastafs list

FASTAFS NAME    FASTFS ID   FASTAFS        SEQUENCES    BASES   DISK SIZE    MOUNT POINT
test            uid         v0-x32-2bit     7           88      214          /mnt/fastafs/hg19
```

### fastafs info: stats of cached fasta file
```
$ fastafs info

FASTAFS NAME: /home/youri/.local/share/fastafs/test.fastafs
SEQUENCES:    7
    >chr1                    16          c4c3cd07c36416e1ff1174f7b856b62265899d37
    >chr2                    16          e3c064f22445d48874aab98e1aea7e78aef4de28
    >chr3.1                  13          c859056c0fd8f4c749131f260f010590eddc2f58
    >chr3.2                  14          6d6a114c1533a967a571267f43c36058637083af
    >chr3.3                  15          4a5886fd844d26d04f1fd8b6dc1e1bfffa71afea
    >chr4                    8           c54994a903dd4ccb422f6bbd155779a298f16896
    >chr5                    6           e0848040eef5dac325a166146245a8188675a35c
```

### fastafs mount: mount fastafs archive to unlock fasta file(s)
```
$ fastafs mount test /mnt/fastafs/hg19
$ ls /mnt/fastafs/hg19/
test.fa
$ cat /mnt/fastafs/hg19/test.fa
>chr1
TTTT
CCCC
AAAA
GGGG
>chr2
ACTG
ACTG
NNNN
ACT
```

## Contributing
Feel free to start a discussion or to contribute to the GPL licensed code.
