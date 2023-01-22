#include <iostream>
#include <fstream>

#include "config.hpp"

#include "xbit_byte_encoder.hpp"



void xbit_byte_encoder::next(chunked_reader &r)
{
    unsigned char *buf = new unsigned char[2];
    r.read(buf, 1);
    this->data = buf[0];

    delete[] buf;
}

