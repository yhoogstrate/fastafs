#!/bin/bash

cmake -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=~/.local -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON .
make "$@"
make install
