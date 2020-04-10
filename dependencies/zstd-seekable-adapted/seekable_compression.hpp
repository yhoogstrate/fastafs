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

#include "zstd_seekable.h"

static void* malloc_orDie(size_t size);

static FILE* fopen_orDie(const char *filename, const char *instruction);

static size_t fread_orDie(void* buffer, size_t sizeToRead, FILE* file);

static size_t fwrite_orDie(const void* buffer, size_t sizeToWrite, FILE* file);

static size_t fclose_orDie(FILE* file);

size_t ZSTD_seekable_compressFile_orDie(const char* fname, const char* outName, int cLevel, unsigned frameSize);

static char* createOutFilename_orDie(const char* filename);

