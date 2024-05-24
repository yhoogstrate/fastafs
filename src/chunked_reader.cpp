
#include "chunked_reader.hpp"



void State::set_context(chunked_reader *arg_context)
{
    this->context = arg_context;
}

// This does not read the actual flat file, this copies its internal buffer to arg_buffer_to
size_t State::read(unsigned char *arg_buffer_to, size_t arg_buffer_to_size,
                   size_t &buffer_i, size_t &buffer_n)
{
#if DEBUG
    if(arg_buffer_to_size > READ_BUFFER_SIZE) {
        throw std::runtime_error("[ContextUncompressed::read] Requested buffer size larger than internal context buffer.\n");
    }
#endif //DEBUG

    size_t written = 0;
    const size_t n1 = std::min(buffer_n - buffer_i, arg_buffer_to_size);// number of characters to copy

    // copy current internal buffer completely
    while(written < n1) {
        arg_buffer_to[written++] = this->context->get_buffer()[buffer_i++];
    }

    if(written < arg_buffer_to_size) {
        this->context->cache_buffer();// needs to set n to 0

        while(buffer_i < buffer_n and written < arg_buffer_to_size) {
            arg_buffer_to[written++] = this->context->get_buffer()[buffer_i++];
        }
    }


    return written;
}




/**
 * Inits the chunked_reader class, by:
 *  - storing the file name
 *  - find the compression format and adapt class stucture to it (state pattern)
 */
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
//@todo switch?
compression_type chunked_reader::get_filetype()
{
    if(this->typeid_state() == typeid(ContextUncompressed)) {
        return compression_type::uncompressed;
    } else if(this->typeid_state() == typeid(ContextZstdSeekable)) {
        return compression_type::zstd;
    } else {
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

    if(arg_buffer == nullptr) {
        throw std::runtime_error("[chunked_reader::read] Invalid / not allocated buffer.\n");
    }

    if(arg_buffer_size > READ_BUFFER_SIZE) {
        throw std::runtime_error("[chunked_reader::read] Requested buffer size larger than internal context buffer.\n");
    }

#endif //DEBUG

    return this->state->read(arg_buffer, arg_buffer_size, this->buffer_i, this->buffer_n);

}


void chunked_reader::TransitionTo(State *arg_state)
{

    if(this->state != nullptr) {
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
    if(is_zstd_file(this->filename.c_str())) {
        return new ContextZstdSeekable;
    } else {
        return new ContextUncompressed;
    }
}


void ContextUncompressed::fopen(off_t start_pos = 0)
{
    if(this->fh != nullptr) {
        throw std::runtime_error("[ContextUncompressed::fopen] opening a non closed reader.\n");
    }

    this->fh = new std::ifstream;
    this->fh->open(this->context->get_filename().c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(this->fh == nullptr) {
        throw std::runtime_error("[ContextUncompressed::fopen] empty fh?\n");
    }

    if(this->fh->is_open()) { // @todo move to top-level fopen()
        this->seek(start_pos);
    } else {
        throw std::runtime_error("[chunked_reader_old::init] Cannot open file for reading.\n");
    }
}

size_t ContextUncompressed::cache_buffer()
{
#if DEBUG
    if(this->fh->tellg() == -1) {
        throw std::runtime_error("ContextUncompressed::cache_buffer\n");
    }

    if(this->context->get_buffer() == nullptr) {
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
    if(!this->fh->is_open()) {
        throw std::runtime_error("[ContextUncompressed::seek] unexpected closed filehandle found.\n");
    }

    this->fh->seekg(arg_offset, std::ios::beg);
}



ContextUncompressed::~ContextUncompressed()
{
    if(this->fh != nullptr) {
        this->fh->close();
        if(!this->fh) {
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
    if(this->fh != nullptr) {
        throw std::runtime_error("[ContextZstdSeekable::fopen] opening a non closed reader.\n");
    }


    this->fh = ZSTD_seekable_decompressFile_init(this->context->get_filename().c_str());


    if((this->fh->fin == NULL) | feof(this->fh->fin)) {
        throw std::runtime_error("[ContextZstdSeekable::fopen] not implemented.\n");
    } else {
        fseek_orDie(this->fh->fin, start_pos, SEEK_SET);// set initial file handle to 0?
        // this->fh->seekg(start_pos, std::ios::beg);

        size_t const initResult = ZSTD_seekable_initFile(this->seekable, fh->fin);
        if(ZSTD_isError(initResult)) {
            fprintf(stderr, "ZSTD_seekable_init() error : %s \n", ZSTD_getErrorName(initResult));
            exit(11);
        }

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
        //delete this->fh_zstd->seekable;
        //delete this->fh_zstd->fin;

        fclose_orDie(this->fh->fin);
        if(this->buffOut != nullptr)
        {
            free(this->buffOut);
        }
        delete this->fh;

    }


    if(this->seekable != nullptr) {
        ZSTD_seekable_free(this->seekable);
        //delete this->ZSTD_seekable;
    }


    //char* const buffOut = (char*) malloc_orDie(buffOutSize);
    //ZSTD_seekable* const seekable = ZSTD_seekable_create(); //@todo -> in constructor, check if not NULL



    //throw std::runtime_error("[ContextUncompressed::~ContextUncompressed] not implemented.\n");
}
