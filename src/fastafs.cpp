
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <openssl/sha.h>

#include "config.hpp"

#include "twobit_byte.hpp"
#include "fastafs.hpp"



fastafs_seq::fastafs_seq(): n(0)
{
}

void fastafs_seq::view_fasta(unsigned int padding, std::ifstream* fh)
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
    
    delete[] byte_tmp;
}



    // unsigned int padding
    // char* = buffer + i_buffer?
    //                                        offset1: starting pos (fasta)
    //                                        offset2: ending pos ? (fasta)
int fastafs_seq::view_fasta_chunk(unsigned int padding, char* buffer, off_t start_pos_in_fasta, size_t len_to_copy) {
    unsigned int i;
    unsigned int written = 0;
    
    // then close line
    if( start_pos_in_fasta == 0 and written < len_to_copy) {
        buffer[written++] = '>';
    }
    
    // first check if sequence name needs to be included
    // start = 0: w=1 set i = 0     s+w-1=0+1-1=0
    // start = 1: w=0 set i = 0     s+w-1=1+0-1=0
    // start = 2: w=0 set i = 1     s+w-1=2+0-1=1
    // start = 3: w=0 set i = 2     s+w-1=
    for(i = start_pos_in_fasta + written - 1; i < this->name.size() and written < len_to_copy; i++) {
        buffer[written++] = this->name[i];
    }
    
    if(start_pos_in_fasta <= this->name.size() + 2 and written < len_to_copy) {
        buffer[written++] = '\n';
    }
    

    
    printf("starting at char (%u + %u - %u): %u after header\n",    start_pos_in_fasta , written, (1 + this->name.size() + 1)  , start_pos_in_fasta + written -  (1 + this->name.size() + 1));

    // find paddings before this number
    // substract this and 

    // if char == padding:
    //     buffer[written++] = "\n";
    
/**
number of newlines within sequence section
padding =  4
for n=0:10:
n=0     0       >chr1 \n
n=1     1       >chr1 \n [A \n ]
n=2     1       >chr1 \n [AC \n ]
n=3     1       >chr1 \n [ACT \n ]
n=4     1       >chr1 \n [ACTA \n ]
n=5     2       >chr1 \n [ACTA \n C \n ]
n=6     2       >chr1 \n [ACTA \n CT \n ]
n=7     2       >chr1 \n [ACTA \n CTG \n ]
n=8     2       >chr1 \n [ACTA \n CTGG \n ]
n=9     3       >chr1 \n [ACTA \n CTGG \n A \n ]

num_paddings = (n + padding - 1) / padding



if not last  (n = 9) -> num_padding = 3:  number of elements = 9 + 3 = 12
    i=0     f       >chr1 \n [A ]
    i=1     1       >chr1 \n [AC ]
    i=2     1       >chr1 \n [ACT ]
    i=3     1       >chr1 \n [ACTA ]
    i=4     2       >chr1 \n [ACTA \n ] 
    i=5     2       >chr1 \n [ACTA \n C ]
    i=6     2       >chr1 \n [ACTA \n CT ]
    i=7     2       >chr1 \n [ACTA \n CTG ]
    i=8     3       >chr1 \n [ACTA \n CTGG ]
    i=9     3       >chr1 \n [ACTA \n CTGG \n ]
    i=10    3       >chr1 \n [ACTA \n CTGG \n A ]
    i=11    3       >chr1 \n [ACTA \n CTGG \n A \n ]


0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 
> c h r 1 \ 1 2 3 4 \  5  6  7  8  \  9  10 11 12 \  13 14 15 16 \



 */
    unsigned int num_paddings = (this->n + padding - 1) / padding;
    unsigned int num_full_padding_blocks = 0;
    printf (" %u < %u \n", written, len_to_copy);
    //printf(" num paddings: %u [%u / %u]    num bytes: %u\n", num_paddings, this->n, padding, this->n + num_paddings);

    //for(i = 0; i < this->n + num_paddings and written < len_to_copy; i++) {
    for(i = 0; written < len_to_copy; i++) {
        
        if((i % (padding+1) == padding) or (i == this->n + num_paddings - 1)) {
            buffer[written++] = '\n';
        }
        else {
            buffer[written++] = 'X';
        }
    }

    //@todo create func this->get_offset_2bit_data();
    /*

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
    
    delete[] byte_tmp;
     */
     
    // then close line (avoid this if the previous one was newline by padding!)
    if(written < len_to_copy) {
        buffer[written++] = '\n';
    }
    
    
    return 0;
}

