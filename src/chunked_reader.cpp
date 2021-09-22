
#include "chunked_reader.hpp"



chunked_reader::chunked_reader(char * afilename) :
    fh_flat(nullptr), fh_zstd(nullptr), file_i(0)
{
    this->filename = realpath_cpp(afilename);
    this->init();
}

chunked_reader::chunked_reader(const char * afilename) :
    fh_flat(nullptr), fh_zstd(nullptr), file_i(0)
{
    this->filename = realpath_cpp(afilename);
    this->init();
}

chunked_reader::~chunked_reader()
{
    //printf("[chunked_reader::~chunked_reader] exterminate, destroy(!)\n");

    if(this->fh_flat != nullptr) {
        if(this->fh_flat->is_open()) {
            this->fh_flat->close();
        }

        delete this->fh_flat;
    }

    if(this->fh_zstd != nullptr) {
        //ZSTD_seekable_free(this->fh_zstd->seekable);
        fclose_orDie(this->fh_zstd->fin);

        //delete this->fh_zstd->seekable;
        //delete this->fh_zstd->fin;

        delete this->fh_zstd;
    }
}

void chunked_reader::init()
{
    this->buffer_ii = this->buffer;
    this->buffer_nn = this->buffer;//was set to 0 before

    this->set_filetype();

    switch(this->filetype) {

    case uncompressed:
        this->fh_flat = new std::ifstream;
        this->fh_flat->open(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        if(this->fh_flat->is_open()) {
            this->fh_flat->seekg(0, std::ios::beg);
            this->update_flat_buffer();
        } else {
            throw std::runtime_error("[chunked_reader::init] Cannot open file for reading.\n");
        }
        break;

    case zstd:
        //printf("[chunked_reader::init()] - init ZSTD_seekable_decompress_init_data* fh_zstd; \n");
        this->fh_zstd = ZSTD_seekable_decompressFile_init(this->filename.c_str());
        // make zstd handle - to be implemented later on
        //ZSTD_seekable_decompress_data
        break;

    default:
        throw std::runtime_error("[chunked_reader::init] Should never happen - but avoids compiler warning.\n");
        break;
    }
}

void chunked_reader::set_filetype()
{
    if(is_zstd_file((const char*) this->filename.c_str())) {
        this->filetype = zstd;
    } else {
        this->filetype = uncompressed;
    }
}


size_t chunked_reader::read(char *arg_buffer, size_t buffer_size)
{
    char *arg_buffer_orig = arg_buffer;
    

    //=buffer_size = std::min(buffer_size, (size_t) READ_BUFFER_SIZE);
    size_t to_copy = std::min(buffer_size,  (size_t) (this->buffer_nn - this->buffer_ii));

    while(to_copy--) {
        *arg_buffer++ = *this->buffer_ii++;
    }



    /*
    size_t n = std::min(this->buffer_n - this->buffer_i, buffer_size - written);
    memcpy(&arg_buffer[written], &this->buffer[this->buffer_i] , n);
    written += n;
    this->buffer_i += n;
    */

    if(arg_buffer - arg_buffer_orig < buffer_size) {
        // overwrite buffer
        switch(this->filetype) {
        case uncompressed:
            this->update_flat_buffer();
            break;
        case zstd:
            this->update_zstd_buffer();
            break;
#if DEBUG
        default:
            throw std::runtime_error("[chunked_reader::read] reading from uninitialized object\n");
            break;
#endif
        }


        size_t to_copy = std::min(buffer_size,  (size_t) (this->buffer_nn - this->buffer_ii));

        while(to_copy--) {
            *arg_buffer++ = *this->buffer_ii++;
        }

        // same loop again
        //while(this->buffer_ii < this->buffer_nn and written < buffer_size) {
        //    arg_buffer[written++] = *this->buffer_ii++;
        //}
        /* - somehow memcpy is slightly slower - test again @ mom laptop
        size_t n = std::min(this->buffer_n - this->buffer_i, buffer_size - written);
        memcpy(&arg_buffer[written], &this->buffer[this->buffer_i] , n);
        written += n;
        this->buffer_i += n;
        */
    }

    return arg_buffer - arg_buffer_orig;
}





size_t chunked_reader::read(unsigned char *arg_buffer, size_t buffer_size)
{

    buffer_size = std::min(buffer_size, (size_t) READ_BUFFER_SIZE);
    size_t written = 0;

    while(this->buffer_ii < this->buffer_nn) {
        arg_buffer[written++] = *this->buffer_ii++;
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
        while(this->buffer_ii < this->buffer_nn and written < buffer_size) {
            arg_buffer[written++] = *this->buffer_ii++;
        }
    }

    return written;
}



// reads single byte from the buffer
unsigned char chunked_reader::read()
{
    if(this->buffer_ii >= this->buffer_nn) {
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
    }

    return *this->buffer_ii++;
}




void chunked_reader::update_flat_buffer()
{
    this->fh_flat->read(this->buffer, READ_BUFFER_SIZE);
    int count = this->fh_flat->gcount();

    this->buffer_ii = this->buffer;
    this->buffer_nn = this->buffer + count;
    
    this->file_i += count;
}


void chunked_reader::update_zstd_buffer()
{
    //size_t written = ZSTD_seekable_decompressFile_orDie(this->filename.c_str(), this->file_i,  this->buffer, this->file_i + READ_BUFFER_SIZE);
    size_t written = ZSTD_seekable_decompressFile_orDie(this->fh_zstd, this->file_i,  this->buffer, this->file_i + READ_BUFFER_SIZE);

    this->buffer_ii = this->buffer;
    
    this->buffer_nn = this->buffer + written;
    this->file_i += written;
}



void chunked_reader::seek(off_t offset)
{
    this->file_i = offset;

    switch(this->filetype) {
    case uncompressed:
        this->fh_flat->clear(); // reset error state

        if(!this->fh_flat->is_open()) {
            this->fh_flat->open(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
        }

        this->fh_flat->seekg(offset, std::ios::beg);
        this->update_flat_buffer();
        break;
    default:
        this->update_zstd_buffer();
        break;
    }
}


size_t chunked_reader::tell()
{
        //10 - 5  + 2 = 7
        //10 + (5 - 3)
        
    //return this->file_i - this->buffer_n + this->buffer_i;
    
    return this->file_i + (this->buffer_ii - this->buffer_nn);
}

