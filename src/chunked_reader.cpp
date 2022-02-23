
#include "chunked_reader.hpp"



chunked_reader::chunked_reader(char * afilename) :
    fh_flat(nullptr), fh_zstd(nullptr), buffer_i(0), buffer_n(0), file_i(0)
{

    this->filename = realpath_cpp(afilename);
    this->init();
}

chunked_reader::chunked_reader(const char * afilename) :
    fh_flat(nullptr), fh_zstd(nullptr), buffer_i(0), buffer_n(0), file_i(0)
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
    this->find_filetype();

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

void chunked_reader::find_filetype()
{
    if(is_zstd_file((const char*) this->filename.c_str())) {
        this->set_filetype(zstd);
    } else {
        this->set_filetype(uncompressed);
    }
}

void chunked_reader::set_filetype(compression_type arg_filetype)
{
    this->filetype = arg_filetype;
}


void set_filtetype(compression_type &filetype_arg);


size_t chunked_reader::read(char *arg_buffer, size_t buffer_size)
{


    buffer_size = std::min(buffer_size, (size_t) READ_BUFFER_SIZE);
    size_t written = 0;

    while(this->buffer_i < this->buffer_n and written < buffer_size) {
        arg_buffer[written++] = this->buffer[this->buffer_i++];
    }



    /*
    size_t n = std::min(this->buffer_n - this->buffer_i, buffer_size - written);
    memcpy(&arg_buffer[written], &this->buffer[this->buffer_i] , n);
    written += n;
    this->buffer_i += n;
    */

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
            arg_buffer[written++] = this->buffer[this->buffer_i++];
        }
        /* - somehow memcpy is slightly slower - test again @ mom laptop
        size_t n = std::min(this->buffer_n - this->buffer_i, buffer_size - written);
        memcpy(&arg_buffer[written], &this->buffer[this->buffer_i] , n);
        written += n;
        this->buffer_i += n;
        */
    }

    return written;
}





size_t chunked_reader::read(unsigned char *arg_buffer, size_t buffer_size)
{


    buffer_size = std::min(buffer_size, (size_t) READ_BUFFER_SIZE);
    size_t written = 0;

    while(this->buffer_i < this->buffer_n and written < buffer_size) {
        arg_buffer[written++] = this->buffer[this->buffer_i++];
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
            arg_buffer[written++] = this->buffer[this->buffer_i++];
        }
    }

    return written;
}



