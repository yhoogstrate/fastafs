
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
    virtual std::string doAlgorithm(const unsigned char* data) const = 0;
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



// four bit byte etc: 
class byte_decoder_interface_A : public byte_decoder_interface
{
public:
    std::string doAlgorithm(const unsigned char* data) const override 
    {
            std::string out = "";
            
            out += " [4b]";

    return out;
    }
};


/*
 * two bit byte etc:
 * "AAAA" => "\x10 \x10 \x10 \x10"
 * "CAAA" => "\x01 \x10 \x10 \x10"
 * "TAAA" => "\x00 \x10 \x10 \x10"
 * "GAAA" => "\x11 \x10 \x10 \x10"
 */
class byte_decoder_interface_B : public byte_decoder_interface
{
public:
    std::string doAlgorithm(const unsigned char* data) const override
    {
        const unsigned char hash_size_input = 4;
        const unsigned char hash_size_output = 1;
        
        std::string out = "";
        
        for(int i = 0; i < 5; i++) {
            out += data[i];
        }
        
        out += " [2b]";

        return out;
    }
};

// five bit byte etc
class byte_decoder_interface_C : public byte_decoder_interface
{
public:
    std::string doAlgorithm(const unsigned char* data) const override
    {
            std::string out = "";
            
            out += " [5b]";

            return out;
    }
};



#endif



