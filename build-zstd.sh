#!/bin/bash

g++ -o zstd-s-c seekable_compression.c zstdseek_compress.c -I"dependencies/zstd/lib/common" -I"dependencies/zstd/lib" "dependencies/zstd/lib/libzstd.a"
