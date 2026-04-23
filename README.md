# 🧬 FastaFS

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17%2F20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B)
[![License: GPL-2.0](https://img.shields.io/badge/License-GPL--2.0-blue.svg)](https://opensource.org/licenses/GPL-2.0)
[![DOI](https://img.shields.io/badge/DOI-10.1186%2Fs12859--021--04455--3-blue.svg)](https://doi.org/10.1186/s12859-021-04455-3)

**FastaFS lets you mount compressed FASTA archives as a virtual filesystem — enabling instant random access without preprocessing, indexing, or duplication.**

---

## ❓ Why FastaFS?

Working with large FASTA files is inefficient and error-prone:

- Requires auxiliary files (`.fai`, `.dict`)
- Random access needs preprocessing or indexing
- Tools expect flat files, not compressed archives
- Storage is duplicated across pipelines
- Data and metadata can get out of sync

**FastaFS solves this by turning compressed FASTA archives into a mountable filesystem.**

---

## 🔥 Key Features

- ⚡ **Near-native performance** – optimized C++ backend with minimal overhead  
- 🧠 **No on-demand preprocessing required** – skip indexing and loading into memory  
- 📂 **Works with existing tools** – use `grep`, `awk`, `samtools`, etc.  
- 💾 **Efficient storage** – no duplicate FASTA files or temporary extraction  
- 🔌 **Mount as a filesystem** – interact like regular files  
- 🔄 **Preserves compatibility** – fully compatible with existing FASTA-based workflows and tooling  
- 🎯 **Selective decompression** – only access the regions you need  

---

## 🚀 Quick Start

```bash
# Clone
git clone https://github.com/yhoogstrate/fastafs.git
cd fastafs

# Build
./build-release.sh
make check

# Cache + mount
./fastafs cache reference ./reference.fa
./fastafs mount reference /mnt/genome

# Use like normal files
ls /mnt/genome
head /mnt/genome/chr1.fa
```

---

## 🧠 How it works

FastaFS introduces a virtual filesystem layer using FUSE.

When mounted:

- FASTA and metadata files are generated on-the-fly  
- Only requested regions are decompressed  
- `.fa`, `.fai`, `.dict`, and `.2bit` stay perfectly in sync  

➡️ No temporary files. No duplication. No indexing overhead.

---

## 🧪 Use Cases

- Large-scale genomics pipelines  
- HPC environments with limited I/O bandwidth  
- Streaming access to reference genomes  
- Toolchains requiring standard FASTA input  
- Reproducible workflows

---

## 📄 File Format Specification

https://github.com/yhoogstrate/fastafs/blob/master/doc/FASTAFS-FORMAT-SPECIFICATION.md

---

## 🔗 Links

https://bio.tools/fastafs
https://github.com/facebook/zstd/blob/dev/contrib/seekable_format/zstd_seekable_compression_format.md

---

## 🧰 Installation

### Dependencies

- libboost (unit testing)
- libopenssl / libssl
- libfuse
- zlib / libzstd
- C++ compiler (C++14+)
- cmake or meson + ninja

---

### Debian / Ubuntu

```bash
sudo apt install git build-essential cmake libboost-dev libssl-dev \
libboost-test-dev libboost-system-dev libboost-filesystem-dev \
zlib1g-dev libzstd-dev libfuse-dev

git clone https://github.com/yhoogstrate/fastafs.git
cd fastafs
```

---

### RHEL / CentOS / Fedora

```bash
sudo yum install git cmake gcc-c++ boost-devel openssl-devel \
libzstd-devel zlib-devel fuse-devel

git clone https://github.com/yhoogstrate/fastafs.git
cd fastafs
```

---

### Compile (recommended)

```bash
cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=/usr/local .
make -j $(nproc)
sudo make install
```

Without root:

```bash
cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=~/.local .
make -j $(nproc)
make install
```

---

## ⚙️ Usage

### Add FASTA to cache

```bash
fastafs cache test ./test.fa
```

Or from 2bit:

```bash
fastafs cache test ./test.2bit
```

---

### List cached datasets

```bash
fastafs list
```

---

### Mount archive

```bash
fastafs mount hg19 /mnt/fastafs/hg19
ls /mnt/fastafs/hg19
```

---

### Inspect sequences

```bash
fastafs info
```

---

### Running mounts

```bash
fastafs ps
```

---

### Mount via fstab

```bash
mount.fastafs#/path/to/file.fastafs /mnt/fastafs fuse auto,allow_other 0 0
```

---

## 📚 Citation

If you use FastaFS in your research, please cite:

> Hoogstrate, Y., Jenster, G.W. & van de Werken, H.J.G.  
> **FASTAFS: file system virtualisation of random access compressed FASTA files.**  
> BMC Bioinformatics 22, 535 (2021).  
> https://doi.org/10.1186/s12859-021-04455-3

---

## 🤝 Contributing

Contributions are welcome!

- Open an issue  
- Submit a pull request  

Format code with:

```bash
make tidy
```

---

## 💡 Final note

FastaFS does not replace FASTA or TwoBit — it **enhances them** by making them easier to use, more efficient, and seamlessly integrated into existing workflows.
