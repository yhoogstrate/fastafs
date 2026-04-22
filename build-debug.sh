#!/bin/bash

#cmake -GNinja -DCMAKE_BUILD_TYPE=debug -DCMAKE_INSTALL_PREFIX=~/.local -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON .
#ninja -j`nproc`
#ninja install


## using make - sometimes much slower
# voeg toe later:  -fsanitize=address,undefined,leak  is echt enorm sloom
cmake -S . -B build-debug \
      -DCMAKE_BUILD_TYPE=debug \
      -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -O1 -fsanitize-address-use-after-scope -fno-omit-frame-pointer -D_GLIBCXX_DEBUG" \
      -DCMAKE_INSTALL_PREFIX=~/.local \
      -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
make -C build-debug "$@" -j $(nproc)
# installs to ~/.local; debug binaries: build-debug/bin/fastafs  tests: build-debug/test/
make -C build-debug install
