/*
 * Copyright (c) 2017-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 */

#include <string>    // strlen, memset, strcat

/*
#include <stdlib.h>    // malloc, free, exit, atoi
#include <stdio.h>     // fprintf, perror, feof, fopen, etc.
#include <string.h>    // strlen, memset, strcat

//#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>      // presumes zstd library is installed

#include "zstd_seekable.h"*/
#include "seekable_utils.hpp"
//#include "seekable_compression.hpp"

void* malloc_orDie(size_t size)
{
    void* const buff = malloc(size);
    if (buff) return buff;
    /* error */
    perror("malloc:");
    exit(1);
}


FILE* fopen_orDie(const char *filename, const char *instruction)
{
    FILE* const inFile = fopen(filename, instruction);
    if (inFile) return inFile;
    /* error */
    perror(filename);
    exit(3);
}


size_t fread_orDie(void* buffer, size_t sizeToRead, FILE* file)
{
    size_t const readSize = fread(buffer, 1, sizeToRead, file);
    if (readSize == sizeToRead) return readSize;   /* good */
    if (feof(file)) return readSize;   /* good, reached end of file */
    /* error */
    perror("fread");
    exit(4);
}


size_t fwrite_orDie(const void* buffer, size_t sizeToWrite, FILE* file)
{
    size_t const writtenSize = fwrite(buffer, 1, sizeToWrite, file);
    if (writtenSize == sizeToWrite) return sizeToWrite;   /* good */
    /* error */
    perror("fwrite");
    exit(5);
}


size_t fclose_orDie(FILE* file)
{
    if (!fclose(file)) return 0;
    /* error */
    perror("fclose");
    exit(6);
}


char* createOutFilename_orDie(const char* filename)
{
    std::string newname = std::string(filename) + ".zst";
    return (char*) newname.c_str();
}

/*
 * Copyright (c) 2017-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 */


#include <stdlib.h>    // malloc, exit
#include <stdio.h>     // fprintf, perror, feof
#include <string.h>    // strerror
#include <errno.h>     // errno
#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>      // presumes zstd library is installed
#include <zstd_errors.h>

#include "zstd_seekable.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))


void* realloc_orDie(void* ptr, size_t size)
{
    ptr = realloc(ptr, size);
    if (ptr) return ptr;
    /* error */
    perror("realloc");
    exit(1);
}





void fseek_orDie(FILE* file, long int offset, int origin) {
    if (!fseek(file, offset, origin)) {
        if (!fflush(file)) return;
    }
    /* error */
    perror("fseek");
    exit(7);
}


