#include <iostream>
#include <fstream>

#include "config.hpp"

#include "twobit_header.hpp"



void twobit_seq_header::view(unsigned int padding, std::string* filename) {
    unsigned int modulo = padding - 1;
    printf(">");
    std::cout << this->name << "\n";
    unsigned int i;
    for(i = 0; i < this->n; i++) {
        printf(".");
        if(i % padding == modulo) {
            std::cout << "\n";
        }
    }
    if(i % padding != 0) {
        std::cout << "\n";
    }
}


unsigned int fourbytes_to_uint(char *chars, unsigned char offset) {
    return (chars[0 + offset] << 24) | (chars[1 + offset] << 16) | (chars[2 + offset] << 8) | chars[3 + offset];
}

void twobit_header::load(std::string *filename) {

    std::streampos size;
    char * memblock;

    std::ifstream file (*filename, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        std::cout << "parsing file: " << *filename << std::endl;
        this->filename = filename;
        
        size = file.tellg();
        //std::cout << size << " bytes\n";
        if(size < 16) {
            file.close();
            throw std::invalid_argument("Corrupt file: " + *filename);
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

            
            // check magic
            for(i = 0 ;i < 4;  i++) {
                if(memblock[i] != twobit_magic[i]) {
                    throw std::invalid_argument("Corrupt file: " + *filename);
                }
            }
            for(i = 0+4 ;i < 0+4+4;  i++) {
                if(memblock[i] !='\0' or memblock[i+8] != '\0') {
                    throw std::invalid_argument("Corrupt file: " + *filename);
                }
            }
            
            unsigned int n_seq = fourbytes_to_uint(memblock, 8);
            
            
            unsigned char j;
            twobit_seq_header *s;
            for(i = 0; i < n_seq; i ++ ) {
                s = new twobit_seq_header;
                file.read (memblock, 1);

                char name[memblock[0] + 1];
                
                file.read (name, memblock[0]);
                name[(char) memblock[0]] = '\0';
                s->name = std::string(name);
                //=printf("  [%s]\n",name);
                

                file.read(memblock, 4);

                s->data_position = fourbytes_to_uint(memblock, 0);
                //printf("[%u]seq start data/offset=%i\n", (unsigned int) i , s->data_position);
                
                this->data.push_back(s);

            }



            for(i = 0; i < n_seq; i ++ ) {
                s = this->data[i];
                //file.seekg ( s->data_position);

                //s->n
                file.read(memblock, 4);
                s->n = fourbytes_to_uint(memblock, 0);

                // N_regions
                file.read(memblock, 4);
                unsigned int nblocks = fourbytes_to_uint(memblock, 0);
                s->N_regions = nblocks;


                for(j = 0 ; nblocks > j  ; j ++) {
                    file.read(memblock, 4);
                    s->n_starts.push_back( fourbytes_to_uint(memblock, 0));
                }
                for(j = 0 ; j < nblocks ; j ++) {
                    file.read(memblock, 4);
                    s->n_ends.push_back(fourbytes_to_uint(memblock, 0));
                }

                file.read(memblock, 4);
                //unsigned int maskblock = fourbytes_to_uint(memblock, 0);

                // @ todo write subfunc
                unsigned int n_twobit_datapoints = s->n;// - s->N_regions + 3) / 4;
                for(j = 0 ; j < s->N_regions; j++) {
                    n_twobit_datapoints -= s->n_ends[j] - s->n_starts[j] + 1;
                }
                s->n_actg = (n_twobit_datapoints + 3) / 4;


                for(j = 0; j < s->n_actg; j++) {
                    // @todo seekg and add n_twobit_datapoints as offset
                    file.read (memblock, 1);
                }
            }

   
            file.close();

            //std::cout << "the entire file contpaddingent is in memory";

            delete[] memblock;
        }
    }
    
    else std::cout << "Unable to open file";
}



void twobit_header::view(unsigned int padding) {
    for(unsigned int i = 0; i < this->data.size(); i++) {
        this->data[i]->view(4, this->filename);// filename shouldn't be one single stream to allow parallel processing
    }
}
