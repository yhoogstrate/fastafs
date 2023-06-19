
#ifndef BYTE_DECODER_HPP
#define BYTE_DECODER_HPP

#include "config.hpp"

#include <array>
#include <iostream>
#include <fstream>



// https://refactoring.guru/design-patterns/strategy/cpp/example


class byte_decoder {
    private:
        size_t buffer_input_size; // set to 0
        //unsigned char buffer_input_data[1024]; // bytes of raw decoded data
        const unsigned char *buffer_input_data;
    
        size_t buffer_decoded_size; // set to 0
        unsigned char buffer_decoded_data[1024]; // bytes of decoded data
        unsigned char *buffer_decoded_data_i;
    
    public:
        void set_input_data(const unsigned char *input_data, size_t input_data_size);
        
        //virtual ~byte_decoder() = default;
    
    
};


#endif

