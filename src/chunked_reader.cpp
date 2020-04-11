
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

void chunked_reader::init() {
    this->set_filetype();
    
    switch(this->filetype) {
        case uncompressed:
            printf("set file handle for flat file: %s\n",this->filename.c_str());
            fh_flat.open(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);            
            //size = fh_flat.tellg();
            fh_flat.seekg(0, std::ios::beg);
            
            printf("checkpoint A\n");
            if(fh_flat.is_open()) {
                printf("OPEN\n");

                this->update_flat_buffer();
            }
            else {
                printf("NOT OPEN??\n");
            }
        break;
        case zstd:
            // make zstd handle - to be implemented later on
        break;
    }

    printf("checkpoint D\n");
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
        printf("|%c|", this->buffer[this->buffer_i]);

        this->buffer_i++;
        written++;
    }
    printf("\n");
    
    if(written < buffer_size) {
        // overwrite buffer
        switch(this->filetype) {
            case uncompressed:
                this->update_flat_buffer();
            break;
            case zstd:
                printf("reading another 1024 characters from a zstd file");
            break;
            default:
                throw std::runtime_error("[chunked_reader::read] reading from uninitialized object\n");
                exit(1);
            break;
        }
        printf("------------\n");

        // same loop again
        while(this->buffer_i < this->buffer_n and written < buffer_size) {
            arg_buffer[written] = this->buffer[this->buffer_i];
            printf("{%c}", this->buffer[this->buffer_i]);
            
            this->buffer_i++;
            written++;
        }
    }
    
    // does not happen in regular read either
    //arg_buffer[written] = '\0';
    
    return written;
}


void chunked_reader::update_flat_buffer() {
    printf("reading another 1024 characters from a flat file");

    for(int i = 0; i < 110; i++) {
        this->buffer[i] = '\0';
    }

    printf("[[%s]]1\n",this->buffer);
    fh_flat.read(this->buffer, 0);
    printf("read 0, strlen=%i\n", strlen(buffer));


    fh_flat.read(this->buffer, 1);
    printf("read 1, strlen=%i\n", strlen(buffer));


    fh_flat.read(this->buffer, 10);
    printf("read 10, strlen=%i\n", strlen(buffer));


    fh_flat.read(this->buffer, 100);
    printf("read 100, strlen=%i\n", strlen(buffer));

    printf("[[%s]]2\n",this->buffer);

    this->buffer_i = 0;
    this->buffer_n = (size_t) fh_flat.gcount();
    printf("gcount: %i\n", buffer_n);
    this->file_i += this->buffer_n;
}

