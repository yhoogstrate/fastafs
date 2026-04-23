#!/bin/bash

cmake -S . -B build-release \
      -DCMAKE_BUILD_TYPE=release \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
make -C build-release "$@" -j $(nproc)
make -C build-release install
# release binary: build-release/bin/fastafs
