
#ifndef BYTE_ENCODER_HPP
#define BYTE_ENCODER_HPP

#include "config.hpp"
#include "twobit_byte.hpp"
#include "fourbit_byte.hpp"
#include "fivebit_fivebytes.hpp"

#include <memory>
#include <cstring>



class encoder_interface
{
public:
    virtual ~encoder_interface() = default;
    virtual unsigned char bytes_per_chunk() const = 0;
    virtual unsigned char nucleotides_per_chunk() const = 0;
    virtual void encode_chunk(const char *input, unsigned char *output) const = 0;
};



class byte_encoder
{
private:
    unsigned char *buffer_output_data = nullptr;
    size_t buffer_output_size = 0;

    std::unique_ptr<encoder_interface> strategy_;

public:
    explicit byte_encoder(std::unique_ptr<encoder_interface> &&strategy);
    void set_strategy(std::unique_ptr<encoder_interface> &&strategy);
    void set_output_buffer(unsigned char *output_data, size_t output_data_size);

    size_t encode(const char *input, size_t input_size) const;
};



class encoder_interface_twobit_dna : public encoder_interface
{
public:
    unsigned char bytes_per_chunk() const override { return 1; }
    unsigned char nucleotides_per_chunk() const override { return 4; }
    void encode_chunk(const char *input, unsigned char *output) const override;
};



class encoder_interface_twobit_rna : public encoder_interface
{
public:
    unsigned char bytes_per_chunk() const override { return 1; }
    unsigned char nucleotides_per_chunk() const override { return 4; }
    void encode_chunk(const char *input, unsigned char *output) const override;
};



class encoder_interface_fourbit : public encoder_interface
{
public:
    unsigned char bytes_per_chunk() const override { return 1; }
    unsigned char nucleotides_per_chunk() const override { return 2; }
    void encode_chunk(const char *input, unsigned char *output) const override;
};



class encoder_interface_fivebit : public encoder_interface
{
public:
    unsigned char bytes_per_chunk() const override { return 5; }
    unsigned char nucleotides_per_chunk() const override { return 8; }
    void encode_chunk(const char *input, unsigned char *output) const override;
};



#endif

