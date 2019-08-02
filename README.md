# fastafs: fuse layer and file system for storing FASTA files

![](https://bioinf-galaxian.erasmusmc.nl/public/images/fastafs/fastafs-example.gif)

## in a compressed and (randomly) accessible manner

DNA sequences are typically stored in the FASTA format. Although very commonly used and easy to read, FASTA files consume vast amounts of diskspace. It is possible to use compression (gzip or 2bit) instead, but such files are binary and not as widely adopted as FASTA. Using for instance 2bit requires all a software package to support each compressed format, which alse requires more discipline on maintainance and testing.

Here we propose a solution; a mixture of 2bit compression and random on FASTA very similar to UCSC TwoBit files, accessible as read-onl FASTA, FAI, DICT and 2BIT files via a FUSE file system layer. By simply mounting the compressed archive as a FASTA and necessary metadata files, we only virtualize the large FASTA file on request. Additional advantages of FASTAFS are the toolkit and interface and the possibility to crc check files, search for duplicate entries.

FASTAFS is not compatible with 2bit files, but 2bit files can be convert to FASTAFS and, more importantly, a mounted FASTAFS mountpoint will not only virtualize the FASTA but also the 2bit file.
So, is FASTAFS this famous 15th standard (<https://xkcd.com/927/>)?
Partially, but it is not designed to replace FASTA nor 2bit as those mountpoints should be used as flat files did before. 
Using 2bit files as backend for the FASTAFS toolkit was impossible as fastafs tries to also do other filesystem related things, such as adding checksums and preserving other metadata that is important, that are not implemented in 2bit.
Nevertheless, the FASTAFS format is heavily inspired by 2bit.

## installation and compilation

Currently the package uses cmake for compilation
Required dependencies are:

 -   libboost (only for unit testing, will be come an optional dependency soon)
 -   libopenssl (for generating MD5 hashes)
 -   libfuse (for access to the fuse layer system and file virtualization)
 -   c++ compiler supporting c++-17

Compilation is done using cmake. The build script to run cmake for common use is:

```
$ ./build-release.sh
$ make install
```

If you like to play with the code and like to help development, you can create a debug binary as follows:

```
$ ./build-debug.sh
$ make install
```

If you have patches, changes or even cool new features you believe are worth contributing, please run astyle with the following command:

```
$ make tidy
```

This styles the code in a more or less compatible way with the rest of the code.
Thanks in advance(!)

## usage
### fastafs cache: adding files to fastafs
We can add files to the fastafs database by running:

```
$ fastafs cache test ./test.fa
```

Or, starting with 2bit:

```
$ fastafs cache test-from-2bit ./test.2bit
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

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/c90c7d61651d4e18aa82a4b02f3599fa)](https://www.codacy.com/app/yhoogstrate/fastafs?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=yhoogstrate/fastafs&amp;utm_campaign=Badge_Grade)
