

#include <iostream>
#include <fstream>

#include "config.hpp"

#include "byte_decoder.hpp"



void byte_decoder::set_input_data(const unsigned char *input_data, size_t input_data_size) {
    this->buffer_input_size = input_data_size;
    this->buffer_input_data = input_data;
}
