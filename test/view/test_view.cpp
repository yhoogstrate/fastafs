#define BOOST_TEST_MODULE fastfs_cache

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"



BOOST_AUTO_TEST_SUITE(Testing)

/**
 * @brief
 * @test
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
    
    std_buffer = std::string(buffer, 100);
    
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\n"), 0);


    // test 2: padding: 999 - longer than longest seq
    written = fs.view_fasta_chunk(999, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer, 100);
    
    //>chr1 TTTTCCCCAAAAGGGG >chr2 ACTGACTGNNNNACTG >chr3.1 ACTGACTGAAAAC >chr3.2 ACTGACTGAAAACC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTTCCCCAAAAGGGG\n>chr2\nACTGACTGNNNNACTG\n>chr3.1\nACTGACTGAAAAC\n>chr3.2\nACTGACTGAAAACC\n>chr3.3\nA"), 0);


    // test 3: padding: 5 - see if 2bit works
    written = fs.view_fasta_chunk(5, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer, 100);
    
    //>chr1 TTTTC CCCAA AAGGG G >chr2 ACTGA CTGNN NNACT G >chr3.1 ACTGA CTGAA AAC >chr3.2 ACTGA CTGAA AACC >chr3.3 ACTGA CTGAA AACCC >chr4 ACTGN NNN >chr5 NNACT G 
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTTC\nCCCAA\nAAGGG\nG\n>chr2\nACTGA\nCTGNN\nNNACT\nG\n>chr3.1\nACTGA\nCTGAA\nAAC\n>chr3.2\nACTGA\nCTGAA\nAACC"), 0);


    // test 4: padding: 1
    written = fs.view_fasta_chunk(1, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer, 100);
    
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G 
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\n"), 0);


    // test 5: padding: 1, offset 1
    written = fs.view_fasta_chunk(1, buffer, 100, 1);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer, 100);
    
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G 
    //X----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("chr1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA"), 0);


    // test 6: padding: 1, offset 2
    written = fs.view_fasta_chunk(1, buffer, 100, 2);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer, 100);
    
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G 
    //XX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("hr1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA\n"), 0);


    // test 7: padding: 1, offset 3
    written = fs.view_fasta_chunk(1, buffer, 100, 3);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer, 100);
    
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G 
    //XXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("r1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA\nA"), 0);


    // test 8: padding: 1, offset 4
    written = fs.view_fasta_chunk(1, buffer, 100, 4);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer, 100);
    
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G 
    //XXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA\nA\n"), 0);


    // test 9: padding: 1, offset 5
    written = fs.view_fasta_chunk(1, buffer, 100, 5);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer, 100);
    
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G 
    //XXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA\nA\nA"), 0);


    // test
    written = fs.view_fasta_chunk(4, buffer, 100, 6);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer, 100);
    
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //XXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("TTTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>ch"), 0);



    // test 11: padding: 1, offset 7
    written = fs.view_fasta_chunk(4, buffer, 100, 7);
    BOOST_CHECK_EQUAL(written, 100);
    
    std_buffer = std::string(buffer, 100);

    std::cout << "[" << std_buffer << "]" << std::endl;
    
    
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //XXXXXXX---.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("TTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>ch"), 0);


    
    delete buffer;
}


BOOST_AUTO_TEST_SUITE_END()
