#include <iostream>
#include <fstream>

#include "config.hpp"

#include "twobit_header.hpp"


void twobit_header::load(std::string filename) {
    std::cout << "parsing file: " << filename << std::endl;
    
    // open and read first four bytes
    
    
    std::streampos size;
    char * memblock;

    std::ifstream file (filename, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        std::cout << size << " bytes\n";
        if(size < 16) {
            file.close();
            throw std::invalid_argument("Corrupt file: " + filename);
        }
        else {
            // magic
            // version
            // 
            memblock = new char [17];
            file.seekg (0, std::ios::beg);
            file.read (memblock, 16);
            memblock[16] = '\0';
            printf("[%s]\n", memblock);
            
            file.close();

            std::cout << "the entire file content is in memory";

            delete[] memblock;
        }
    }
    
    else std::cout << "Unable to open file";
}

