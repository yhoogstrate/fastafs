#!/bin/bash

g++ -o bin/zstd-s-c dependencies/zstd-seekable-adapted/seekable_compression.cpp dependencies/zstd-seekable-adapted/zstdseek_compress.c -I"dependencies/zstd/lib/common" -I"dependencies/zstd/lib" "dependencies/zstd/lib/libzstd.a"
