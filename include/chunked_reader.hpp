

#ifndef CHUNKED_READER
#define CHUNKED_READER


#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>


#include "config.hpp"

#include "utils.hpp"

#include "zstd_seekable_utils.hpp"


enum compression_type : signed char {
    undefined = -1,
    uncompressed = 0,
    zstd = 1
};


//@todo implement w/ state design pattern
//url: https://refactoring.guru/design-patterns/state



class chunked_reader
{
private:
    std::ifstream *fh_flat;
    ZSTD_seekable_decompress_init_data* fh_zstd;

    std::string filename; // try doing this with inode
    
    compression_type filetype;
    
    char buffer[READ_BUFFER_SIZE + 1];
    size_t buffer_i;
    size_t buffer_n;

    off_t file_i;


public:
    chunked_reader(char *); // filename
    chunked_reader(const char *); // filename
    ~chunked_reader();

    void init(); // generic tasks needed for init

    void update_flat_buffer();
    void update_zstd_buffer();


    void find_filetype();

    void set_filetype(compression_type);
    compression_type get_filetype() { return this->filetype ; };

    size_t read(char *, size_t);// @deprecate
    size_t read(unsigned char *, size_t);
    unsigned char read();

    void seek(off_t);
    size_t tell();
    //size_t size();
};



#endif

