#include <iostream>
#include <fstream>

#include "config.hpp"

#include "twobit_header.hpp"

unsigned int fourbytes_to_uint(char *chars, unsigned char offset) {
    return (chars[0 + offset] << 24) | (chars[1 + offset] << 16) | (chars[2 + offset] << 8) | chars[3 + offset];
}

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
            
            char twobit_magic[5] = TWOBIT_MAGIC;
            
            
            unsigned int i;
            for(i = 0; i < 16 ; i++) {
                printf("%i\t", memblock[i]);
                if(i % 4 == 3) {
                    printf("\n");
                }
            }

            
            // check magic
            for(i = 0 ;i < 4;  i++) {
                if(memblock[i] != twobit_magic[i]) {
                    throw std::invalid_argument("Corrupt file: " + filename);
                }
            }
            for(i = 0+4 ;i < 0+4+4;  i++) {
                if(memblock[i] !='\0' or memblock[i+8] != '\0') {
                    throw std::invalid_argument("Corrupt file: " + filename);
                }
            }
            
            unsigned int n_seq = fourbytes_to_uint(memblock, 8);
            
            
            printf("\nn seqs: %i\n", n_seq);
            unsigned char j;
            twobit_seq_header *s;
            for(i = 0; i < n_seq; i ++ ) {
                s = new twobit_seq_header;
                file.read (memblock, 1);

                char name[memblock[0] + 1];
                
                file.read (name, memblock[0]);
                name[memblock[0]] = '\0';
                s->name = std::string(name);
                printf("  [%s]\n",name);
                

                file.read(memblock, 4);
                for(j = 0; j < 4 ; j++) {
                    printf("%i\t", (unsigned char) memblock[j]);
                    if(j % 4 == 3) {
                        printf("\n");
                    }
                }
                
                s->data_position = fourbytes_to_uint(memblock, 0);
                printf("[%i]seq start data/offset=%i\n", (unsigned int) i , s->data_position);
                
                this->data.push_back(s);

            }

            for(i = 0; i < n_seq; i ++ ) {

/*
 *              read s->n and set it 
                file.read (memblock, 4);
                for(j = 0; j < 4 ; j++) {
                    printf("%i\t", (unsigned char) memblock[j]);
                    if(j % 4 == 3) {
                        printf("\n");
                    }
                }
                
                unsigned int nblocks = fourbytes_to_uint(memblock, 0);
                s->N = nblocks;
                printf("[%i]N-blocks=%i\n", (unsigned int) i , nblocks);
                */
            }

   
            file.close();

            std::cout << "the entire file content is in memory";

            delete[] memblock;
        }
    }
    
    else std::cout << "Unable to open file";
}

