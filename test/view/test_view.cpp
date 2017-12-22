#define BOOST_TEST_MODULE fastfs_cache

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"



BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_fastafs_seq_static_func)
{
    /*
        padding=4, offset=0, position_until=0, 1, 2, 3:      0  "A" "AC" "ACT" "ACTG"
        padding=4, offset=0, position_until=4, 5, 6, 7, 8:   1  "ACTG\n" "ACTG\nA" "ACTG\nAA" "ACTG\nAAA" "ACTG\nAAAA"
        padding=4, offset=0, position_until=9, 10,11,12,13:  2  "ACTG\nAAAA\n" "ACTG\nAAAA\nG" "ACTG\nAAAA\nGG" "ACTG\nAAAA\nGGG" "ACTG\nAAAA\nGGGG"
        
        padding=4, offset=1, position_until=*, 1, 2, 3:      0  "" "C" "CT" "CTG"
        padding=4, offset=1, position_until=4, 5, 6, 7, 8:   1  "CTG\n" "CTG\nA" "CTG\nAA" "CTG\nAAA" "CTG\nAAAA"
        padding=4, offset=1, position_until=9, 10,11,12,13:  2  "CTG\nAAAA\n" "CTG\nAAAA\nG" "CTG\nAAAA\nGG" "CTG\nAAAA\nGGG" "CTG\nAAAA\nGGGG"
        padding=4, offset=1, position_until=14,15,16,17,18:  3  ...
        padding=4, offset=1, position_until=19,20,21,22,23:  4  ...
        
        padding=4, offset=2, position_until=*, *, 2, 3:      0  "" "" "T" "TG"
        padding=4, offset=2, position_until=4, 5, 6, 7, 8:   1  "TG\n" "TG\nA" "TG\nAA" "TG\nAAA" "TG\nAAAA"
        padding=4, offset=2, position_until=9, 10,11,12,13:  2  "TG\nAAAA\n" "TG\nAAAA\nG" "TG\nAAAA\nGG" "TG\nAAAA\nGGG" "TG\nAAAA\nGGGG"

        padding=4, offset=3, position_until=*, *, *, 3:      0  "" "" "" "G"
        padding=4, offset=3, position_until=4, 5, 6, 7, 8:   1  "G\n" "G\nA" "G\nAA" "G\nAAA" "G\nAAAA"
        padding=4, offset=3, position_until=9, 10,11,12,13:  2  "G\nAAAA\n" "G\nAAAA\nG" "G\nAAAA\nGG" "G\nAAAA\nGGG" "G\nAAAA\nGGGG"

        padding=4, offset=4, position_until=*, *, *, *:      0  "" "" "" ""
        padding=4, offset=4, position_until=4, 5, 6, 7, 8:   1  "\n" "\nA" "\nAA" "\nAAA" "\nAAAA"
        padding=4, offset=4, position_until=9, 10,11,12,13:  2  "\nAAAA\n" "\nAAAA\nG" "\nAAAA\nGG" "\nAAAA\nGGG" "\nAAAA\nGGGG"

        padding=4, offset=5, position_until=*, *, *, *:      0  "" "" "" ""
        padding=4, offset=5, position_until=*, 5, 6, 7, 8:   0  "" "A" "AA" "AAA" "AAAA"
        padding=4, offset=5, position_until=9, 10,11,12,13:  1  "AAAA\n" "AAAA\nG" "AAAA\nGG" "AAAA\nGGG" "AAAA\nGGGG"
    */
    
    unsigned int padding = 4;
    unsigned int offset, position_until;
    
    offset = 0;
    for(position_until = 0; position_until <= 3; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 0);
    }
    for(position_until = 4; position_until <= 8; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 1);
    }
    for(position_until = 9; position_until <= 13; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 2);
    }
    
    
    offset = 1;
    for(position_until = 1; position_until <= 3; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 0);
    }
    for(position_until = 4; position_until <= 8; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 1);
    }
    for(position_until = 9; position_until <= 13; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 2);
    }
    for(position_until = 14; position_until <= 18; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 3);
    }
    for(position_until = 19; position_until <= 23; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 4);
    }
    
    offset = 2;
    for(position_until = 2; position_until <= 3; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 0);
    }
    for(position_until = 4; position_until <= 8; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 1);
    }
    for(position_until = 9; position_until <= 13; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 2);
    }
    
    offset = 3;
    for(position_until = 3; position_until <= 3; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 0);
    }
    for(position_until = 4; position_until <= 8; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 1);
    }
    for(position_until = 9; position_until <= 13; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 2);
    }
    
    offset = 4;
    for(position_until = 4; position_until <= 8; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 1);
    }
    for(position_until = 9; position_until <= 13; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 2);
    }
    
    offset = 5;
    for(position_until = 5; position_until <= 8; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 0);
    }
    for(position_until = 9; position_until <= 13; ++position_until) {
        BOOST_CHECK_EQUAL(fastafs_seq::n_padding(offset, position_until, padding), 1);
    }
}



