
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
int fastafs_seq::view_fasta_chunk(unsigned int padding, char* buffer, off_t start_pos_in_fasta, size_t len_to_copy, std::ifstream* fh) {
    unsigned int i;
    unsigned int written = 0;
    
    // then close line
    if( start_pos_in_fasta == 0 and written < len_to_copy) {
        buffer[written++] = '>';
    }
    

    for(i = start_pos_in_fasta + written - 1; i < this->name.size() and written < len_to_copy; i++) {
        buffer[written++] = this->name[i];
    }
    
    if(start_pos_in_fasta < this->name.size() + 2 and written < len_to_copy) {
        buffer[written++] = '\n';
    }
    

    
    //printf("starting at char (%u + %u - %u): %u after header\n",    start_pos_in_fasta , written, (1 + this->name.size() + 1)  , start_pos_in_fasta + written -  (1 + this->name.size() + 1));


    bool in_N = false;
    char *byte_tmp = new char [4];
    const char *chunk;
    twobit_byte t = twobit_byte();
    unsigned int chunk_offset;
    unsigned int i_in_seq = 0;
    unsigned int i_n_end = 0;
    unsigned int i_n_start = 0;
    unsigned int num_full_padding_blocks = 0;
    unsigned int num_paddings = (this->n + padding - 1) / padding;

/*
>chr1
ttttccccaaaagggg
>chr2
ACTGACTGnnnnACTG
>chr3.1
ACTGACTGaaaac
>chr3.2
ACTGACTGaaaacc
>chr3.3
ACTGACTGaaaaccc
>chr4
ACTGnnnn
>chr5
nnACTG

 *   ***    ****    ****    ****     = 16 ACTGN + 4 N
{T} [TTT \n CCCC \n AAAA \n GGGG \n]
 |   ||| |  |||| |  ||||  | |||| |
1    234 5  6789 10 11 14 15  19 20

 * */

    // 1. zoek nucleotide om te beginnen a.d.h.v. start_pos + copy len (minus geschreven header lengte)
    unsigned int start_pos_after_header = (written + start_pos_in_fasta) - (this->name.size() + 2); // how many'th char after ">header\n"
    unsigned int end_pos_after_header = start_pos_after_header + len_to_copy - written;
    printf("\toffsets in file [ACTG N \\n]: (%u, %u)\n", start_pos_after_header, end_pos_after_header);
    
    
    unsigned int removal_pre = fastafs_seq::n_padding(0, start_pos_after_header, padding);
    unsigned int removal_post = fastafs_seq::n_padding(start_pos_after_header, end_pos_after_header, padding);
    /*for(unsigned int q = 1; q < 25; ++q) {
        printf("\tfastafs_seq::n_padding(1, %u, 4) == %u\n", q, fastafs_seq::n_padding(1, q, 4));
    }*/
    unsigned int start_nucleotide = start_pos_after_header - removal_pre;
    unsigned int end_nucleotide = end_pos_after_header - removal_post;
    printf("\tnucleotides in file [ACTG N]: {-%u, -%u} => (%u, %u)\n",removal_pre, removal_post, start_nucleotide, end_nucleotide);
    
    
    
    // 2. subtract aantal N's & set is_N
    // 3. bepaal 2bit & zet file allocatie goed

    fh->seekg ((unsigned int) this->data_position + 4 + 4 + 4 + (this->n_starts.size() * 8), fh->beg);

    // 4. gaan met die loop
    
    
    
    //unsigned int last_nucleotide = len_to_copy - nucleotide_and_newline_offset;
    //printf("\tFA file    [ACTGN\n] offset: %u -> %u\n",nucleotide_and_newline_offset, last_nucleotide );
    
    //unsigned int nls = nucleotide_and_newline_offset - fastafs_seq::n_padding(0, last_nucleotide, padding); // how many'th nucleotide , is not 0 if there is an offset
    //printf("newlines %u\n",nls);

    //printf("\tnucleotide [ACTGN] offset: %u -> %u\n",start_nucleotide_offset, last_nucleotide_offset );

    
    // @todo function n's until ...
    //unsigned int nucleotide_actg_offset = nucleotide_offset;// number of ACTGS have gone by before
    i_n_start = 0;
    while(this->n_starts.size () > i_n_start ) {
        //printf("%u %u => %u,min(%u,%u)  [len=%u]\n",i_n_start, this->n_starts.size(),this->n_starts[i_n_start],this->n_ends[i_n_start],nucleotide_offset,this->n_ends[i_n_start] - this->n_starts[i_n_start]);
        i_n_start++;
        
        // for sure the opening is in range 
        // only need to check its size and whether we are currently 'in_N'
        
        
    }
    i_n_start = 0;
    
    unsigned int twobit_offset;// number of twobit bytes that have passed
    
    if( (this->name.size() + 2) >  (written + start_pos_in_fasta)) {
        //printf("\n");
        //printf("\tonly header reading: %u <-> %u \n", (written + start_pos_in_fasta) , (this->name.size() + 2));        
    }
    else {
        //printf("\n");
        //printf("\tnucleotide [ACTG]  offset: %u\n",nucleotide_actg_offset);
        //printf("\ttwobit byte offset:        %u\n",twobit_offset);
    }
    
    // if i != 0, set i_in_seq and set fseek appropriately
    
    
    unsigned int i_in_file;i=0;
    for(i_in_file = 0; written < len_to_copy; i_in_file++) {
        
        if((i_in_file % (padding+1) == padding) or (i_in_file == this->n + num_paddings - 1)) {
            buffer[written++] = '\n';
        }
        else {
            if(this->n_starts.size() > i_n_start and i == this->n_starts[i_n_start]) {
                in_N = true;
            }
            if(in_N) {
                buffer[written++] = 'N';

                if(i == this->n_ends[i_n_end]) {
                    i_n_end++;
                    in_N = false;
                }
            } else {
                
                chunk_offset = i_in_seq % 4;
                // load new twobit chunk when needed
                
                if(chunk_offset == 0) {
                    fh->read(byte_tmp, 1);
                    t.data = byte_tmp[0];
                    chunk = t.get();
                }
                //std::cout << chunk[chunk_offset];


                buffer[written++] = chunk[chunk_offset];

                i_in_seq++;
            }
            
            i++;
        }
    }


    
    
    return written;
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


//@brief calculates the number of paddings found in a sequence of length N with 
unsigned int fastafs_seq::n_padding(unsigned int offset, unsigned int position_until,unsigned int padding) {
    // if debug:
    //   if offset > position_until
    //      throw error
    // end if
    
    unsigned int n = (position_until + 1) / (padding + 1);
    
    // minus all the n's that occurred before offset
    if (offset > 0) {
        n -= fastafs_seq::n_padding(0, offset - 1, padding);
    }
    
    return n;
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


int fastafs::view_fasta_chunk(unsigned int padding, char* buffer, size_t buffer_size, off_t file_offset) {
    unsigned int written = 0,tmppp=0;
    unsigned int total_fa_size = 0, i_buffer = 0;
    unsigned int i, seq_true_fasta_size;

    std::ifstream file (this->filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open()) {


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
                
                printf("    read(&buffer[%i], %i, min(%i,%i)= %i)\n",
                    i_buffer,
                    file_offset + i_buffer - total_fa_size,
                    (unsigned int) buffer_size - i_buffer, seq_true_fasta_size - file_offset,
                    std::min((unsigned int) buffer_size - i_buffer, seq_true_fasta_size- ( (unsigned int) file_offset + i_buffer - total_fa_size ))
                    );
                
                tmppp = this->data[i]->view_fasta_chunk(
                    padding,
                    &buffer[i_buffer],
                    file_offset + i_buffer - total_fa_size,
                    std::min((unsigned int) buffer_size - i_buffer, seq_true_fasta_size- ( (unsigned int) file_offset + i_buffer - total_fa_size ) ),
                    &file
                    );
                
                printf("    [written = %u]\n", tmppp);
                
                written += tmppp;
                
                while(file_offset + i_buffer < (total_fa_size + seq_true_fasta_size) and i_buffer < buffer_size) {
                    //printf("%i ", file_offset + i_buffer - total_fa_size);
                    i_buffer++;
                }
            }
            
            
            printf("\n");
            // update for next iteration
            total_fa_size +=  seq_true_fasta_size;
        }
    
    
        file.close();
    }
    else {
        throw std::runtime_error("could not load fastafs: " + this->filename);
    }
    
    return written;
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
