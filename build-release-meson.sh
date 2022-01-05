#!/bin/bash

# rm -rf bin-meson
meson bin-meson
cd bin-meson
ninja
