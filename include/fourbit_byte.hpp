
#ifndef fourbit_BYTE_HPP
#define fourbit_BYTE_HPP

#include <array>
#include "config.hpp"

class fourbit_byte
{
public:
    static const char fourbit_alhpabet[17];
    static const char fourbit_hash[256][3];

    unsigned char data;
    void set(unsigned char, unsigned char);
    void set(char*);// string met 4 bytes set
    const char *get(void);
    char *get(unsigned char);

    static unsigned char iterator_to_offset(unsigned int);
};

#endif
