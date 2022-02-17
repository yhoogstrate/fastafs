
#ifndef FOURBIT_BYTE_HPP
#define FOURBIT_BYTE_HPP

#include <array>
#include "config.hpp"

#include "chunked_reader.hpp"


class fourbit_byte
{
public:
    static const char fourbit_alhpabet[17];
    static char encode_hash[256][3];
    static const char n_fill_unmasked = '-';
    static const char n_fill_masked = '-';

    static const unsigned char bits_per_nucleotide = 4;
    static const char nucleotides_per_byte = 8 / fourbit_byte::bits_per_nucleotide ; // this is about compressed data
    static const char nucleotides_per_chunk = 8 / fourbit_byte::bits_per_nucleotide ; // this is about decompressed chunks

    unsigned char data;
    void set(unsigned char, unsigned char);
    void set(char*);// string met 4 bytes set
    char *get(void);
    char *get(unsigned char);

    static unsigned char iterator_to_offset(unsigned int);

    static const off_t nucleotides_to_compressed_fileoffset(size_t); // file offset waarna gelezen kan worden
    static const off_t nucleotides_to_compressed_offset(size_t);// aantal bytes nodig om zoveel data weg te schrijven

    //@todo chunked reader should be in a function above this.
    //next(char *) should be implemented with decompressed content only
    void next(chunked_reader &); // update the compressed data and set buffer to decompressed data
};

#endif
