#!/bin/bash

#cmake -GNinja -DCMAKE_BUILD_TYPE=debug -DCMAKE_INSTALL_PREFIX=~/.local -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON .
#ninja -j`nproc`
#ninja install


## using make - sometimes much slower
cmake -DCMAKE_BUILD_TYPE=debug -DCMAKE_INSTALL_PREFIX=~/.local -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON .
make "$@" -j $(nproc)
make install
