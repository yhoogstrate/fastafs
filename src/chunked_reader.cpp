
#include "chunked_reader.hpp"



chunked_reader_old::chunked_reader_old(char * afilename) :
    fh_flat(nullptr), fh_zstd(nullptr), buffer_i(0), buffer_n(0), file_i(0)
{
    this->filename = realpath_cpp(afilename);
    this->init();
}

chunked_reader_old::chunked_reader_old(const char * afilename) :
    fh_flat(nullptr), fh_zstd(nullptr), buffer_i(0), buffer_n(0), file_i(0)
{
    this->filename = realpath_cpp(afilename);
    this->init();
}

chunked_reader_old::~chunked_reader_old()
{
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

void chunked_reader_old::init()
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
            throw std::runtime_error("[chunked_reader_old::init] Cannot open file for reading.\n");
        }
        break;

    case zstd:
        //printf("[chunked_reader_old::init()] - init ZSTD_seekable_decompress_init_data* fh_zstd; \n");
        this->fh_zstd = ZSTD_seekable_decompressFile_init(this->filename.c_str());
        // make zstd handle - to be implemented later on
        //ZSTD_seekable_decompress_data
        break;

    default:
        throw std::runtime_error("[chunked_reader_old::init] Should never happen - but avoids compiler warning.\n");
        break;
    }
}

void chunked_reader_old::find_filetype()
{
    if(is_zstd_file((const char*) this->filename.c_str())) {
        this->set_filetype(zstd);
    } else {
        this->set_filetype(uncompressed);
    }
}

void chunked_reader_old::set_filetype(compression_type arg_filetype)
{
    this->filetype = arg_filetype;
}


void set_filtetype(compression_type &filetype_arg);


size_t chunked_reader_old::read(char *arg_buffer, size_t buffer_size)
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
            throw std::runtime_error("[chunked_reader_old::read] reading from uninitialized object\n");
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





size_t chunked_reader_old::read(unsigned char *arg_buffer, size_t buffer_size)
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
            throw std::runtime_error("[chunked_reader_old::read] reading from uninitialized object\n");
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
unsigned char chunked_reader_old::read()
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
            throw std::runtime_error("[chunked_reader_old::read] reading from uninitialized object\n");
            break;
        }
    }

    return this->buffer[this->buffer_i++];
}




void chunked_reader_old::update_flat_buffer()
{
    this->fh_flat->read(this->buffer, READ_BUFFER_SIZE);

    this->buffer_i = 0;
    this->buffer_n = (size_t) this->fh_flat->gcount();
    this->file_i += this->buffer_n;
}


void chunked_reader_old::update_zstd_buffer()
{
    //size_t written = ZSTD_seekable_decompressFile_orDie(this->filename.c_str(), this->file_i,  this->buffer, this->file_i + READ_BUFFER_SIZE);
    size_t written = ZSTD_seekable_decompressFile_orDie(this->fh_zstd, this->file_i,  this->buffer, this->file_i + READ_BUFFER_SIZE);

    this->buffer_i = 0;
    this->buffer_n = written;
    this->file_i += written;
}



void chunked_reader_old::seek(off_t offset)
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


size_t chunked_reader_old::tell()
{
    //@todo decide what to return when out of bound
    //e.g. when exceeding file size

    return this->file_i - this->buffer_n + this->buffer_i;
}



void State::set_context(chunked_reader *arg_context)
{
    this->context = arg_context;
}

// This does not read the actual flat file, this copies its internal buffer to arg_buffer_to
size_t State::read(unsigned char *arg_buffer_to, size_t arg_buffer_to_size,
            size_t &buffer_i, size_t &buffer_n)
{
#if DEBUG
    if(arg_buffer_to_size > READ_BUFFER_SIZE)
    {
        throw std::runtime_error("[ContextUncompressed::read] Requested buffer size larger than internal context buffer.\n");
    }
#endif //DEBUG

    size_t written = 0;
    const size_t n1 = std::min(buffer_n - buffer_i, arg_buffer_to_size);// number of characters to copy
    
    // copy current internal buffer completely
    while(written < n1)
    {
        arg_buffer_to[written++] = this->context->get_buffer()[buffer_i++];
    }

    if(written < arg_buffer_to_size)
    {
        this->context->cache_buffer();// needs to set n to 0

        while(buffer_i < buffer_n and written < arg_buffer_to_size)
        {
            arg_buffer_to[written++] = this->context->get_buffer()[buffer_i++];
        }
    }


    return written;
}





