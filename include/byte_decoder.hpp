
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
    virtual std::string doAlgorithm(const unsigned char *data) const = 0;
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
    std::string decode() const;
};



class byte_decoder_interface_twobit_dna : public byte_decoder_interface
{
public:
    unsigned char bytes_per_chunk() const override { return 1; }
    std::string doAlgorithm(const unsigned char *data) const override
    {
        return std::string(ENCODE_HASH_TWOBIT_DNA[data[0]], 4);
    }
};



class byte_decoder_interface_twobit_rna : public byte_decoder_interface
{
public:
    unsigned char bytes_per_chunk() const override { return 1; }
    std::string doAlgorithm(const unsigned char *data) const override
    {
        return std::string(ENCODE_HASH_TWOBIT_RNA[data[0]], 4);
    }
};



class byte_decoder_interface_fourbit : public byte_decoder_interface
{
public:
    unsigned char bytes_per_chunk() const override { return 1; }
    std::string doAlgorithm(const unsigned char *data) const override
    {
        return std::string(fourbit_byte::encode_hash[data[0]], 2);
    }
};



class byte_decoder_interface_fivebit : public byte_decoder_interface
{
public:
    unsigned char bytes_per_chunk() const override { return 5; }
    std::string doAlgorithm(const unsigned char *data) const override
    {
        fivebit_fivebytes chunk;
        unsigned char compressed[5] = {data[0], data[1], data[2], data[3], data[4]};
        chunk.set_compressed(compressed);
        return std::string(reinterpret_cast<const char *>(chunk.data_decompressed), 8);
    }
};



#endif


