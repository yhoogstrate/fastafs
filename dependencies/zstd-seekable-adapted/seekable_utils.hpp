/*
 * Copyright (c) 2017-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 */

#include <string>    // strlen, memset, strcat


#include <stdlib.h>    // malloc, exit
#include <stdio.h>     // fprintf, perror, feof
#include <string.h>    // strerror
#include <errno.h>     // errno
//#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>      // presumes zstd library is installed
#include <zstd_errors.h>

#include "zstd_seekable.h"



void* malloc_orDie(size_t size);

FILE* fopen_orDie(const char *filename, const char *instruction);

size_t fread_orDie(void* buffer, size_t sizeToRead, FILE* file);

size_t fwrite_orDie(const void* buffer, size_t sizeToWrite, FILE* file);

size_t fclose_orDie(FILE* file);

void* realloc_orDie(void* ptr, size_t size);

void fseek_orDie(FILE* file, long int offset, int origin);



size_t ZSTD_seekable_compressFile_orDie(const char* fname, const char* outName, int cLevel, unsigned frameSize);

