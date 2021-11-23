# FASTAFS: toolkit for file system virtualisation of random access compressed FASTA files
----

# Citing FASTAFS or its virtualisation philosophy

Hoogstrate, Y., Jenster, G.W. & van de Werken, H.J.G.

FASTAFS: file system virtualisation of random access compressed FASTA files.

BMC Bioinformatics 22, 535 (2021).

https://doi.org/10.1186/s12859-021-04455-3

----

Direct link to the file format specification:

[https://github.com/yhoogstrate/fastafs/blob/master/doc/FASTAFS-FORMAT-SPECIFICATION.md](https://github.com/yhoogstrate/fastafs/blob/master/doc/FASTAFS-FORMAT-SPECIFICATION.md)

----

![](https://bioinf-galaxian.erasmusmc.nl/public/images/fastafs/fastafs-example.gif)

## Elegant integration of sequence data archives, backwards compatible with FASTA and no API's needed

RNA, DNA and protein sequences are commonly stored in the FASTA format. Although very commonly used and easy to read, FASTA files come with additional metadata files and consume unnecessary disk space. These additional metadata files need to be are necessary to achieve random access and have certain interoperability features, and require additional maintaince. Classical FASTA (de-)compressors only offer back and forwards compression of the files, often requiring to decompress to a new copy of the FASTA file making it inpractical solutions in particular for random access use cases. Although they typically produce very compact archives with quick algorithms, they are not widely adopted in our bioinformatics software.

Here we propose a solution; a virtual layer between (random access) FASTA archives and read-only access to FASTA files and their guarenteed in-sync FAI, DICT and 2BIT files, through the File System in Userspace (FUSE) file system layer. When the archive is mounted, fastafs virtualizes a folder containing the FASTA and necessary metadata files, only accessing the chunks of the archive needed to deliver to the file request. This elegant software solution offers several advantages:
 - virtual files and their system calls are identical to flat files and preserve backwards compatibility with tools only compatible with FASTA, also for random access use-cases,
 - there is no need to use additional disk space for temporary decompression or to put entire FASTA files into memory,
 - for random access requests, computational resources are only spent on decompressing the region of interest,
 - it does not need multiple implementations of software libraries for each distinct tool and for each programming language,
 - it does not require to maintain multiple files that all together make up one data entity as it is guaranteed to provide dict- and fai-files that are in sync with their FASTA of origin.

In addition, the corresponding toolkit offers an interface that allows ENA sequence identification, file integrity verification and  management of the mounted files and process ids.

FASTAFS is deliberately made backwards compatible with both TwoBit and Fasta. The package even allows to mount TwoBit files instead of FASTAFS files, to FASTA files. For those who believe FASTAFS is this famous 15th standard (<https://xkcd.com/927/>)?
Partially, it is not designed to replace FASTA nor TwoBit as the mountpoints provide an exact identical way of file access as regular flat file acces, and is thus backwards compatible. Instead, it offers the same old standard with an elegant toolkit that allows easier integration with workflow management systems.

## Installation and compilation

Currently the package uses cmake for compilation
Required dependencies are:

 -   libboost (only for unit testing, will be come an optional dependency soon)
 -   libopenssl (for generating MD5 hashes)
 -   libfuse (for access to the fuse layer system and file virtualization)
 -   c++ compiler supporting c++-14

### For debian + ubuntu:

sudo apt install git build-essential cmake libboost-dev libssl-dev libboost-test-dev libboost-system-dev libboost-filesystem-dev zlib1g-dev libzstd-dev libfuse-dev

## First install libzstd:

This is currently done by running:

```
cd dependencies/zstd
make
sudo make install
```

Indeed, we should patch the cmake config to do this automatically.

## Compilation

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

# FASTAFS NAME: /home/youri/.local/share/fastafs/test.fastafs
# SEQUENCES:    7
chr1                    16          2bit    75255c6d90778999ad3643a2e69d4344
chr2                    16          2bit    8b5673724a9965c29a1d76fe7031ac8a
chr3.1                  13          2bit    61deba32ec4c3576e3998fa2d4b87288
chr3.2                  14          2bit    99b90560f23c1bda2871a6c93fd6a240
chr3.3                  15          2bit    3625afdfbeb43765b85f612e0acb4739
chr4                    8           2bit    bd8c080ed25ba8a454d9434cb8d14a68
chr5                    6           2bit    980ef3a1cd80afec959dcf852d026246
```

### fastafs mount: mount fastafs archive to unlock fasta file(s)

```
$ fastafs mount hg19 /mnt/fastafs/hg19 
$ ls /mnt/fastafs/hg19 
hg19.2bit  hg19.dict  hg19.fa  hg19.fa.fai  seq

$ ls -alsh /mnt/fastafs/hg19
total 0
-rw-r--r-- 1 youri youri 779M Aug 19 15:26 hg19.2bit
-rw-r--r-- 1 youri youri 7.9K Aug 19 15:26 hg19.dict
-rw-r--r-- 1 youri youri 3.0G Aug 19 15:26 hg19.fa
-rw-r--r-- 1 youri youri 3.5K Aug 19 15:26 hg19.fa.fai
drwxr-xr-x 1 youri youri    0 Aug 19 15:26 seq


$ head -n 5 /mnt/bio/hg19/hg19.dict 
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

To find the file size of chrM (16571):
```
$ ls -l /mnt/bio/hg19/seq/chrM

-rw-r--r-- 1 youri youri 16571 Feb  1 10:47 /mnt/bio/hg19/seq/chrM
```

### Find all running `fastafs mount` / `mount.fastafs` instances

The output format of `fastafs ps` is: `<pid>\t<source>\t<destination>\n`

```
$ fastafs ps
16383	/home/youri/.local/share/fastafs/test.fastafs	/mnt/tmp
```

### Mounting via fstab (for instance on linux boot)

You can add the following line(s) to /etc/fstab to make fastafs mount during boot:

```
mount.fastafs#/home/youri/.local/share/fastafs/hg19.fastafs /mnt/fastafs/hg19 fuse auto,allow_other 0 0
```

Here `mount.fastafs` refers to the binary that only does mounting, without the rest of the toolkit.
This is followed by a hash-tag and the path of the desired fastafs file. The next value is the mount point followed by the argument indicating it runs fuse.
The `auto,allow_other` refers to the `-o` argument.
Here `auto` ensures it is mounted automatically after boot.
Given that a system mounts as root user, the default permission of the mount point will be only for root. 
By setting `allow_other`, file system users get permissions to the mountpoint.

## Contributing
Feel free to start a discussion or to contribute to the GPL licensed code.
If you are willing to make even the smallest contribution to this project in any way, really, feel free to open an issue or to send an e-mail.

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/c90c7d61651d4e18aa82a4b02f3599fa)](https://www.codacy.com/app/yhoogstrate/fastafs?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=yhoogstrate/fastafs&amp;utm_campaign=Badge_Grade)
