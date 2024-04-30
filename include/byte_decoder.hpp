
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
// https://refactoring.guru/design-patterns/strategy/cpp/example [looks more like strategy]


class byte_decoder_interface // generic decoder interface/
{
private:
    unsigned int size_block_encoded; //
    unsigned int size_block_decoded; //

    int block_size_1;

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


    std::unique_ptr<byte_decoder_interface> byte_decoder_interface_;

public:
    explicit byte_decoder(std::unique_ptr<byte_decoder_interface> &&strategy);
    void set_strategy(std::unique_ptr<byte_decoder_interface> &&strategy);

    void set_input_data(const unsigned char *input_data, size_t input_data_size);

    void decode();

};



// four bit byte etc
class byte_decoder_interface_A : public byte_decoder_interface
{

public:
    std::string doAlgorithm(std::string data) const override;
};


// two bit byte etc
class byte_decoder_interface_B : public byte_decoder_interface
{
public:

    std::string doAlgorithm(std::string data) const override;
};

// five bit byte etc
class byte_decoder_interface_C : public byte_decoder_interface
{

public:
    std::string doAlgorithm(std::string data) const override;
};



#endif

