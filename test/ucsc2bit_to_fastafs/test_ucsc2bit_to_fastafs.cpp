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
    
    // the followin should be member of a conversion struct, because they're not related to the original 2bit format:
    SHA_CTX ctx;
    unsigned char sha1_digest[SHA_DIGEST_LENGTH];
};



static char nt[2] = "T";
static char nc[2] = "C";
static char na[2] = "A";
static char ng[2] = "G";
static char nn[2] = "N";


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_ucsc2bit_to_fasta)
{
    std::string fastafs_file = "tmp/test.regenerated.fastafs";
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
    ucsc2bit_seq_header *s;

    std::ifstream fh_twobit (ucsc2bit_file.c_str(), std::ios::in | std::ios::binary);
    std::ofstream fh_fastafs (fastafs_file.c_str(), std::ios::out | std::ios::binary);
    if(fh_twobit.is_open() and fh_fastafs.is_open()) {
        fh_twobit.read(buffer, 12);

        n = fourbytes_to_uint_ucsc2bit(buffer, 8);
        
        // write fastafs header
        fh_fastafs << UCSC2BIT_MAGIC;
        fh_fastafs << UCSC2BIT_VERSION;
        uint_to_fourbytes(buffer, n);
        fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
        fh_fastafs << "\x00\x00\x00\x00"s;
        
        
        fh_twobit.seekg(16);
        for(i = 0 ; i < n; i ++) {
            s = new ucsc2bit_seq_header();
            fh_twobit.read(buffer, 1);
            s->name_size = buffer[0];
            //header_size += 1 + s->name_size + 20 + 4;
            
            fh_twobit.read(buffer, s->name_size);
            s->name = new char[s->name_size + 1];
            strncpy(s->name, buffer, s->name_size);
            s->name[s->name_size] = '\0';

            fh_twobit.read(buffer, 4);
            s->offset = fourbytes_to_uint_ucsc2bit(buffer, 0);
            
            data.push_back(s);
            
            SHA1_Init(&s->ctx);

            fh_fastafs.write((char *) &s->name_size, (size_t) 1); // name size
            fh_fastafs.write(s->name, (size_t) s->name_size);// name
            fh_fastafs << "ssssssssssSSSSSSSSSS"s;//sha1 placeholder, overwrite later with gseek etc
            fh_fastafs << "oooo"s;//file offset placeholder, can only be rewritten after m and n blocks are filled
        }

        
        for(i = 0 ; i < n; i ++) {
            // use seekg to get file offset?

            // seq-len
            s = data[i];
            fh_twobit.read(buffer, 4);
            s->dna_size = fourbytes_to_uint_ucsc2bit(buffer, 0);
            
            uint_to_fourbytes(buffer, s->dna_size);
            fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);


            // parse N blocks
            fh_twobit.read(buffer, 4);
            s->n_blocks = fourbytes_to_uint_ucsc2bit(buffer, 0);

            uint_to_fourbytes(buffer, s->n_blocks);
            fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);

            for(j = 0; j < s->n_blocks; j++) {
                fh_twobit.read(buffer, 4);
                s->n_block_starts.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));
                
                uint_to_fourbytes(buffer, s->n_block_starts.back());
                fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
            }

            for(j = 0; j < s->n_blocks; j++) {
                fh_twobit.read(buffer, 4);
                s->n_block_sizes.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));

                uint_to_fourbytes(buffer, s->n_block_starts.back() + s->n_block_sizes.back() - 1);
                fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
            }

            // parse M blocks
            fh_twobit.read(buffer, 4);
            s->m_blocks = fourbytes_to_uint_ucsc2bit(buffer, 0);

            uint_to_fourbytes(buffer, s->m_blocks);
            fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);

            for(j = 0; j < s->m_blocks; j++) {
                fh_twobit.read(buffer, 4);
                s->m_block_starts.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));
                
                uint_to_fourbytes(buffer, s->m_block_starts.back());
                fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
            }

            for(j = 0; j < s->m_blocks; j++) {
                fh_twobit.read(buffer, 4);
                s->m_block_sizes.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));
                
                uint_to_fourbytes(buffer, s->m_block_starts.back() + s->m_block_sizes.back() - 1);
                fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
            }

            // parse and convert sequence
            fh_twobit.read(buffer, 4);
            twobit_byte t_in = twobit_byte();
            const char *decoded_in;

            twobit_byte t_out = twobit_byte();

            unsigned int k = 0; // iterator in fastafs format
            unsigned int next_n;
            unsigned int n_n = 0;
            unsigned int n_ahead = 0; // number of n's ahead
            if(s->n_blocks > n_n) {
                next_n = s->n_block_starts[n_n];
            }
            else {
                next_n = s->dna_size + 1; // otherwise -1
            }
            for(j = 0; j < s->dna_size; j++) {
                if(j % 4 == 0) {
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
                    SHA1_Update(&s->ctx, nn, 1);
                    
                    n_ahead -= 1;

                    if(n_ahead == 0) {
                        n_n++;
                        if(s->n_blocks > n_n) {
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
                            SHA1_Update(&s->ctx, nt, 1);

                            break;
                        case 'c':
                        case 'C':
                            t_out.set(twobit_byte::iterator_to_offset(k), 1);
                            SHA1_Update(&s->ctx, nc, 1);
                            
                            break;
                        case 'a':
                        case 'A':
                            t_out.set(twobit_byte::iterator_to_offset(k), 2);
                            SHA1_Update(&s->ctx, na, 1);

                            break;
                        case 'g':
                        case 'G':
                            t_out.set(twobit_byte::iterator_to_offset(k), 3);
                            SHA1_Update(&s->ctx, ng, 1);

                            break;
                    }
                    //t_out.set(twobit_byte::iterator_to_offset(k), decoded_in[j % 4]);
                    if(k % 4 == 3) {
                        fh_fastafs.write((char *) &(t_out.data), (size_t) 1); // name size
                        printf("[%u] ", t_out.data);// binary fake representation of t_out.data
                    }
                    k++;
                }
            }
            
            if(k % 4 != 0) {
                for(j = k % 4; j < 4; j++) {
                    t_out.set(twobit_byte::iterator_to_offset(j), 0);
                }
                fh_fastafs.write((char *) &(t_out.data), (size_t) 1); // name size
                printf("[%u] ", t_out.data);// binary fake representation of t_out.data

                //fh_fastafs << "?";
            }
            
            delete[] s->name;
            delete s;
            
            printf("\n");
        }
    }
}




BOOST_AUTO_TEST_SUITE_END()
