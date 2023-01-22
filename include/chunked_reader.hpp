

#ifndef CHUNKED_READER
#define CHUNKED_READER


#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>


#include "config.hpp"

#include "utils.hpp"

#include "zstd_seekable_utils.hpp"


enum compression_type : signed char { // dit is State
    undefined = -1,
    uncompressed = 0,
    zstd = 1
};


//@todo implement w/ state design pattern
//url: https://refactoring.guru/design-patterns/state


class chunked_reader;

class State
{
protected:
    chunked_reader *context; // back-reference to context, to access file_i, filename etc.

public:
    virtual ~State() {};
    void set_context(chunked_reader *);
    size_t read(unsigned char *, size_t, size_t &, size_t &); // reads from buffer, context a-specific

    // virtual functions:
    virtual void fopen(off_t) = 0;
    virtual size_t cache_buffer() = 0; // formerly update_..._buffer
    virtual void seek(off_t) = 0;
}; // compression type



class ContextUncompressed : public State
{
private:
    std::ifstream *fh = nullptr;

public:
    void fopen(off_t) override;
    size_t cache_buffer() override;
    void seek(off_t);

    ~ContextUncompressed() override;
};

class ContextZstdSeekable : public State
{
private:
    ZSTD_seekable_decompress_init_data* fh = nullptr;

    size_t const buffOutSize = ZSTD_DStreamOutSize();
    char* const buffOut = (char*) malloc_orDie(buffOutSize);
    ZSTD_seekable* const seekable = ZSTD_seekable_create(); //@todo -> in constructor, check if not NULL

    size_t maxFileSize;

public:
    void fopen(off_t) override;
    size_t cache_buffer() override;
    void seek(off_t);

    ~ContextZstdSeekable() override;
};


class chunked_reader // master chunked_reader
{
protected:
    std::string filename;

    char buffer[READ_BUFFER_SIZE + 1];

    size_t buffer_i;
    size_t buffer_n;

    off_t file_i;

    State *state;

public:
    void TransitionTo(State *); // @todo rename to set_compression_type
    chunked_reader(const char *) ;
    ~chunked_reader();

    State* find_state();
    const std::type_info& typeid_state();

    const std::string& get_filename();
    char* get_buffer();

    compression_type get_filetype();

    void fopen(off_t);
    size_t cache_buffer();
    size_t read(unsigned char *, size_t);
    void seek(off_t);
    size_t tell();
    size_t get_file_i();
};



#endif

