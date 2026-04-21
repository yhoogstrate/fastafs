# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Project Is

FASTAFS is a C++17 toolkit for FUSE-based virtualisation of random-access compressed FASTA files. It stores sequences in a compact binary archive format and mounts a virtual directory exposing standard FASTA, FAI, DICT, and 2BIT files — all synthesised on the fly without decompression to disk.

## Workflow

The user runs all compilation and test commands themselves. Do not run `make`, `cmake`, `bash build-*.sh`, or any build/test commands — only edit source files.

## Build System

Two out-of-source CMake builds are maintained in parallel. Always use these scripts, not in-source cmake.

**Debug build** (installs to `~/.local`):
```bash
bash build-debug.sh           # configure + build + install
make -C build-debug -j$(nproc)  # rebuild only
```
Debug flags include `-Wall -Wextra -Wpedantic -Wconversion -fsanitize-address-use-after-scope -D_GLIBCXX_DEBUG`. Debug binaries land in `build-debug/bin/`, tests in `build-debug/test/`.

**Release build** (installs to `/usr/local`):
```bash
bash build-release.sh
make -C build-release -j$(nproc)
```
Release binary: `build-release/bin/fastafs`.

When modifying `include/config.hpp.in`, CMake regenerates `build-*/include/config.hpp` automatically; never edit the generated file directly.

## Running Tests

```bash
# All tests
make -C build-debug test

# Single test binary
build-debug/test/test_byte_decoder
build-debug/test/test_fastafs

# With ctest
ctest --test-dir build-debug
ctest --test-dir build-debug -R test_byte_decoder   # single test by name
```

All test executables are Boost.Test binaries linked against `libfastafs`. Tests must be run from the repository root (CMake sets `WORKING_DIRECTORY` to `${CMAKE_SOURCE_DIR}`).

## Architecture

### File format
A `.fastafs` archive is a binary file. The `fastafs` class (see `src/fastafs.cpp`, `include/fastafs.hpp`) owns a vector of `fastafs_seq` objects, each describing one sequence: its name, nucleotide count `n`, data offset in the archive, N/M-block coordinates, and a `fastafs_sequence_flags` bitfield that encodes the encoding scheme.

### Encoding schemes
Sequences are packed using one of three per-sequence schemes selected by `fastafs_sequence_flags`:
- **twobit** (`twobit_byte`): 4 nucleotides/byte — DNA (ACGT) or RNA (ACGU).
- **fourbit** (`fourbit_byte`): IUPAC nucleotides including ambiguity codes.
- **fivebit** (`fivebit_fivebytes`): 8 amino acids in 5 bytes — protein sequences.

`byte_decoder` / `byte_encoder` provide a strategy-pattern interface over these three schemes. `byte_decoder` holds a `unique_ptr<byte_decoder_interface>` swapped via `set_strategy()`; concrete implementations (e.g. `byte_decoder_interface_twobit_dna`) override `decode_chunk()`.

### I/O layer
`chunked_reader` wraps zstd-seekable decompression: the archive may be zstd-compressed with a seek table, and `chunked_reader` exposes random-access byte reads into compressed blocks without loading the whole file.

### FUSE virtualisation
`src/fuse.cpp` implements the FUSE callbacks. When a virtual file is `read()`, it calls back into `fastafs_seq` to synthesise the appropriate bytes (FASTA nucleotides, FAI lines, DICT entries, 2bit data) for the requested byte range, decoding only the compressed chunks needed.

### Database
`database` (single file `src/database.cpp`) is a simple registry stored at `~/.local/share/fastafs/` mapping user-defined names to `.fastafs` archive paths.

### Dependencies
Bundled under `dependencies/`:
- `zstd-seekable-adapted/` — seekable zstd fork used by `chunked_reader`.
- `zstd-lib-common/xxhash.c` — xxHash used internally by zstd.

External: libssl/libcrypto (MD5), libfuse, zlib (CRC32), libboost (unit tests only).

## Build Notes

- The `-B` flag in cmake must immediately precede the build directory path: `cmake -S . -B build-debug …`. A trailing `.` after the path is a bug.
- Meson support exists (`meson.build`, `build-release-meson.sh`) but is secondary; prefer CMake.
- `make tidy` runs `astyle` for code formatting via `tidy.sh`.
