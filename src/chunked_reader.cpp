
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



size_t chunked_reader::read(char *arg_buffer, size_t buffer_size) {
    buffer_size = std::min(buffer_size, (size_t) READ_BUFFER_SIZE);
    size_t written = 0;

    // first read
    // chunked buf : [  . . . . . i . .. . . . .n  - -- - - -  ]
    while(this->buffer_i < this->buffer_n and written < buffer_size) {
        arg_buffer[written] = this->buffer[this->buffer_i];
        
        this->buffer_i++;
        written++;
    }
    
    if(written < buffer_size) {
        // overwrite buffer
        switch(this->filetype) {
            case uncompressed:
                printf("reading another 1024 characters from a flat file");
                // set this->buffer_i = 0;
                // set this->buffer_n = ???;
            break;
            case zstd:
                printf("reading another 1024 characters from a zstd file");
            break;
            default:
                throw std::runtime_error("[chunked_reader::read] reading from uninitialized object\n");
                exit(1);
            break;
        }

        // same loop again
        while(this->buffer_i < this->buffer_n and written < buffer_size) {
            arg_buffer[written] = this->buffer[this->buffer_i];
            
            this->buffer_i++;
            written++;
        }
    }
    
    return written;
}

