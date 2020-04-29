
#ifndef FOURBIT_BYTE_HPP
#define FOURBIT_BYTE_HPP

#include <array>
#include "config.hpp"

class fourbit_byte
{
public:
    static const char fourbit_alhpabet[17];
    static const char encode_hash[256][3];
    static const char n_fill_unmasked = '-';
    static const char n_fill_masked = '-';

    static const char bits_per_nucleotide = 4;
    static const char nucleotides_per_byte = 8 / bits_per_nucleotide ;

    unsigned char data;
    void set(unsigned char, unsigned char);
    void set(char*);// string met 4 bytes set
    const char *get(void);
    char *get(unsigned char);

    static unsigned char iterator_to_offset(unsigned int);
};

#endif
