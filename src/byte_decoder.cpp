

#include <string>

#include "config.hpp"
#include "byte_decoder.hpp"



byte_decoder::byte_decoder(std::unique_ptr<byte_decoder_interface> &&strategy) : strategy_(std::move(strategy))
{
}


void byte_decoder::set_strategy(std::unique_ptr<byte_decoder_interface> &&strategy)
{
    strategy_ = std::move(strategy);
}


void byte_decoder::set_input_data(const unsigned char *input_data, size_t input_data_size)
{
    buffer_input_data = input_data;
    buffer_input_size = input_data_size;
}


std::string byte_decoder::decode() const
{
    std::string result;
    const unsigned char chunk_size = strategy_->bytes_per_chunk();
    for(size_t i = 0; i + chunk_size <= buffer_input_size; i += chunk_size) {
        result += strategy_->doAlgorithm(buffer_input_data + i);
    }
    return result;
}


