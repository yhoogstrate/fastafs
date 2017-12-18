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


 * @test just padding and buf size = 100, no offsets
 */
BOOST_AUTO_TEST_CASE(test_chunked_viewing)
{
    unsigned int written;
    
    std::string fastafs_file = "tmp/test.fastafs";
    
    fasta_to_fastafs f = fasta_to_fastafs("test", "test/cache/test.fa");
    f.cache();
    f.write(fastafs_file);
    
    
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);
    
    char *buffer = new char[100];// buffer needs to be c buffer because of the fuse layer
    std::string std_buffer;
    
    
    // test 1: padding: 4
    written = fs.view_fasta_chunk(4, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer);
    
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\n"), 0);


    // test 2: padding: 999 - longer than longest seq
    written = fs.view_fasta_chunk(999, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer);
    
    //>chr1 TTTTCCCCAAAAGGGG >chr2 ACTGACTGNNNNACTG >chr3.1 ACTGACTGAAAAC >chr3.2 ACTGACTGAAAACC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTTCCCCAAAAGGGG\n>chr2\nACTGACTGNNNNACTG\n>chr3.1\nACTGACTGAAAAC\n>chr3.2\nACTGACTGAAAACC\n>chr3.3\nA"), 0);


    // test 3: padding: 5 - longer than longest seq
    written = fs.view_fasta_chunk(5, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer);
    
    //>chr1 TTTTC CCCAA AAGGG G >chr2 ACTGA CTGNN NNACT G >chr3.1 ACTGA CTGAA AAC >chr3.2 ACTGA CTGAA AACC >chr3.3 ACTGA CTGAA AACCC >chr4 ACTGN NNN >chr5 NNACT G 
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTTC\nCCCAA\nAAGGG\nG\n>chr2\nACTGA\nCTGNN\nNNACT\nG\n>chr3.1\nACTGA\nCTGAA\nAAC\n>chr3.2\nACTGA\nCTGAA\nAACC"), 0);



    
    delete buffer;
}


BOOST_AUTO_TEST_SUITE_END()
