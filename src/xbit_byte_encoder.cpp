#include <iostream>
#include <fstream>

#include "config.hpp"

#include "xbit_byte_encoder.hpp"



void xbit_byte_encoder::next(chunked_reader &r)
{
    this->data = r.read();
}

