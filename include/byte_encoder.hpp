
#ifndef BYTE_ENCODER_HPP
#define BYTE_ENCODER_HPP

#include "config.hpp"
#include "twobit_byte.hpp"
#include "fourbit_byte.hpp"
#include "fivebit_fivebytes.hpp"

#include <memory>
#include <cstring>
#include <array>

inline constexpr auto make_twobit_encode_table()
{
    std::array<unsigned char, 256> t = {};
    t['A'] = t['a'] = 2;
    t['C'] = t['c'] = 1;
    t['G'] = t['g'] = 3;
    t['T'] = t['t'] = 0;
    t['U'] = t['u'] = 0;
    t['N'] = t['n'] = 0;
    return t;
}

inline constexpr auto TWOBIT_ENCODE = make_twobit_encode_table();



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
    unsigned char bytes_per_chunk() const override
    {
        return 1;
    }
    unsigned char nucleotides_per_chunk() const override
    {
        return 4;
    }
    void encode_chunk(const char *input, unsigned char *output) const override
    {
        output[0] = (unsigned char)(
                        (TWOBIT_ENCODE[(unsigned char)input[0]] << 6) |
                        (TWOBIT_ENCODE[(unsigned char)input[1]] << 4) |
                        (TWOBIT_ENCODE[(unsigned char)input[2]] << 2) |
                        (TWOBIT_ENCODE[(unsigned char)input[3]])
                    );
    }
};



class encoder_interface_twobit_rna : public encoder_interface
{
public:
    unsigned char bytes_per_chunk() const override
    {
        return 1;
    }
    unsigned char nucleotides_per_chunk() const override
    {
        return 4;
    }
    void encode_chunk(const char *input, unsigned char *output) const override
    {
        output[0] = (unsigned char)(
                        (TWOBIT_ENCODE[(unsigned char)input[0]] << 6) |
                        (TWOBIT_ENCODE[(unsigned char)input[1]] << 4) |
                        (TWOBIT_ENCODE[(unsigned char)input[2]] << 2) |
                        (TWOBIT_ENCODE[(unsigned char)input[3]])
                    );
    }
};



class encoder_interface_fourbit : public encoder_interface
{
public:
    unsigned char bytes_per_chunk() const override
    {
        return 1;
    }
    unsigned char nucleotides_per_chunk() const override
    {
        return 2;
    }
    void encode_chunk(const char *input, unsigned char *output) const override
    {
        fourbit_byte f;
        f.set((char*)input);
        output[0] = f.data;
    }
};



class encoder_interface_fivebit : public encoder_interface
{
public:
    unsigned char bytes_per_chunk() const override
    {
        return 5;
    }
    unsigned char nucleotides_per_chunk() const override
    {
        return 8;
    }
    void encode_chunk(const char *input, unsigned char *output) const override
    {
        fivebit_fivebytes f;
        f.set((char*)input);
        memcpy(output, f.data_compressed, 5);
    }
};



#endif

