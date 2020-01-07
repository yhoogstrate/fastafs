
#ifndef TWOBIT_BYTE_HPP
#define TWOBIT_BYTE_HPP

#include <array>
#include "config.hpp"

class twobit_byte
{
public:
    static const char encode_hash[256][5];
    static const char n_fill_unmasked = 'N';
    static const char n_fill_masked = 'n';

    static const char bits_per_nucleotide = 2;
    static const char nucleotides_per_byte = 8 / bits_per_nucleotide ;

    unsigned char data;
    void set(unsigned char, unsigned char);
    void set(char*);// string met 4 bytes set
    const char *get(void);
    char *get(unsigned char);

    static unsigned char iterator_to_offset(unsigned int);
};

#endif
