
#ifndef TWOBIT_BYTE_HPP
#define TWOBIT_BYTE_HPP

#include <array>
#include "config.hpp"

#include "chunked_reader.hpp"


class twobit_byte
{
public:
    static const char n_fill_unmasked = 'N';
    static const char n_fill_masked = 'n';
    static const unsigned char bits_per_nucleotide = 2;

    unsigned char data;

    char (&encode_hash)[256][5];
    twobit_byte(char (&encode_hash_arg)[256][5]);

    static const char nucleotides_per_byte = 8 / twobit_byte::bits_per_nucleotide;
    static const char nucleotides_per_chunk = 8 / twobit_byte::bits_per_nucleotide;

    void set(unsigned char, unsigned char);
    void set(char*);
    char *get(void);
    size_t get(unsigned char length, char *output);

    static unsigned char iterator_to_offset(unsigned int);

    static off_t nucleotides_to_compressed_fileoffset(size_t);
    static off_t nucleotides_to_compressed_offset(size_t);

    void next(chunked_reader &);
};




extern char ENCODE_HASH_TWOBIT_DNA[256][5];
extern char ENCODE_HASH_TWOBIT_RNA[256][5];



class twobit_byte_dna : public twobit_byte
{
public:
    twobit_byte_dna();
};



class twobit_byte_rna : public twobit_byte
{
public:
    twobit_byte_rna();
};






#endif
