
#ifndef XBIT_BYTE_HPP
#define XBIT_BYTE_HPP

#include <array>
#include "config.hpp"

#include "chunked_reader.hpp"



class xbit_byte_encoder
{
private:
    
public:
    // these members need to be overwritten by parental classes
    static const char n_fill_unmasked;
    static const char n_fill_masked;
    
    static const unsigned char bits_per_nucleotide;

    unsigned char data; // go private

    xbit_byte_encoder() {};

/*
    char (&encode_hash)[256][5];
    twobit_byte(char (&encode_hash_arg)[256][5]): encode_hash(encode_hash_arg) {};

    unsigned char data; // go private
    void set(unsigned char, unsigned char);
    void set(char*);// string met 4 bytes set
    char *get(void);
    char *get(unsigned char);

    static unsigned char iterator_to_offset(unsigned int);

    static const off_t nucleotides_to_compressed_fileoffset(size_t); // file offset waarna gelezen kan worden
    static const off_t nucleotides_to_compressed_offset(size_t);// aantal bytes nodig om zoveel data weg te schrijven
    */

    void next(chunked_reader &);  // update the compressed data
};






#endif
