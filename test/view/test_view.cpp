#define BOOST_TEST_MODULE fastfs_cache

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"



BOOST_AUTO_TEST_SUITE(Testing)

/**
 * @brief

seq1:


[>] + [chr1] + [\n] + [ACTG]{16} + [\n] = 1+4+1+16+1 = 23
 0     1234     5      678... 22    23


 * @test
 */
BOOST_AUTO_TEST_CASE(test_chunked_viewing)
{
    std::string fastafs_file = "tmp/test.fastafs";
    
    fasta_to_fastafs f = fasta_to_fastafs("test", "test/cache/test.fa");
    f.cache();
    f.write(fastafs_file);
    
    
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);
    
    char *buffer = new char[100];
    for(unsigned int i = 0 ; i < 100 ; i++) {
        buffer[i] = '?';
    }
    
    //unsigned int read =
    fs.view_fasta_chunk(100, buffer, 100, 1);
    
    
    
    
    /*
    
    
    for(unsigned int i = 0 ; i < 100 ; i++) {
        if(buffer[i] == 123) {
            buffer[i] = '?';
        }
        printf("[%c] ",buffer[i]);
        
        if((i+1) % 10 == 0) {
            std::cout << "\n";
        }
    }
    */
    printf("%s\n", buffer);

    delete buffer;
}


BOOST_AUTO_TEST_SUITE_END()
