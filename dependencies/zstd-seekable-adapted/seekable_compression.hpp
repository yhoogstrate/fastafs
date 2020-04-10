/*
 * Copyright (c) 2017-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 */

#include <stdlib.h>    // malloc, free, exit, atoi
#include <stdio.h>     // fprintf, perror, feof, fopen, etc.
#include <string.h>    // strlen, memset, strcat
#include <string>    // strlen, memset, strcat
//#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>      // presumes zstd library is installed

#include "seekable_utils.hpp"
#include "zstd_seekable.h"

size_t ZSTD_seekable_compressFile_orDie(const char* fname, const char* outName, int cLevel, unsigned frameSize);

//static char* createOutFilename_orDie(const char* filename);