BOOST_AUTO_TEST_CASE(test_fastafs_twobit_offset_calc)
{
    // testing "ACTGACTGNNNNACTG"
    
    unsigned int num_Ns; // number of N's until certain nucleotide is reached
    bool in_N;
    
    std::string fastafs_file = "tmp/test.fastafs";
    
    fasta_to_fastafs f = fasta_to_fastafs("test", "test/cache/test.fa");
    f.cache();
    f.write(fastafs_file);
    
    
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);
    
    for(unsigned int i = 0 ; i <= 7; i++) {
        in_N = fs.data[1]->get_n_offset(i, &num_Ns);
        BOOST_CHECK_EQUAL(num_Ns, 0);
        BOOST_CHECK_EQUAL(in_N, false);
    }

    in_N = fs.data[1]->get_n_offset(8, &num_Ns);
    BOOST_CHECK_EQUAL(num_Ns, 1);
    BOOST_CHECK_EQUAL(in_N, true);

    in_N = fs.data[1]->get_n_offset(9, &num_Ns);
    BOOST_CHECK_EQUAL(num_Ns, 2);
    BOOST_CHECK_EQUAL(in_N, true);

    in_N = fs.data[1]->get_n_offset(10, &num_Ns);
    BOOST_CHECK_EQUAL(num_Ns, 3);
    BOOST_CHECK_EQUAL(in_N, true);

    in_N = fs.data[1]->get_n_offset(11, &num_Ns);
    BOOST_CHECK_EQUAL(num_Ns, 4);
    BOOST_CHECK_EQUAL(in_N, true);
    
    for(unsigned int i = 12 ; i <= 15; i++) {
        in_N = fs.data[1]->get_n_offset(i, &num_Ns);
        BOOST_CHECK_EQUAL(num_Ns, 4);
        BOOST_CHECK_EQUAL(in_N, false);
    }

}




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
    
    
    // padding: 4
    written = fs.view_fasta_chunk(4, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\n"), 0);


    // padding: 999 - longer than longest seq
    written = fs.view_fasta_chunk(999, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTTCCCCAAAAGGGG >chr2 ACTGACTGNNNNACTG >chr3.1 ACTGACTGAAAAC >chr3.2 ACTGACTGAAAACC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTTCCCCAAAAGGGG\n>chr2\nACTGACTGNNNNACTG\n>chr3.1\nACTGACTGAAAAC\n>chr3.2\nACTGACTGAAAACC\n>chr3.3\nA"), 0);


    // padding: 5 - see if 2bit works
    written = fs.view_fasta_chunk(5, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTTC CCCAA AAGGG G >chr2 ACTGA CTGNN NNACT G >chr3.1 ACTGA CTGAA AAC >chr3.2 ACTGA CTGAA AACC >chr3.3 ACTGA CTGAA AACCC >chr4 ACTGN NNN >chr5 NNACT G 
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTTC\nCCCAA\nAAGGG\nG\n>chr2\nACTGA\nCTGNN\nNNACT\nG\n>chr3.1\nACTGA\nCTGAA\nAAC\n>chr3.2\nACTGA\nCTGAA\nAACC"), 0);


    // padding: 1
    written = fs.view_fasta_chunk(1, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G 
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\n"), 0);


    // padding: 1, offset 1
    written = fs.view_fasta_chunk(1, buffer, 100, 1);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G 
    //X----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("chr1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA"), 0);


    // padding: 1, offset 2
    written = fs.view_fasta_chunk(1, buffer, 100, 2);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G 
    //XX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("hr1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA\n"), 0);


    // padding: 1, offset 3
    written = fs.view_fasta_chunk(1, buffer, 100, 3);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G 
    //XXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("r1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA\nA"), 0);


    // padding: 1, offset 4
    written = fs.view_fasta_chunk(1, buffer, 100, 4);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G 
    //XXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA\nA\n"), 0);


    // padding: 1, offset 5
    written = fs.view_fasta_chunk(1, buffer, 100, 5);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G 
    //XXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA\nA\nA"), 0);


    // padding: 4, offset: 6
    written = fs.view_fasta_chunk(4, buffer, 100, 6);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //XXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("TTTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>ch"), 0);


    // padding: 4, offset: 7
    written = fs.view_fasta_chunk(4, buffer, 100, 7);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //XXXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("TTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr"), 0);


    // padding: 4, offset: 8
    written = fs.view_fasta_chunk(4, buffer, 100, 8);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //XXXXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("TT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr3"), 0);


    // padding: 4, offset: 9
    written = fs.view_fasta_chunk(4, buffer, 100, 9);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //XXXXXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("T\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr3."), 0);


    // padding: 4, offset: 10
    written = fs.view_fasta_chunk(4, buffer, 100, 10);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //XXXXXXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr3.3"), 0);


    // padding: 4, offset: 11
    written = fs.view_fasta_chunk(4, buffer, 100, 11);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //XXXXXXXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("CCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr3.3\n"), 0);


    // padding: 4, offset: 12
    written = fs.view_fasta_chunk(4, buffer, 100, 12);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //XXXXXXXXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("CCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr3.3\nA"), 0);


    // padding: 4, offset: 13
    written = fs.view_fasta_chunk(4, buffer, 100, 13);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG 
    //XXXXXXXXXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("CC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr3.3\nAC"), 0);




    //std::cout << "[" << std_buffer << "]" << std::endl;
    delete buffer;
}




BOOST_AUTO_TEST_SUITE_END()
