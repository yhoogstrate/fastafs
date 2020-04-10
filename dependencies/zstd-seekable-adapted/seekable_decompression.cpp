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
#include "seekable_utils.hpp"

#define MIN(a, b) ((a) < (b) ? (a) : (b))


static void decompressFile_orDie(const char* fname, off_t startOffset, off_t endOffset)
{
    FILE* const fin  = fopen_orDie(fname, "rb");
    FILE* const fout = stdout;
    size_t const buffOutSize = ZSTD_DStreamOutSize();  /* Guarantee to successfully flush at least one complete compressed block in all circumstances. */
    void*  const buffOut = malloc_orDie(buffOutSize);

    ZSTD_seekable* const seekable = ZSTD_seekable_create();
    if (seekable==NULL) { fprintf(stderr, "ZSTD_seekable_create() error \n"); exit(10); }

    size_t const initResult = ZSTD_seekable_initFile(seekable, fin);
    if (ZSTD_isError(initResult)) { fprintf(stderr, "ZSTD_seekable_init() error : %s \n", ZSTD_getErrorName(initResult)); exit(11); }

    while (startOffset < endOffset) {
        size_t const result = ZSTD_seekable_decompress(seekable, buffOut, MIN(endOffset - startOffset, buffOutSize), startOffset);

        if (ZSTD_isError(result)) {
            fprintf(stderr, "ZSTD_seekable_decompress() error : %s \n",
                    ZSTD_getErrorName(result));
            exit(12);
        }
        fwrite_orDie(buffOut, result, fout);
        startOffset += result;
    }

    ZSTD_seekable_free(seekable);
    fclose_orDie(fin);
    fclose_orDie(fout);
    free(buffOut);
}


/*
int main(int argc, const char** argv)
{
    const char* const exeName = argv[0];

    if (argc!=4) {
        fprintf(stderr, "wrong arguments\n");
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "%s FILE START END\n", exeName);
        return 1;
    }

    {
        const char* const inFilename = argv[1];
        off_t const startOffset = atoll(argv[2]);
        off_t const endOffset = atoll(argv[3]);
        decompressFile_orDie(inFilename, startOffset, endOffset);
    }

    return 0;
}
*/
