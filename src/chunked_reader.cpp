
#include "chunked_reader.hpp"



chunked_reader::chunked_reader(char * afilename) :
    buffer_i(0), file_i(0)
{

    this->filename = realpath_cpp(afilename);
    this->set_filetype();
}

chunked_reader::chunked_reader(const char * afilename) :
    buffer_i(0), file_i(0)
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
