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
//#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>      // presumes zstd library is installed
#include <zstd_errors.h>
#include <unistd.h>

#include <cstdlib> // random nr

 
#include "zstd_seekable.h"
#include "zstd_seekable_utils.hpp"


//#define MIN(a, b) ((a) < (b) ? (a) : (b))


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




size_t ZSTD_seekable_compressFile_orDie(const char* fname,
                                        const char* outName,
                                        int cLevel,
                                        unsigned int frameSize)
{
    size_t written = 0;

    FILE* const fin  = fopen_orDie(fname, "rb");
    FILE* const fout = fopen_orDie(outName, "wb");
    size_t const buffInSize = ZSTD_CStreamInSize();    /* can always read one full block */
    void*  const buffIn  = malloc_orDie(buffInSize);
    size_t const buffOutSize = ZSTD_CStreamOutSize();  /* can always flush a full block */
    void*  const buffOut = malloc_orDie(buffOutSize);

    ZSTD_seekable_CStream* const cstream = ZSTD_seekable_createCStream();
    if (cstream==NULL) { fprintf(stderr, "ZSTD_seekable_createCStream() error \n"); exit(10); }
    size_t const initResult = ZSTD_seekable_initCStream(cstream, cLevel, 1, frameSize);
    if (ZSTD_isError(initResult)) { fprintf(stderr, "ZSTD_seekable_initCStream() error : %s \n", ZSTD_getErrorName(initResult)); exit(11); }

    size_t read, toRead = buffInSize;
    while( (read = fread_orDie(buffIn, toRead, fin)) ) {
        ZSTD_inBuffer input = { buffIn, read, 0 };
        while (input.pos < input.size) {
            ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
            toRead = ZSTD_seekable_compressStream(cstream, &output , &input);   /* toRead is guaranteed to be <= ZSTD_CStreamInSize() */
            if (ZSTD_isError(toRead)) { fprintf(stderr, "ZSTD_seekable_compressStream() error : %s \n", ZSTD_getErrorName(toRead)); exit(12); }
            if (toRead > buffInSize) toRead = buffInSize;   /* Safely handle case when `buffInSize` is manually changed to a value < ZSTD_CStreamInSize()*/
            fwrite_orDie(buffOut, output.pos, fout);
        }
    }

    while (1) {
        ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
        size_t const remainingToFlush = ZSTD_seekable_endStream(cstream, &output);   /* close stream */
        if (ZSTD_isError(remainingToFlush)) { fprintf(stderr, "ZSTD_seekable_endStream() error : %s \n", ZSTD_getErrorName(remainingToFlush)); exit(13); }
        written += fwrite_orDie(buffOut, output.pos, fout);
        if (!remainingToFlush) break;
    }

    ZSTD_seekable_freeCStream(cstream);
    fclose_orDie(fout);
    fclose_orDie(fin);
    free(buffIn);
    free(buffOut);
    
    return written;
}





ZSTD_seekable_decompress_init_data* ZSTD_seekable_decompressFile_init(const char* fname)
{
    //FILE* const fin  = fopen_orDie(fname, "rb");
    
    return new ZSTD_seekable_decompress_init_data{
        fopen_orDie(fname, "rb") 
    };
}







size_t ZSTD_seekable_decompressFile_orDie(ZSTD_seekable_decompress_init_data* fh, off_t startOffset, char* buffer, off_t endOffset)
{
    //printf("[%i] Read raw zstd seekable [re-using fin] ... \n", msgid);
    size_t written = 0;
  
    if(fh->fin == NULL) {
        printf("fin == NULL: YES!!\n");
        exit(124);
    }
    //else {
    //   printf("[%i]  == NULL: no\n",msgid);
    //}

    if (feof(fh->fin)) {
      printf ("!!!! FEOF !!!!! \n");
      exit(123);
    }
    //else {
    //    printf ("[%i] no feof\n",msgid);
    //}
    //printf("[%i] ftell: %i\n",msgid, ftell(fh->fin));
    fseek_orDie(fh->fin,0, SEEK_SET);
    //printf("[%i] ftell post fseek: %i\n",msgid, ftell(fh->fin));
    

    size_t const buffOutSize = ZSTD_DStreamOutSize();  // Guarantee to successfully flush at least one complete compressed block in all circumstances.
    char* const buffOut = (char*) malloc_orDie(buffOutSize);

    ZSTD_seekable* const seekable = ZSTD_seekable_create();
    if (seekable==NULL) { fprintf(stderr, "ZSTD_seekable_create() error \n"); exit(10); }

    size_t const initResult = ZSTD_seekable_initFile(seekable, fh->fin);
    if (ZSTD_isError(initResult)) { fprintf(stderr, "ZSTD_seekable_init() error : %s \n", ZSTD_getErrorName(initResult)); exit(11); }

    size_t maxFileSize = ZSTD_seekable_getFileDecompressedSize(seekable);
    endOffset = std::min( (size_t) endOffset, maxFileSize); // avoid out of boundary requests

    size_t buffer_out_i = 0;
    while (startOffset < endOffset) {
        size_t const result = ZSTD_seekable_decompress(seekable, buffOut, std::min((size_t) endOffset - startOffset, buffOutSize), (size_t) startOffset);

        if (ZSTD_isError(result)) {
            fprintf(stderr, "ZSTD_seekable_decompress() error : %s \n",
                    ZSTD_getErrorName(result));
            exit(12);
        }

        for(size_t i = 0; i < result; i++) {
            buffer[buffer_out_i] = buffOut[i];
            buffer_out_i++;
        }

        startOffset += result;
        written += result;
    }

    ZSTD_seekable_free(seekable);
    //fclose_orDie(fin);
    free(buffOut);
    
    
    //fh->fin_locked = false;

    return written;
}






size_t ZSTD_seekable_decompressFile_orDie(const char* fname, off_t startOffset, char* buffer, off_t endOffset)
{
    printf("Read raw zstd seekable [+ new fin] ... \n");
    size_t written = 0;

    FILE* const fin  = fopen_orDie(fname, "rb");
    size_t const buffOutSize = ZSTD_DStreamOutSize();  // Guarantee to successfully flush at least one complete compressed block in all circumstances.
    char* const buffOut = (char*) malloc_orDie(buffOutSize);

    ZSTD_seekable* const seekable = ZSTD_seekable_create();
    if (seekable==NULL) { fprintf(stderr, "ZSTD_seekable_create() error \n"); exit(10); }

    size_t const initResult = ZSTD_seekable_initFile(seekable, fin);
    if (ZSTD_isError(initResult)) { fprintf(stderr, "ZSTD_seekable_init() error : %s \n", ZSTD_getErrorName(initResult)); exit(11); }

    size_t maxFileSize = ZSTD_seekable_getFileDecompressedSize(seekable);
    endOffset = std::min( (size_t) endOffset, maxFileSize); // avoid out of boundary requests

    size_t buffer_out_i = 0;
    while (startOffset < endOffset) {
        size_t const result = ZSTD_seekable_decompress(seekable, buffOut, std::min((size_t) endOffset - startOffset, buffOutSize), (size_t) startOffset);

        if (ZSTD_isError(result)) {
            fprintf(stderr, "ZSTD_seekable_decompress() error : %s \n",
                    ZSTD_getErrorName(result));
            exit(12);
        }

        for(size_t i = 0; i < result; i++) {
            buffer[buffer_out_i] = buffOut[i];
            buffer_out_i++;
        }

        startOffset += result;
        written += result;
    }

    ZSTD_seekable_free(seekable);
    fclose_orDie(fin);
    free(buffOut);

    return written;
}