chunked_reader::chunked_reader(const char * arg_filename) : filename(arg_filename), buffer("\0"), buffer_i(0), buffer_n(0), file_i(0), state(nullptr)
{
    this->TransitionTo(this->find_state());
}

chunked_reader::~chunked_reader()
{
    delete this->state;
}



const std::string& chunked_reader::get_filename()
{
    return this->filename;
}

char * chunked_reader::get_buffer()
{
    return this->buffer;
}


//@todo remove and use typeid only
compression_type chunked_reader::get_filetype()
{
    if(this->typeid_state() == typeid(ContextUncompressed))
    {
        return compression_type::uncompressed;
    }
    else if(this->typeid_state() == typeid(ContextZstdSeekable))
    {
        return compression_type::zstd;
    }
    else
    {
        return compression_type::undefined;
    }
}




size_t chunked_reader::cache_buffer()
{
    size_t s = this->state->cache_buffer();
    this->buffer_n = s;

    this->buffer_i = 0;
    this->file_i += s;
    
    return s;
}

size_t chunked_reader::read(unsigned char *arg_buffer, size_t arg_buffer_size)
{
    //arg_buffer_size = std::min(arg_buffer_size, (size_t) READ_BUFFER_SIZE);
#if DEBUG
    
    if(arg_buffer == nullptr)
    {
        throw std::runtime_error("[chunked_reader::read] Invalid / not allocated buffer.\n");
    }

    if(arg_buffer_size > READ_BUFFER_SIZE)
    {
        throw std::runtime_error("[chunked_reader::read] Requested buffer size larger than internal context buffer.\n");
    }
    
#endif //DEBUG
    
    return this->state->read(arg_buffer, arg_buffer_size, this->buffer_i, this->buffer_n);

}


void chunked_reader::TransitionTo(State *arg_state) {

    if(this->state != nullptr)
    {
        delete this->state; // delete and destruct previous state, incl file points, should also run fh.close(); etc.
    }

    this->state = arg_state;
    this->state->set_context(this);
}

void chunked_reader::fopen(off_t file_offset)
{
    this->state->fopen(file_offset); // open file handle
    this->cache_buffer(); // read into buffer
}


void chunked_reader::seek(off_t arg_offset)
{
    this->file_i = arg_offset; // @todo obtain return value from this->state->seek() and limit this
    this->state->seek(arg_offset);// set file pointer
    this->cache_buffer();// update internal buffer
}


// positio in the (decompressed) file
size_t chunked_reader::tell()
{
    //printf("Context :: tell: %i - %i + %i = %i\n",
    //this->file_i , 
    //this->buffer_n , 
    //this->buffer_i ,
     //this->file_i - this->buffer_n + this->buffer_i);
     
    return this->file_i - this->buffer_n + this->buffer_i;
}

size_t chunked_reader::get_file_i()
{
    return this->file_i;
}

const std::type_info& chunked_reader::typeid_state()
{
    return typeid(*this->state); // somehow pointer is needed to return ContextSubvariant rather than State
}




State *chunked_reader::find_state()
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
    if(this->fh == nullptr)
    {
        throw std::runtime_error("[ContextUncompressed::fopen] empty fh?\n");
    }

    if(this->fh->is_open()) // @todo move to top-level fopen()
    {
        this->seek(start_pos);
    }
    else
    {
        throw std::runtime_error("[chunked_reader_old::init] Cannot open file for reading.\n");
    }
}

size_t ContextUncompressed::cache_buffer()
{
#if DEBUG
    if(this->fh->tellg() == -1)
    {
        throw std::runtime_error("ContextUncompressed::cache_buffer\n");
    }
    
    if(this->context->get_buffer() == nullptr)
    {
        throw std::runtime_error("ContextUncompressed::cache_buffer - no valid buffer?\n");
    }
#endif //DEBUG

    this->fh->read(this->context->get_buffer(), READ_BUFFER_SIZE);

    size_t s = (size_t) this->fh->gcount();
    /*printf("context uncompressed cache_buffer: %i\n", (int) s);
    printf("%02hhX %02hhX %02hhX %02hhX %02hhX %02hhX %02hhX %02hhX\n",
    this->context->get_buffer()[0],
    this->context->get_buffer()[1],
    this->context->get_buffer()[2],
    this->context->get_buffer()[3],
    this->context->get_buffer()[4],
    this->context->get_buffer()[5],
    this->context->get_buffer()[6],
    this->context->get_buffer()[7]
    );*/

    if(this->fh->eof()) {
        this->fh->clear();
        this->fh->seekg(0, std::ios::end);
    }
    
    return s;
}



