#!/bin/bash

# rm -rf bin-meson
rm include/config.hpp
meson bin-meson
cd bin-meson
ninja
