
#ifndef FIVEBIT_FIVEBYTES_HPP
#define FIVEBIT_FIVEBYTES_HPP

#include <array>
#include "config.hpp"

class fivebit_byte
{
public:
    static const char fivebit_alhpabet[28];
    //static const char encode_hash[256][3];
    //static const char n_fill_unmasked = '-';
    //static const char n_fill_masked = '-';
    unsigned char buffer[5];
    unsigned char data[9];

    static const char bits_per_nucleotide = 4;
    static const char nucleotides_per_byte = 8 / bits_per_nucleotide ;

    void set(unsigned char, unsigned char);
    void set(char*);// string met 4 bytes set
    const char *get(void);
    char *get(unsigned char);

    static unsigned char iterator_to_offset(unsigned int);
};

#endif
