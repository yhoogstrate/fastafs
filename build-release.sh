#!/bin/bash

#cmake -GNinja -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON .
cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON .
make "$@" -j `nproc`
make install
