

#ifndef CHUNKED_READER
#define CHUNKED_READER


#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>


#include "config.hpp"

#include "utils.hpp"


enum compression_type : signed char
{
    undefined = -1,
    uncompressed = 0,
    zstd = 1
};




class chunked_reader
{
public:
    chunked_reader(char *); // filename
    chunked_reader(const char *); // filename
    //~chunked_reader(); seems to do nasty with the ifstream in this class?
    
    void init(); // generic tasks needed for init

    std::string filename; // try doing this with inode

    std::ifstream fh_flat;
    void update_flat_buffer();
    
    compression_type filetype;
    char buffer[READ_BUFFER_SIZE + 1];
    size_t buffer_i;
    size_t buffer_n;

    off_t file_i;

    void set_filetype();
    size_t read(char *, size_t);
    size_t size();
};



#endif

