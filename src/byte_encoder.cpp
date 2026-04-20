
#include <string>

#include "config.hpp"
#include "byte_encoder.hpp"



byte_encoder::byte_encoder(std::unique_ptr<encoder_interface> &&strategy) : strategy_(std::move(strategy))
{
}


void byte_encoder::set_strategy(std::unique_ptr<encoder_interface> &&strategy)
{
    strategy_ = std::move(strategy);
}


void byte_encoder::set_output_buffer(unsigned char *output_data, size_t output_data_size)
{
    buffer_output_data = output_data;
    buffer_output_size = output_data_size;
}


size_t byte_encoder::encode(const char *input, size_t input_size) const
{
    const unsigned char nuc_per_chunk = strategy_->nucleotides_per_chunk();
    const unsigned char bytes_per_chunk = strategy_->bytes_per_chunk();
    size_t written = 0;

#if DEBUG
    if(input_size % nuc_per_chunk != 0) {
        throw std::invalid_argument("byte_encoder::encode() -> input_size must be multiple of " + std::to_string(nuc_per_chunk) + ", got " + std::to_string(input_size) + "\n");
    }
#endif

    for(size_t i = 0; i + nuc_per_chunk <= input_size; i += nuc_per_chunk) {
        if(written + bytes_per_chunk > buffer_output_size) break;
        strategy_->encode_chunk(input + i, buffer_output_data + written);
        written += bytes_per_chunk;
    }
    return written;
}


void encoder_interface_twobit_dna::encode_chunk(const char *input, unsigned char *output) const
{
    // Convert 4 nucleotides to 1 byte using twobit encoding
    // T=0, C=1, A=2, G=3 (2 bits each)
    twobit_byte_dna t;
    t.set((char*)input);
    output[0] = t.data;
}


void encoder_interface_twobit_rna::encode_chunk(const char *input, unsigned char *output) const
{
    // Convert 4 nucleotides to 1 byte using twobit encoding (RNA variant)
    // U=0, C=1, A=2, G=3 (2 bits each)
    twobit_byte_rna t;
    t.set((char*)input);
    output[0] = t.data;
}


void encoder_interface_fourbit::encode_chunk(const char *input, unsigned char *output) const
{
    // Convert 2 nucleotides to 1 byte using fourbit encoding
    // IUPAC alphabet, 4 bits per symbol
    fourbit_byte f;
    f.set((char*)input);
    output[0] = f.data;
}


void encoder_interface_fivebit::encode_chunk(const char *input, unsigned char *output) const
{
    // Convert 8 symbols to 5 bytes using fivebit encoding
    // Protein alphabet, 5 bits per symbol
    fivebit_fivebytes f;
    f.set((char*)input);
    memcpy(output, f.data_compressed, 5);
}


