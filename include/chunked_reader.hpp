

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
    zstdseekable = 1
};




class chunked_reader
{
public:
    chunked_reader(char *); // filename
    chunked_reader(const char *); // filename

    std::string filename; // try doing this with inode
    
    compression_type filetype;
    char buffer[READ_BUFFER_SIZE + 1];
    size_t buffer_i;
    off_t file_i;

    void set_filetype();
    void read();
    size_t size();
};



#endif

