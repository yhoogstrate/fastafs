
#include "chunked_reader.hpp"



chunked_reader::chunked_reader(char * afilename) :
    buffer_i(0), buffer_n(0), file_i(0)
{

    this->filename = realpath_cpp(afilename);
    this->init();
}

chunked_reader::chunked_reader(const char * afilename) :
    buffer_i(0), buffer_n(0), file_i(0)
{
    this->filename = realpath_cpp(afilename);
    this->init();
}

/*
chunked_reader::~chunked_reader() {
    if(this->fh_flat.is_open()) {
        this->fh_flat.close();
    }
}
*/

void chunked_reader::init() {
    this->set_filetype();
    
    switch(this->filetype) {
        case uncompressed:
            fh_flat.open(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);            
            
            if(fh_flat.is_open()) {
                fh_flat.seekg(0, std::ios::beg);
                this->update_flat_buffer();
            }
            else {
                throw std::runtime_error("[chunked_reader::init] Cannot open file for reading.\n");
            }
        break;
        case zstd:
            // make zstd handle - to be implemented later on
            printf("fake init zstd\n");
        break;
        default:
            throw std::runtime_error("[chunked_reader::init] Should never happen - but avoids compiler warning.\n");
        break;
    }
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
                this->update_flat_buffer();
            break;
            case zstd:
                this->update_zstd_buffer();
            break;
            default:
                throw std::runtime_error("[chunked_reader::read] reading from uninitialized object\n");
            break;
        }

        // same loop again
        while(this->buffer_i < this->buffer_n and written < buffer_size) {
            arg_buffer[written] = this->buffer[this->buffer_i];

            this->buffer_i++;
            written++;
        }
    }
    
    if(written == 0 and this->fh_flat.is_open()) {
        this->fh_flat.close();
    }

    return written;
}


void chunked_reader::update_flat_buffer() {
    this->fh_flat.read(this->buffer, READ_BUFFER_SIZE);

    this->buffer_i = 0;
    this->buffer_n = (size_t) fh_flat.gcount();
    this->file_i += this->buffer_n;
}


void chunked_reader::update_zstd_buffer() {
    printf("should set 1st 1024 byts from th zstd file\n");
    
    size_t written = ZSTD_seekable_decompressFile_orDie(this->filename.c_str(), this->file_i,  this->buffer, this->file_i + READ_BUFFER_SIZE);

    this->buffer_i = 0;
    this->buffer_n = written;
    this->file_i += written;
    
    printf("written: %i\n",written);
}


