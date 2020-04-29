
#ifndef FIVEBIT_FIVEBYTES_HPP
#define FIVEBIT_FIVEBYTES_HPP

#include <array>
#include "config.hpp"

class fivebit_fivebytes
{
public:
    static const char fivebit_alhpabet[28 + 1];
    static const char encode_hash[28 + 1][2];
    
    static const char n_fill_unmasked = '?';
    static const char n_fill_masked = '?';
    
    unsigned char buffer[5];
    unsigned char buffer_out[8];

    unsigned char data; // single char - not useful

    static const char bits_per_nucleotide = 5;
    static const char nucleotides_per_byte = 1 ; //8 / bits_per_nucleotide ;

    void set(unsigned char, unsigned char);
    void set(char*);// string met 4 bytes set
    const char *get(void);
    char *get(unsigned char);

    static unsigned char iterator_to_offset(unsigned int);
};

#endif
