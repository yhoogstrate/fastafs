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

                s->data_position = fourbytes_to_uint(memblock, 0);
                printf("[%u]seq start data/offset=%i\n", (unsigned int) i , s->data_position);
                
                this->data.push_back(s);

            }



            for(i = 0; i < n_seq; i ++ ) {
                s = this->data[i];
                printf("\n\ngoto: %u\n",  s->data_position);
                
                //file.seekg ( s->data_position);

                // n (seq size)
                file.read(memblock, 4);
                for(j = 0; j < 4 ; j++) {
                    printf("%i\t", memblock[j]);
                }
                printf("\n");
                s->n = fourbytes_to_uint(memblock, 0);
                printf("[%i]n=%i\n", (unsigned int) i , (unsigned int) s->n);



                // N blocks
                file.read(memblock, 4);
                for(j = 0; j < 4 ; j++) {
                    printf("%i\t", memblock[j]);
                }
                printf("\n");
                unsigned int nblocks = fourbytes_to_uint(memblock, 0);
                s->N_regions = nblocks;
                printf("[%i]N-blocks=%u\n\n", (unsigned int) i , nblocks);

                for(j = 0 ; nblocks > j  ; j ++) {
                    file.read(memblock, 4);
                    unsigned int nblock = fourbytes_to_uint(memblock, 0);
                    s->n_starts.push_back(nblock);
                    printf("[%i]N-block start: %i\n", (unsigned int) i , nblock);

                }
                for(j = 0 ; j < nblocks ; j ++) {
                    file.read(memblock, 4);
                    unsigned int nblock = fourbytes_to_uint(memblock, 0);
                    s->n_ends.push_back(nblock);
                    printf("[%i]N-block start: %i\n", (unsigned int) i , nblock);

                }

                file.read(memblock, 4);
                for(j = 0; j < 4 ; j++) {
                    printf("[%i]\t", memblock[j]);
                }
                printf("\n");
                unsigned int maskblock = fourbytes_to_uint(memblock, 0);
                printf("mask: %i\n\n", (unsigned int) i , maskblock);

                
                

                unsigned int n_twobit_datapoints = s->n;// - s->N_regions + 3) / 4;
                printf("[%i]\n", n_twobit_datapoints);
                for(j = 0 ; j < s->N_regions; j++) {
                    printf("  %i - %i = %i \n",s->n_ends[j] , s->n_starts[j],s->n_ends[j] - s->n_starts[j]);
                    n_twobit_datapoints -= s->n_ends[j] - s->n_starts[j] + 1;
                    printf(" - [%i]\n", n_twobit_datapoints);
                }
                n_twobit_datapoints = (n_twobit_datapoints + 3) / 4;
                printf(" / [%i]\n", n_twobit_datapoints);
                for(j = 0; j <  n_twobit_datapoints; j++) {
                    file.read (memblock, 1);
                    printf("[%i]\t", memblock[0]);
                }
                printf("\ndata\n\n");
                


                /*
                file.read(memblock, 4);
                for(j = 0; j < 4 ; j++) {
                    printf("[%i]\t", memblock[j]);
                }
                printf("\n");
                unsigned int nullblock = fourbytes_to_uint(memblock, 0);
                printf("null: %i\n", (unsigned int) i , nullblock);

                */
            }

   
            file.close();

            std::cout << "the entire file content is in memory";

            delete[] memblock;
        }
    }
    
    else std::cout << "Unable to open file";
}

