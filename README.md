# fastafs: fuse layer and file system for storing FASTA files

----

Direct link to the file format specification:

[https://github.com/yhoogstrate/fastafs/blob/master/doc/FASTAFS-FORMAT-SPECIFICATION.md](https://github.com/yhoogstrate/fastafs/blob/master/doc/FASTAFS-FORMAT-SPECIFICATION.md)

----

![](https://bioinf-galaxian.erasmusmc.nl/public/images/fastafs/fastafs-example.gif)

## in a compressed and (randomly) accessible manner

DNA sequences are typically stored in the FASTA format. Although very commonly used and easy to read, FASTA files consume vast amounts of diskspace. It is possible to use compression (gzip or 2bit) instead, but such files are binary and not as widely adopted as FASTA. Using for instance 2bit requires all a software package to support each compressed format, which alse requires more discipline on maintainance and testing.

Here we propose a solution; a mixture of 2bit compression and random on FASTA very similar to UCSC TwoBit files, accessible as read-onl FASTA, FAI, DICT and 2BIT files via a FUSE file system layer. By simply mounting the compressed archive as a FASTA and necessary metadata files, we only virtualize the large FASTA file on request. Additional advantages of FASTAFS are the toolkit and interface and the possibility to crc check files, search for duplicate entries.

FASTAFS is compatible with 2bit files but 2bit has some limitations and does for instance not allow mounting DICT files needed for CRAM and Picard tools. The 2bit files can be convert to FASTAFS and, more importantly, a mounted FASTAFS mountpoint will not only virtualize the FASTA but also the 2bit file. Is FASTAFS this famous 15th standard (<https://xkcd.com/927/>)?
Partially, but it is not designed to replace FASTA nor 2bit as those mountpoints should be used as flat files did before and the  virtualization of dict files is not possible when we mount 2bit files.

## installation and compilation

Currently the package uses cmake for compilation
Required dependencies are:

 -   libboost (only for unit testing, will be come an optional dependency soon)
 -   libopenssl (for generating MD5 hashes)
 -   libfuse (for access to the fuse layer system and file virtualization)
 -   c++ compiler supporting c++-17

Compilation is done using cmake. The build command to run cmake for common use is:

```
#!/bin/bash

cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON .
make "$@" -j `nproc`
sudo make install
```


If you like to play with the code and like to help development, you can create a debug binary as follows:

```
$ cmake -DCMAKE_BUILD_TYPE=debug -DCMAKE_INSTALL_PREFIX=~/.local -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON .
$ make "$@" -j `nproc`
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

FASTAFS files will be saved in `~/.local/share/fastafs/<uid>.fastafs` and an entry will be added to the 'database' (`~/.local/share/fastafs/index`).

### fastafs list: overview of fastafs db

The `list` command lists all FASTAFS files located in the 'database' (`~/.local/share/fastafs/index`):

```
$ fastafs list

FASTAFS NAME    FASTAFS        SEQUENCES    BASES   DISK SIZE
test            v0-x32-2bit    7            88      214      
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
$ fastafs ./bin/fastafs mount hg19 /mnt/fastafs/hg19 
$ ls /mnt/fastafs/hg19 
hg19.2bit  hg19.dict  hg19.fa  hg19.fa.fai

$ ls -alsh /mnt/fastafs/hg19
total 0
-rw-r--r-- 1 youri youri 779M Aug 19 15:26 hg19.2bit
-rw-r--r-- 1 youri youri 7.9K Aug 19 15:26 hg19.dict
-rw-r--r-- 1 youri youri 3.0G Aug 19 15:26 hg19.fa
-rw-r--r-- 1 youri youri 3.5K Aug 19 15:26 hg19.fa.fai

$ cat /mnt/fastafs/hg19/hg19.dict 
@HD	VN:1.0	SO:unsorted
@SQ	SN:chr1	LN:249250621	M5:1b22b98cdeb4a9304cb5d48026a85128	UR:fastafs:///hg19
@SQ	SN:chr2	LN:243199373	M5:a0d9851da00400dec1098a9255ac712e	UR:fastafs:///hg19
@SQ	SN:chr3	LN:198022430	M5:641e4338fa8d52a5b781bd2a2c08d3c3	UR:fastafs:///hg19
@SQ	SN:chr4	LN:191154276	M5:23dccd106897542ad87d2765d28a19a1	UR:fastafs:///hg19
```

### fastafs mount: use custom padding

```
$ fastafs mount test /mnt/fastafs/test        
$ ls /mnt/fastafs/test 
test.2bit  test.dict  test.fa  test.fa.fai

$ cat /mnt/fastafs/test/test.fa
>chr1
ttttccccaaaagggg
>chr2
ACTGACTGnnnnACTG
>chr3.1
ACTGACTGaaaac
>chr3.2
ACTGACTGaaaacc
>chr3.3
ACTGACTGaaaaccc
>chr4
ACTGnnnn
>chr5
nnACTG

$ umount /mnt/fastafs/test

$ fastafs mount -p 4 test /mnt/fastafs/test        
$ cat  /mnt/fastafs/test/test.fa | head -n 15
>chr1
tttt
cccc
aaaa
gggg
>chr2
ACTG
ACTG
nnnn
ACTG
>chr3.1
ACTG
ACTG
aaaa
c
```

## Contributing
Feel free to start a discussion or to contribute to the GPL licensed code.

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/c90c7d61651d4e18aa82a4b02f3599fa)](https://www.codacy.com/app/yhoogstrate/fastafs?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=yhoogstrate/fastafs&amp;utm_campaign=Badge_Grade)
