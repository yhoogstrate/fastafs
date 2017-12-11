#include <iostream>
#include <fstream>

#include "config.hpp"

#include "twobit_byte.hpp"
#include "fastafs.hpp"


unsigned int fourbytes_to_uint(char *chars, unsigned char offset)
{

    unsigned int u = ((unsigned char) chars[0 + offset] << 24) | ((unsigned char) chars[1 + offset] << 16) | ((unsigned char)  chars[2 + offset] << 8) | ((unsigned char) chars[3 + offset]);

    return u;
}


fastafs_seq::fastafs_seq(): n(0) {
}

void fastafs_seq::view(unsigned int padding, std::ifstream* fh)
{
#if DEBUG
    if(this->n_starts.size() != this->n_ends.size()) {
        throw std::invalid_argument("unequal number of start and end positions for N regions\n");
    }
#endif //DEBUG

    printf(">");
    std::cout << this->name << "\n";

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

    //@todo create func this->get_offset_2bit_data();
    fh->seekg ((unsigned int) this->data_position + 4 + 4 + 4 + (this->n_starts.size() * 8), fh->beg);
    for(i = 0; i < this->n; i++) {


        if(this->n_starts.size() > i_n_start and i == this->n_starts[i_n_start]) {
            in_N = true;
        }

        if(in_N) {
            std::cout << "N";

            if(i == this->n_ends[i_n_end]) {
                i_n_end++;
                in_N = false;
            }
        } else {
            // load new twobit chunk when needed
            chunk_offset = i_in_seq % 4;
            if(chunk_offset == 0) {

                fh->read(byte_tmp, 1);
                t.data = byte_tmp[0];
                chunk = t.get();
            }
            std::cout << chunk[chunk_offset];

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


void fastafs::load(std::string *filename)
{

    std::streampos size;
    char * memblock;

    std::ifstream file (*filename, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open()) {
        this->filename = filename;

        size = file.tellg();
        if(size < 16) {
            file.close();
            throw std::invalid_argument("Corrupt file: " + *filename);
        } else {
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
            for(i = 0 ; i < 4;  i++) {
                if(memblock[i] != twobit_magic[i]) {
                    throw std::invalid_argument("Corrupt file: " + *filename);
                }
            }
            for(i = 0+4 ; i < 0+4+4;  i++) {
                if(memblock[i] !='\0' or memblock[i+8] != '\0') {
                    throw std::invalid_argument("Corrupt file: " + *filename);
                }
            }

            unsigned int n_seq = fourbytes_to_uint(memblock, 8);


            unsigned char j;
            fastafs_seq *s;
            for(i = 0; i < n_seq; i ++ ) {
                s = new fastafs_seq;
                file.read (memblock, 1);

                char name[memblock[0] + 1];

                file.read (name, memblock[0]);
                name[(unsigned char) memblock[0]] = '\0';
                s->name = std::string(name);



                file.read(memblock, 4);
                s->data_position = fourbytes_to_uint(memblock, 0);

                this->data.push_back(s);

            }



            for(i = 0; i < n_seq; i ++ ) {
                s = this->data[i];

                file.seekg ((unsigned int) s->data_position, file.beg);

                //s->n
                file.read(memblock, 4);
                s->n = fourbytes_to_uint(memblock, 0);

                file.read(memblock, 4);
                unsigned int N_regions = fourbytes_to_uint(memblock, 0);


                for(j = 0 ; N_regions > j  ; j ++) {
                    file.read(memblock, 4);
                    s->n_starts.push_back( fourbytes_to_uint(memblock, 0));
                }
                for(j = 0 ; j < N_regions ; j ++) {
                    file.read(memblock, 4);
                    s->n_ends.push_back(fourbytes_to_uint(memblock, 0));
                }

                //file.read(memblock, 4);
                //unsigned int maskblock = fourbytes_to_uint(memblock, 0);

            }


            file.close();


            delete[] memblock;
        }
    }

    else std::cout << "Unable to open file";
}



void fastafs::view(unsigned int padding)
{
    if(this->filename == nullptr) {
        throw std::invalid_argument("No filename found");
    }

    std::ifstream file (this->filename->c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open()) {
        for(unsigned int i = 0; i < this->data.size(); i++) {
            this->data[i]->view(padding, &file);
        }
        file.close();
    }
}