std::string fastafs_seq::sha1(std::ifstream* fh)
{
    char chunk[4];
    unsigned int i;

    SHA_CTX ctx;
    SHA1_Init(&ctx);

    uint_to_fourbytes(chunk, this->n);
    SHA1_Update(&ctx, chunk, 4);

    for(i = 0; i < this->n_starts.size(); i++) {
        uint_to_fourbytes(chunk, this->n_starts[i]);
        SHA1_Update(&ctx, chunk, 4);

        uint_to_fourbytes(chunk, this->n_ends[i]);
        SHA1_Update(&ctx, chunk, 4);
    }

    fh->seekg ((unsigned int) this->data_position + 4 + 4 + 4 + (this->n_starts.size() * 8), fh->beg);
    for(i = 0; i < this->n_twobits(); i++) {
        fh->read(chunk, 1);
        SHA1_Update(&ctx, chunk, 1);
    }


    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1_Final(hash, &ctx);

    char outputBuffer[41];
    for(i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[40] = 0;
    return std::string(outputBuffer);
}


unsigned int fastafs_seq::n_twobits()
{
    // if n actg bits is:
    // 0 -> 0
    // 1,2,3 and 4 -> 1

    unsigned int n = this->n;

    for(unsigned int i = 0; i < this->n_starts.size(); i++) {
        n -= n_ends[i] - this->n_starts[i] + 1;
    }

    return (n + 3)/ 4;
}



fastafs::fastafs(std::string arg_name) : 
    name(arg_name) {
    
}

fastafs::~fastafs() {
    for(unsigned int i = 0; i < this->data.size();i++)
    {
        delete this->data[i];
    }
}

void fastafs::load(std::string afilename)
{

    std::streampos size;
    char * memblock;

    std::ifstream file (afilename, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open()) {
        this->filename = afilename;

        size = file.tellg();
        if(size < 16) {
            file.close();
            throw std::invalid_argument("Corrupt file: " + filename);
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
                    throw std::invalid_argument("Corrupt file: " + filename);
                }
            }
            for(i = 0+4 ; i < 0+4+4;  i++) {
                if(memblock[i] !='\0' or memblock[i+8] != '\0') {
                    throw std::invalid_argument("Corrupt file: " + filename);
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



void fastafs::view_fasta(unsigned int padding)
{
    if(this->filename.size() == 0) {
        throw std::invalid_argument("No filename found");
    }

    std::ifstream file (this->filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open()) {
        for(unsigned int i = 0; i < this->data.size(); i++) {
            this->data[i]->view_fasta(padding, &file);
        }
        file.close();
    }
}


/**
example (padding = 4, buffer = 12):

>seq1
ACTG
>seq2
NNNN
>seq3
AAAG


buffer_size = 12
file_offset = 0
seq       1   1   1   1   1   1    1   1   1   1   1    2
buffer -> [>] [s] [e] [q] [1] [\n] [A] [C] [T] [G] [\n] [>]

buffer_size = 12
file_offset = 12
seq       2   2   2   2   2    2   2   2   2   2    3   3
buffer -> [s] [e] [q] [2] [\n] [N] [N] [N] [N] [\n] [>] [s]

buffer_size = 12
file_offset = 24
          3   3   3   3    3   3   3   3
buffer -> [e] [q] [3] [\n] [A] [A] [A] [G] ?   ?    ?   ?

 */
int fastafs::view_fasta_chunk(unsigned int padding, char* buffer, size_t buffer_size, off_t file_offset) {
    unsigned int total_fa_size = 0, i_buffer = 0;
    unsigned int i, seq_true_fasta_size;

    
    for(i = 0; i < this->data.size(); i++) {
        seq_true_fasta_size = 1; // '>'
        seq_true_fasta_size += this->data[i]->name.size() + 1;// "chr1\n"
        seq_true_fasta_size += this->data[i]->n; // ACTG NNN
        seq_true_fasta_size += (this->data[i]->n + (padding - 1)) / padding;// number of newlines corresponding to ACTG NNN lines

        printf(">%u 'th sequence is being scanned [estimated len=%u]\n", i+1, seq_true_fasta_size);
        
        
        // determine whether and how much there needs to be read between: total_fa_size <=> total_fa_size + seq_true_fasta_size
        if((file_offset + i_buffer) >= total_fa_size and file_offset < (total_fa_size + seq_true_fasta_size)) {
            
            // file offset = 4
            // i_buffer = 0
            
            // total_fa_size = 0
            // seq_true_fasta_size = 23
            
            
            // we zijn op plek:
            // file_offset + i_buffer
            
            printf("read(&buffer[%i], %i, %i)\n",
                i_buffer,
                file_offset + i_buffer - total_fa_size,
                std::min((unsigned int) buffer_size - i_buffer, seq_true_fasta_size )
                );
            
            this->data[i]->view_fasta_chunk(
                padding,
                &buffer[i_buffer],
                file_offset + i_buffer - total_fa_size,
                std::min((unsigned int) buffer_size - i_buffer, seq_true_fasta_size )
                );
            
            while(file_offset + i_buffer < (total_fa_size + seq_true_fasta_size) and i_buffer < buffer_size) {
                //printf("%i ", i_buffer);
                printf("%i ", file_offset + i_buffer - total_fa_size);
                //if(buffer[i_buffer] == 123) {
                //    buffer[i_buffer] = (unsigned char) (i+1);
                //}
                i_buffer++;
            }

            //printf("\n{%i}\n", total_fa_size + file_offset);
        }
        
        
        printf("\n");
        // update for next iteration
        total_fa_size +=  seq_true_fasta_size;
    }
    
    
    
}


void fastafs::info()
{
    if(this->filename.size() == 0) {
        throw std::invalid_argument("No filename found");
    }

    std::ifstream file (this->filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open()) {
        std::cout << "FASTAFS NAME: " << this->filename << "\n";
        printf("SEQUENCES:    %u\n",(unsigned int) this->data.size());

        for(unsigned int i = 0; i < this->data.size(); i++) {
            //this->data[i]->view(padding, &file);
            printf("    >%-24s%-12i%s\n", this->data[i]->name.c_str(),this->data[i]->n,this->data[i]->sha1(&file).c_str());
        }
        file.close();
    }
}


unsigned int fastafs::n()
{
    unsigned int n = 0;
    for(unsigned i = 0; i < this->data.size(); i++) {
        n += this->data[i]->n;
    }

    return n;
}



void fastafs::mount() {
}



std::string fastafs::basename() {
    std::string basename = "";
}
