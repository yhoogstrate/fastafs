
#ifndef FIVEBIT_FIVEBYTES_HPP
#define FIVEBIT_FIVEBYTES_HPP

#include <array>
#include "config.hpp"

#include "chunked_reader.hpp"


class fivebit_fivebytes
{
public:
    static const char fivebit_alphabet[28 + 1];
    static const char encode_hash[28 + 1][2];

    static const char n_fill_unmasked = '?';
    static const char n_fill_masked = '?';

    unsigned char data_compressed[5]; // 5
    unsigned char data_decompressed[8];

    void unpack(); // unpacks this->data into this->data_decompressed

    //unsigned char data; // single char - not useful

    //static const unsigned char bits_per_nucleotide = 5;
    //static const char nucleotides_per_byte = 1 ; //8 / bits_per_nucleotide ;
    static const char bytes_per_chunk = 5 ; // this is about decompressed chunks
    static const char nucleotides_per_chunk = 8 ; // this is about decompressed chunks

    void set(unsigned char, unsigned char);
    void set(char *);// string with 8 amino acids
    void set_compressed(unsigned char (&compressed_data)[5]);// string with 5 character - requires unpacking

    char *get(void);
    char *get(unsigned char);

    static unsigned char iterator_to_offset(unsigned int);
    static unsigned char decompressed_to_compressed_bytes(unsigned char); // when only 5/8 bytes are filled, only 4/5 bytes need to be written

    static off_t nucleotides_to_compressed_fileoffset(size_t); // file offset waarna gelezen kan worden
    static off_t nucleotides_to_compressed_offset(size_t);// aantal bytes nodig om zoveel data weg te schrijven


    void next(chunked_reader &); // update the compressed data and set buffer to decompressed data
};

#endif
