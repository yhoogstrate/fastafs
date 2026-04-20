
#ifndef FOURBIT_BYTE_HPP
#define FOURBIT_BYTE_HPP

#include <array>
#include "config.hpp"

#include "chunked_reader.hpp"


class fourbit_byte
{
public:
    static const char n_fill_unmasked = '-';
    static const char n_fill_masked = '-';
    static const unsigned char bits_per_nucleotide = 4;

    unsigned char data;

    static char encode_hash[256][3];

    static const char nucleotides_per_byte = 8 / fourbit_byte::bits_per_nucleotide;
    static const char nucleotides_per_chunk = 8 / fourbit_byte::bits_per_nucleotide;

    void set(unsigned char, unsigned char);
    void set(char*);
    char *get(void);
    size_t get(unsigned char length, char *output);

    static unsigned char iterator_to_offset(unsigned int);

    static off_t nucleotides_to_compressed_fileoffset(size_t);

    void next(chunked_reader &);
};

#endif
