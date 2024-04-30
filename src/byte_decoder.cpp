

#include <iostream>
#include <fstream>

#include "config.hpp"

#include "byte_decoder.hpp"




byte_decoder::byte_decoder(std::unique_ptr<byte_decoder_interface> &&strategy = {}) : byte_decoder_interface_(std::move(strategy))
{
};


void byte_decoder::set_strategy(std::unique_ptr<byte_decoder_interface> &&strategy)
{
    byte_decoder_interface_ = std::move(strategy);
};



void byte_decoder::set_input_data(const unsigned char *input_data, size_t input_data_size)
{
    this->buffer_input_size = input_data_size;
    this->buffer_input_data = input_data;
}



void byte_decoder::decode()
{
    printf("decoding ... to be implemented by interface\n");
    printf(this->byte_decoder_interface_->doAlgorithm("ABC").c_str());
    printf("\n");
}







