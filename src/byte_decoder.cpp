

// https://refactoring.guru/design-patterns/strategy/cpp/example


class byte_decoder {
    private size_t raw_buffer_size; // set to 0
    private unsigned char raw_buffer_data[1024]; // bytes of raw decoded data
    
    private size_t decoded_buffer_size; // set to 0
    private unsigned char decoded_buffer_data[1024 * 4]; // bytes of decoded data
    
    
    virtual ~byte_decoder() = default;
    
    
}
