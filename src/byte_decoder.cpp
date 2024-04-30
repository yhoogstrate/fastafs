

#include <iostream>
#include <fstream>

#include "config.hpp"

#include "byte_decoder.hpp"




byte_decoder::byte_decoder(std::unique_ptr<byte_decoder_interface> &&strategy = {}) : byte_decoder_interface_(std::move(strategy))
{
};



void byte_decoder::set_input_data(const unsigned char *input_data, size_t input_data_size)
{
    this->buffer_input_size = input_data_size;
    this->buffer_input_data = input_data;
}



