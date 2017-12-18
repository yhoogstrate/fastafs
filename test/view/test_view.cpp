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
 0     1234     5      6789...22    23
 *                     0123...16


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
    fs.view_fasta_chunk(4, buffer, 100, 0);
    
    std::string std_buffer = std::string(buffer);
    
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\n"), 0);



    
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