void ContextUncompressed::seek(off_t arg_offset)
{
    if(!this->fh->is_open())
    {
        throw std::runtime_error("[ContextUncompressed::seek] unexpected closed filehandle found.\n");
    }

    this->fh->seekg(arg_offset, std::ios::beg);
}



ContextUncompressed::~ContextUncompressed()
{
    if(this->fh != nullptr)
    {
        this->fh->close();
        if(!this->fh)
        {
            std::cerr << "[ContextUncompressed::~ContextUncompressed] unexpected closed filehandle found.\n";
        }

        delete this->fh;
    }
}




size_t ContextZstdSeekable::cache_buffer()
{
    //size_t written = ZSTD_seekable_decompressFile_orDie(this->fh_zstd, this->file_i,  this->buffer, this->file_i + READ_BUFFER_SIZE);
    //this->fh->read(this->context->get_buffer(), READ_BUFFER_SIZE);
    
    // figure out the location in the decompressed file
    
    size_t written = ZSTD_seekable_decompressFile_orDie(
            this->fh, 
            this->context->get_file_i(), //this->context->file_i,
            this->context->get_buffer(),
            this->context->tell() + READ_BUFFER_SIZE //this->context->file_i + READ_BUFFER_SIZE
        );


    //printf("written = %i\n", written);
    //printf("{{%s}}\n",  this->context->get_buffer());
    
    /*
    {
    #if DEBUG
        if(this->fh->tellg() == -1)
        {
            throw std::runtime_error("ContextUncompressed::cache_buffer\n");
        }
    #endif //DEBUG

        this->fh->read(this->context->get_buffer(), READ_BUFFER_SIZE);

        size_t s = (size_t) this->fh->gcount();

        if(this->fh->eof()) {
            this->fh->clear();
            this->fh->seekg(0, std::ios::end);
        }

        return s;
    }
    */

    //throw std::runtime_error("[ContextZstdSeekable::cache_buffer] not implemented.\n");
    
    return written;
}

void ContextZstdSeekable::fopen(off_t start_pos)
{
    if(this->fh != nullptr)
    {
        throw std::runtime_error("[ContextZstdSeekable::fopen] opening a non closed reader.\n");
    }
    
    
    this->fh = ZSTD_seekable_decompressFile_init(this->context->get_filename().c_str());


    if((this->fh->fin == NULL) | feof(this->fh->fin))
    {
        throw std::runtime_error("[ContextZstdSeekable::fopen] not implemented.\n");
    }
    else
    {
        fseek_orDie(this->fh->fin, start_pos, SEEK_SET);// set initial file handle to 0?
        // this->fh->seekg(start_pos, std::ios::beg);

        size_t const initResult = ZSTD_seekable_initFile(this->seekable, fh->fin);
        if (ZSTD_isError(initResult)) { fprintf(stderr, "ZSTD_seekable_init() error : %s \n", ZSTD_getErrorName(initResult)); exit(11); }

        //@todo class member?
        this->maxFileSize = ZSTD_seekable_getFileDecompressedSize(this->seekable);
    }
}

void ContextZstdSeekable::seek(off_t arg_offset)
{
    fseek_orDie(fh->fin, arg_offset, SEEK_SET);
}

ContextZstdSeekable::~ContextZstdSeekable()
{
    if(this->fh != nullptr) {
        //ZSTD_seekable_free(this->fh_zstd->seekable);
        fclose_orDie(this->fh->fin);

        //delete this->fh_zstd->seekable;
        //delete this->fh_zstd->fin;

        delete this->fh;
    }
    
    //throw std::runtime_error("[ContextUncompressed::~ContextUncompressed] not implemented.\n");
}
