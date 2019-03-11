#define BOOST_TEST_MODULE ucsc2bit_to_fastafs

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "utils.hpp"
#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"
//#include "ucsc2bit_to_fastafs.hpp"

#include <vector>


struct twobit_seq_info {
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
    std::vector<twobit_seq_info *> data;
    unsigned int i, n, file_offset;
    unsigned char c;
    twobit_seq_info *s;
    file_offset = 0;
    
    std::ifstream fh_twobit (ucsc2bit_file.c_str(), std::ios::in | std::ios::binary);
    if(fh_twobit.is_open()) {
        fh_twobit.read(buffer, 16); file_offset += 16;
        
        n = fourbytes_to_uint_ucsc2bit(buffer, 8);
        printf("[%d]\n",n);
        
        for(i = 0 ; i < n; i ++) {
            s = new twobit_seq_info();
            fh_twobit.read(buffer, 1); file_offset += 1;
            s->name_size = buffer[0];
            
            fh_twobit.read(buffer, s->name_size); file_offset += s->name_size;
            s->name = new char[s->name_size + 1];
            strncpy(s->name, buffer, s->name_size);
            s->name[s->name_size] = '\0';
            printf("name size: [%c / %i / %d]\n", s->name_size, s->name_size, s->name_size);
            printf("name: [%s]\n", s->name);

            fh_twobit.read(buffer, 4); file_offset += 4;
            s->offset = fourbytes_to_uint_ucsc2bit(buffer, 0);
            
            
            data.push_back(s);
        }
    }
    
    // @todo DELETE ALL IN s in vector, and their names
}




BOOST_AUTO_TEST_SUITE_END()
