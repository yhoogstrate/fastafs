
#include "chunked_reader.hpp"



chunked_reader::chunked_reader(char * afilename) :
    buffer_i(0), buffer_n(0), file_i(0)
{

    this->filename = realpath_cpp(afilename);
    this->set_filetype();
}

chunked_reader::chunked_reader(const char * afilename) :
    buffer_i(0), buffer_n(0), file_i(0)
{
    this->filename = realpath_cpp(afilename);
    this->set_filetype();
}



void chunked_reader::set_filetype() {
    if(is_zstd_file((const char*) this->filename.c_str()) ) {
        this->filetype = zstd;
    }
    else {
        this->filetype = uncompressed;
    }
}



size_t chunked_reader::read(char *buffer, size_t buffer_size) {
    buffer_size = std::min(buffer_size, READ_BUFFER_SIZE);
    size_t written = 0;

    // first read
    // chunked buf : [  . . . . . i . .. . . . .n  - -- - - -  ]
    
    
    return written;
}

