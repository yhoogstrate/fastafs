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




#ifndef ZSTD_SEEKABLE_DECOMPRESS_INIT_DATA_HPP
#define ZSTD_SEEKABLE_DECOMPRESS_INIT_DATA_HPP

// struct for already initialised reading
struct ZSTD_seekable_decompress_init_data {
    FILE* fin;
    //bool fin_locked;
    
    //ZSTD_seekable* seekable;
    
    // size_t const initResult; // to be added later if always the same?
    // size_t maxFileSize; // to be added later if always the same?
};

#endif


ZSTD_seekable_decompress_init_data* ZSTD_seekable_decompressFile_init(const char* );
size_t ZSTD_seekable_decompressFile_orDie(ZSTD_seekable_decompress_init_data* , off_t, char* , off_t);
size_t ZSTD_seekable_decompressFile_orDie(const char* , off_t , char *, off_t );
