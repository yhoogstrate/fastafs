

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



class chunked_reader // dit is Context
{
private:
    std::ifstream *fh_flat;
    ZSTD_seekable_decompress_init_data* fh_zstd;

    std::string filename; // try doing this with inode
    
    compression_type filetype;
    
    char buffer[READ_BUFFER_SIZE + 1];
    size_t buffer_i;
    size_t buffer_n;

    off_t file_i;


public:
    chunked_reader(char *); // filename
    chunked_reader(const char *); // filename
    ~chunked_reader();

    void init(); // generic tasks needed for init

    void update_flat_buffer();
    void update_zstd_buffer();


    void find_filetype();

    void set_filetype(compression_type);
    compression_type get_filetype() { return this->filetype ; };

    size_t read(char *, size_t);// @deprecate
    size_t read(unsigned char *, size_t);
    unsigned char read();

    void seek(off_t);
    size_t tell();
    //size_t size();
};




class Context;

class State
{
protected:
    Context *context; // back-reference to context, to access file_i etc.

public:
    virtual ~State() {};
    void set_context(Context *);


    // virtual functions:
    virtual void fopen(off_t) = 0;
    virtual void update_buffer() = 0;
    
}; // comrpession type



class ContextUncompressed : public State
{
private:
    std::ifstream *fh_flat;// = nullptr;

public:
    void fopen(off_t) override;
    void update_buffer() override;
};

class ContextZstdSeekable : public State
{
private:
    ZSTD_seekable_decompress_init_data* fh_zstd;

public:
    void fopen(off_t) override;
    void update_buffer() override;
};


class Context // master chunked_reader
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
    Context(const char *) ;
    
    State* find_state();
    void fopen(off_t);
    
    const std::string& get_filename();
};




#endif