// reads single byte from the buffer
unsigned char chunked_reader::read()
{
    if(this->buffer_i >= this->buffer_n) {
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

    return this->buffer[this->buffer_i++];
}




void chunked_reader::update_flat_buffer()
{
    this->fh_flat->read(this->buffer, READ_BUFFER_SIZE);

    this->buffer_i = 0;
    this->buffer_n = (size_t) this->fh_flat->gcount();
    this->file_i += this->buffer_n;
}


void chunked_reader::update_zstd_buffer()
{
    //size_t written = ZSTD_seekable_decompressFile_orDie(this->filename.c_str(), this->file_i,  this->buffer, this->file_i + READ_BUFFER_SIZE);
    size_t written = ZSTD_seekable_decompressFile_orDie(this->fh_zstd, this->file_i,  this->buffer, this->file_i + READ_BUFFER_SIZE);

    this->buffer_i = 0;
    this->buffer_n = written;
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
    return this->file_i - this->buffer_n + this->buffer_i;
}



void State::set_context(Context *arg_context)
{
    this->context = arg_context;
}




Context::Context(const char * arg_filename) : filename(arg_filename), buffer("\0"), buffer_i(0), buffer_n(0), file_i(0), state(nullptr)
{
    printf("Constructor alive\n");

    this->TransitionTo(this->find_state());
}

const std::string& Context::get_filename()
{
    return this->filename;
}

char * Context::get_buffer()
{
    return &(this->buffer[0]);
}

size_t Context::cache_buffer()
{
    
    printf("[x] this->buffer_i = %i, this->buffer_n = %i, this->file_i = %i\n",this->buffer_i, this->buffer_n, this->file_i);
    
    size_t s = this->state->cache_buffer();
    printf("<s=%i>\n",s);
    this->buffer_n = s;

    this->buffer_i = 0;
    this->file_i += s;
    
    printf("[y] this->buffer_i = %i, this->buffer_n = %i, this->file_i = %i\n",this->buffer_i, this->buffer_n, this->file_i);
}

size_t Context::read(char *arg_buffer, size_t arg_buffer_size)
{
    //arg_buffer_size = std::min(arg_buffer_size, (size_t) READ_BUFFER_SIZE);
#if DEBUG
    if(arg_buffer_size > READ_BUFFER_SIZE)
    {
        throw std::runtime_error("[ContextUncompressed::read] Requested buffer size larger than internal context buffer.\n");
    }
#endif //DEBUG

    return this->state->read(arg_buffer, arg_buffer_size, this->buffer_i, this->buffer_n);
}


void Context::TransitionTo(State *arg_state) {
    std::cout << "Context: Transition to " << typeid(*arg_state).name() << ".\n";
    
    if (this->state != nullptr)
    {
        delete this->state; // delete and destruct previous state, incl file points, should also run fh.close(); etc.
    }

    this->state = arg_state;
    this->state->set_context(this);
}

void Context::fopen(off_t file_offset)
{
    printf("[1] this->buffer_i = %i, this->buffer_n = %i, this->file_i = %i\n",this->buffer_i, this->buffer_n, this->file_i);
    this->state->fopen(file_offset);
    printf("[2] this->buffer_i = %i, this->buffer_n = %i, this->file_i = %i\n",this->buffer_i, this->buffer_n, this->file_i);
    this->cache_buffer();
    printf("[3] this->buffer_i = %i, this->buffer_n = %i, this->file_i = %i\n",this->buffer_i, this->buffer_n, this->file_i);
}





State * Context::find_state()
{
    if(is_zstd_file(this->filename.c_str()))
    {
        return new ContextZstdSeekable;
    }
    else
    {
        return new ContextUncompressed;
    }
}


void ContextUncompressed::fopen(off_t start_pos = 0)
{
    if(this->fh != nullptr)
    {
        throw std::runtime_error("[ContextUncompressed::fopen] opening a non closed reader.\n");
    }

    this->fh = new std::ifstream;
    this->fh->open(this->context->get_filename().c_str(), std::ios::in | std::ios::binary | std::ios::ate);


    if(this->fh->is_open()) {
        this->fh->seekg(start_pos, std::ios::beg);
    } else {
        throw std::runtime_error("[chunked_reader::init] Cannot open file for reading.\n");
    }
}

size_t ContextUncompressed::cache_buffer()
{
    this->fh->read(this->context->get_buffer(), READ_BUFFER_SIZE);

    if(!this->fh)
    {
        printf("[%s]\n", this->context->get_buffer());
        throw std::runtime_error("[ContextUncompressed::cache_buffer] Coult not open file. \n");
    }
    
    printf("ContextUncompressed::cache_buffer\n");

    size_t s = (size_t) this->fh->gcount();
    printf("[s=%i]\n",s);

    return s;
}

size_t ContextUncompressed::read(char *arg_buffer_to, size_t arg_buffer_to_size,
            size_t &buffer_i, size_t &buffer_n)
{
#if DEBUG
    if(arg_buffer_to_size > READ_BUFFER_SIZE)
    {
        throw std::runtime_error("[ContextUncompressed::read] Requested buffer size larger than internal context buffer.\n");
    }
#endif //DEBUG

    size_t written = 0;
    size_t n = std::min(buffer_n - buffer_i, arg_buffer_to_size);
    
    printf("buffer_n = %i, buffer_i = %i, arg_buffer_to_size = %i, n = %i\n",buffer_n, buffer_i, arg_buffer_to_size, n);

    // copy current internal buffer completely
    while(written < n)
    {
        arg_buffer_to[written++] = this->context->get_buffer()[buffer_i++];
    }

    if(written < arg_buffer_to_size) {
        //

        // same loop again
        /*
        while(this->buffer_i < this->buffer_n and written < buffer_size) {
            arg_buffer[written++] = this->buffer[this->buffer_i++];
        } */
        printf("recursively call another read\n");
    }

    return written;
}

ContextUncompressed::~ContextUncompressed()
{
    if(this->fh != nullptr)
    {
        if(!this->fh)
        {
            this->fh->close();
            throw std::runtime_error("[ContextUncompressed::~ContextUncompressed] unexpected closed filehandle found.\n");
        }
        else
        {
            this->fh->close();
        }

        delete this->fh;
    }
}




size_t ContextZstdSeekable::cache_buffer() {
    printf("hello ZstdSeek\n");
    
    return 0;
}

void ContextZstdSeekable::fopen(off_t start_pos)
{
    throw std::runtime_error("[ContextZstdSeekable::fopen] not implemented.\n");
}

size_t ContextZstdSeekable::read(char *arg_buffer_to, size_t arg_buffer_to_size,
            size_t &buffer_i, size_t &buffer_n)
{
    return 0;
}

