
#ifndef BYTE_DECODER_HPP
#define BYTE_DECODER_HPP

#include "config.hpp"
#include "twobit_byte.hpp"
#include "fourbit_byte.hpp"
#include "fivebit_fivebytes.hpp"

#include <memory>
#include <string>
#include <cstring>



class byte_decoder_interface
{
public:
    virtual ~byte_decoder_interface() = default;
    virtual unsigned char bytes_per_chunk() const = 0;
    virtual unsigned char nucleotides_per_chunk() const = 0;
    virtual void decode_chunk(const unsigned char *input, char *output) const = 0;
};



class byte_decoder
{
private:
    const unsigned char *buffer_input_data = nullptr;
    size_t buffer_input_size = 0;

    std::unique_ptr<byte_decoder_interface> strategy_;

public:
    explicit byte_decoder(std::unique_ptr<byte_decoder_interface> &&strategy);
    void set_strategy(std::unique_ptr<byte_decoder_interface> &&strategy);
    void set_input_data(const unsigned char *input_data, size_t input_data_size);

    size_t decode(char *output, size_t output_size) const;
    std::string decode() const;
};



class byte_decoder_interface_twobit_dna : public byte_decoder_interface
{
public:
    unsigned char bytes_per_chunk() const override { return 1; }
    unsigned char nucleotides_per_chunk() const override { return 4; }
    void decode_chunk(const unsigned char *input, char *output) const override
    {
        memcpy(output, ENCODE_HASH_TWOBIT_DNA[input[0]], 4);
    }
};



class byte_decoder_interface_twobit_rna : public byte_decoder_interface
{
public:
    unsigned char bytes_per_chunk() const override { return 1; }
    unsigned char nucleotides_per_chunk() const override { return 4; }
    void decode_chunk(const unsigned char *input, char *output) const override
    {
        memcpy(output, ENCODE_HASH_TWOBIT_RNA[input[0]], 4);
    }
};



class byte_decoder_interface_fourbit : public byte_decoder_interface
{
public:
    unsigned char bytes_per_chunk() const override { return 1; }
    unsigned char nucleotides_per_chunk() const override { return 2; }
    void decode_chunk(const unsigned char *input, char *output) const override
    {
        memcpy(output, fourbit_byte::encode_hash[input[0]], 2);
    }
};



class byte_decoder_interface_fivebit : public byte_decoder_interface
{
public:
    unsigned char bytes_per_chunk() const override { return 5; }
    unsigned char nucleotides_per_chunk() const override { return 8; }
    void decode_chunk(const unsigned char *input, char *output) const override;
};



#endif


