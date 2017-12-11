#include <iostream>
#include <fstream>

#include "config.hpp"

#include "twobit_byte.hpp"
#include "twobit_header.hpp"


unsigned int fourbytes_to_uint(char *chars, unsigned char offset) {
    //for(unsigned int j = 0; j < 4 ; j++)
    //{
  //      printf("[%i] ", chars[offset + j]);
//    }
    
    unsigned int u = ((unsigned char) chars[0 + offset] << 24) | ((unsigned char) chars[1 + offset] << 16) | ((unsigned char)  chars[2 + offset] << 8) | ((unsigned char) chars[3 + offset]);
    
    //printf(" -> %i | %u\n",(unsigned int) u,(unsigned int) u);
    
    return u;
}


void twobit_seq_header::view(unsigned int padding, std::ifstream* fh) {
#if DEBUG
    if(this->n_starts.size() != this->n_ends.size()) {
        throw std::invalid_argument("unequal number of start and end positions for N regions\n");
    }
#endif //DEBUG

    printf(">");
    std::cout << this->name << "\n";
    
    printf(" settings offset to: %u\n", (unsigned int) this->data_position);

    char *byte_tmp = new char [4];
    unsigned int chunk_offset;
    const char *chunk;
    
    bool in_N = false;
    twobit_byte t = twobit_byte();
    unsigned int i_n_start = 0;//@todo make iterator
    unsigned int i_n_end = 0;//@todo make iterator
    unsigned int i_in_seq = 0;
    unsigned int i;
    unsigned int modulo = padding - 1;
    fh->seekg ((unsigned int) this->data_position + 4 + 4 + 4 + (this->n_starts.size() * 8), fh->beg);
    for(i = 0; i < this->n; i++) {


        if(this->n_starts.size() > i_n_start and i == this->n_starts[i_n_start]) {
            in_N = true;
        }

        if(in_N) {
            printf("N");

            if(i == this->n_ends[i_n_end]) {
                i_n_end++;
                in_N = false;
            }
        } else {
            // load new twobit chunk when needed
            chunk_offset = i_in_seq % 4;
            if(chunk_offset == 0) {
                    
                fh->read(byte_tmp, 1);
                //printf("[%u]", byte_tmp[0]);
                t.data = byte_tmp[0];//this->data[i_in_seq / 4];
                chunk = t.get();
            }
            printf("%c", chunk[chunk_offset]);

            //printf(".");
            i_in_seq++;
        }

        if(i % padding == modulo) {
            std::cout << "\n";
        }
    }
    if(i % padding != 0) {
        std::cout << "\n";
    }
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
    if(this->filename == nullptr) {
        throw std::invalid_argument("No filename found");
    }

    std::ifstream file (this->filename->c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        for(unsigned int i = 0; i < this->data.size(); i++) {
            this->data[i]->view(4, &file);
        }
        file.close();
    }
}
