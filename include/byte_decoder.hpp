
#ifndef BYTE_DECODER_HPP
#define BYTE_DECODER_HPP

#include "config.hpp"

#include <array>
#include <iostream>
#include <fstream>

#include <algorithm>
#include <functional>
#include <memory>



// https://refactoring.guru/design-patterns/state/cpp/example
// https://refactoring.guru/design-patterns/strategy/cpp/example


class byte_decoder_interface // generic decoder interface/
{
private:
    unsigned int size_block_encoded; //
    unsigned int size_block_decoded; //

public:
    virtual ~byte_decoder_interface() = default;
    virtual std::string doAlgorithm(std::string data) const = 0;
};




class byte_decoder
{
private:
    size_t buffer_input_size; // set to 0
    //unsigned char buffer_input_data[1024]; // bytes of raw decoded data
    const unsigned char *buffer_input_data;

    size_t buffer_decoded_size; // set to 0
    unsigned char buffer_decoded_data[1024]; // bytes of decoded data
    unsigned char *buffer_decoded_data_i;

public:
    void set_input_data(const unsigned char *input_data, size_t input_data_size);

    //virtual ~byte_decoder_interface() = default;
    std::unique_ptr<byte_decoder_interface> byte_decoder_interface_;
};



// four bit byte etc
class Concretebyte_decoder_interfaceA : public byte_decoder_interface
{
public:
    std::string doAlgorithm(std::string data) const override
    {
        std::string out = data + " [a]";

        return out;
    }
};

// two bit byte etc
class Concretebyte_decoder_interfaceB : public byte_decoder_interface
{
    std::string doAlgorithm(std::string data) const override
    {
        std::string out = data + " [b]";

        return out;
    }
};



#endif

