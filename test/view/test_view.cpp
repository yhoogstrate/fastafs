#define BOOST_TEST_MODULE fastfs_cache

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"



BOOST_AUTO_TEST_SUITE(Testing)

/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(test_chunked_viewing)
{
    std::string fastafs_file = "tmp/test.fastafs";
    
    //fasta_to_fastafs f = fasta_to_fastafs("test", "test/view/test.fa");
    //f.write(fastafs_file);
    
    
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);
    
    char *buffer = new char[100];
    for(unsigned int i = 0 ; i < 100 ; i++) {
        buffer[i] = 123;
    }
    
    //unsigned int read =
    fs.view_fasta_chunk(4, buffer, 100, 0);
    
    
    
    
    
    
    
    for(unsigned int i = 0 ; i < 100 ; i++) {
        printf("[%i] ",buffer[i]);
        
        if((i+1) % 10 == 0) {
            std::cout << "\n";
        }
    }
    printf("\n");

    delete buffer;
}


BOOST_AUTO_TEST_SUITE_END()
