#define BOOST_TEST_MODULE ucsc2bit_to_fastafs

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "utils.hpp"
#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"
//#include "ucsc2bit_to_fastafs.hpp"

#include <vector>


struct ucsc2bit_seq_header {
    unsigned char name_size;
    char *name;
    unsigned int offset;// in file, in bytes
    
    unsigned int dna_size;

    unsigned int n_blocks;
    std::vector<unsigned int> n_block_starts;
    std::vector<unsigned int> n_block_sizes;
    
    unsigned int m_blocks;
    std::vector<unsigned int> m_block_starts;
    std::vector<unsigned int> m_block_sizes;
    
    // dna can be deduced realtime during iteration
};


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_ucsc2bit_to_fasta)
{
    std::string fastafs_file = "tmp/test.fastafs";
    std::string ucsc2bit_file = "tmp/test.2bit";

    // 01 fasta_to_fastafs()
    fasta_to_fastafs f = fasta_to_fastafs("test", "test/data/test.fa");
    f.cache();
    f.write(fastafs_file);

    // 02 load fastafs
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    // 03 fastafs::chunked ucsc2bit view of the file
    char buffer[4096 +  1];
    std::fstream ucsc2bit_out_stream(ucsc2bit_file.c_str(), std::ios :: out | std::ios :: binary);
    if(ucsc2bit_out_stream.is_open()) {
        unsigned int written = fs.view_ucsc2bit_chunk(buffer, 4096, 0);
        std::string buffer_s = std_string_nullbyte_safe(buffer, written);
        ucsc2bit_out_stream << buffer_s;
        ucsc2bit_out_stream.close();
    } else {
        throw std::runtime_error("Could not write to file: " + ucsc2bit_file);
    }

    // 04 ucsc2bit_to_fasta
    fastafs fs_new = fastafs("");
    std::vector<ucsc2bit_seq_header *> data;
    unsigned int i, j, n;
    unsigned char c;
    ucsc2bit_seq_header *s;

    std::ifstream fh_twobit (ucsc2bit_file.c_str(), std::ios::in | std::ios::binary);
    if(fh_twobit.is_open()) {
        fh_twobit.read(buffer, 12);

        n = fourbytes_to_uint_ucsc2bit(buffer, 8);
        //printf("[%d]\n",n);
        
        fh_twobit.seekg(16);
        for(i = 0 ; i < n; i ++) {
            s = new ucsc2bit_seq_header();
            fh_twobit.read(buffer, 1);
            s->name_size = buffer[0];
            
            fh_twobit.read(buffer, s->name_size);
            s->name = new char[s->name_size + 1];
            strncpy(s->name, buffer, s->name_size);
            s->name[s->name_size] = '\0';

            fh_twobit.read(buffer, 4);
            s->offset = fourbytes_to_uint_ucsc2bit(buffer, 0);
            
            data.push_back(s);
        }
        
        for(i = 0 ; i < n; i ++) {
            s = data[i];
            printf("name: [%s]\n", s->name);

            fh_twobit.read(buffer, 4);
            s->dna_size = fourbytes_to_uint_ucsc2bit(buffer, 0);
            unsigned int n_fastafs_twobits = s->dna_size;
            printf("2bit bytes: %u\n", n_fastafs_twobits);

            fh_twobit.read(buffer, 4);
            s->n_blocks = fourbytes_to_uint_ucsc2bit(buffer, 0);
            
            printf("n-blocks: [%u]\n", s->n_blocks);
            
            for(j = 0; j < s->n_blocks; j++) {
                fh_twobit.read(buffer, 4);
                s->n_block_starts.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));
            }

            for(j = 0; j < s->n_blocks; j++) {
                fh_twobit.read(buffer, 4);
                s->n_block_sizes.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));
                
                n_fastafs_twobits -= s->n_block_sizes.back();
                printf("2bit bytes: %u\n", n_fastafs_twobits);

            }

            fh_twobit.read(buffer, 4);
            s->m_blocks = fourbytes_to_uint_ucsc2bit(buffer, 0);
            
            printf("m-blocks: [%u]\n", s->m_blocks);
            
            for(j = 0; j < s->m_blocks; j++) {
                fh_twobit.read(buffer, 4);
                s->m_block_starts.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));
            }

            for(j = 0; j < s->m_blocks; j++) {
                fh_twobit.read(buffer, 4);
                s->m_block_sizes.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));
            }
            
            fh_twobit.read(buffer, 4);
            

            twobit_byte t_in = twobit_byte();
            const char *decoded_in;

            twobit_byte t_out = twobit_byte();

            unsigned int k = 0; // iterator in fastafs format
            unsigned int next_n;
            unsigned int n_n = 0;
            unsigned int n_ahead = 0; // number of n's ahead
            if(s->n_blocks > n_n) {
                //printf("n start: %u\n",s->n_block_starts[n_n]);
                next_n = s->n_block_starts[n_n];
                //n_ahead = s->n_block_sizes[n_n];
            }
            else {
                next_n = s->dna_size + 1; // otherwise -1
            }
            for(j = 0; j < s->dna_size; j++) {
                if(j % 4 == 0) {
                    //printf("[setting t_in]\n");
                    fh_twobit.read(buffer, 1);
                    t_in.data = buffer[0];
                    decoded_in = t_in.get();// pointer to the right value?
                }
                
                // check if N
                if(j == next_n) // new next block has opened
                {
                    n_ahead = s->n_block_sizes[n_n];
                }
                if(n_ahead > 0) {// we are in an N block on an N base
                    n_ahead -= 1;

                    if(n_ahead == 0) {
                        n_n++;
                        if(s->n_blocks > n_n) {
                            //printf("n start: %u\n",s->n_block_starts[n_n]);
                            next_n = s->n_block_starts[n_n];
                            n_ahead = 0;
                        }
                    }
                }
                else {
                    // 0 ->
                    //printf("2bit.set(%c , %i -> %i)\n", decoded_in[j % 4], k , twobit_byte::iterator_to_offset(k) );
                    switch(decoded_in[j % 4]) {
                        case 't':
                        case 'T':
                        case 'u':
                        case 'U':
                            t_out.set(twobit_byte::iterator_to_offset(k), 0);
                            break;
                        case 'c':
                        case 'C':
                            t_out.set(twobit_byte::iterator_to_offset(k), 1);
                            break;
                        case 'a':
                        case 'A':
                            t_out.set(twobit_byte::iterator_to_offset(k), 2);
                            break;
                        case 'g':
                        case 'G':
                            t_out.set(twobit_byte::iterator_to_offset(k), 3);
                            break;
                    }
                    //t_out.set(twobit_byte::iterator_to_offset(k), decoded_in[j % 4]);
                    if(k % 4 == 3) {
                        printf("?");// binary fake representation of t_out.data
                    }
                    k++;
                }
            }
            
            //printf("remaining nuceotides in last 2bit: k % 4 = %i\n",k % 4);
            printf("%c", t_out.get(k % 4));
            
            delete[] s->name;
            delete s;
        }
    }
}




BOOST_AUTO_TEST_SUITE_END()
