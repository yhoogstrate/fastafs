

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


size_t byte_decoder::decode(char *output, size_t output_size) const
{
    const unsigned char chunk_size = strategy_->bytes_per_chunk();
    const unsigned char nuc_per_chunk = strategy_->nucleotides_per_chunk();
    size_t written = 0;
    for(size_t i = 0; i + chunk_size <= buffer_input_size; i += chunk_size) {
        if(written + nuc_per_chunk > output_size) break;
        strategy_->decode_chunk(buffer_input_data + i, output + written);
        written += nuc_per_chunk;
    }
    return written;
}


std::string byte_decoder::decode() const
{
    const size_t chunk_size = strategy_->bytes_per_chunk();
    const size_t nuc_per_chunk = strategy_->nucleotides_per_chunk();
    const size_t n_chunks = buffer_input_size / chunk_size;
    std::string result(n_chunks * nuc_per_chunk, '\0');
    size_t n = decode(&result[0], result.size());
    result.resize(n);
    return result;
}


