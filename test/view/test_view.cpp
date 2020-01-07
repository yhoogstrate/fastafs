#define BOOST_TEST_MODULE fastfs_test_view


#include <iostream>
#include <fstream>

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_twobit_fastafs.hpp"
#include "fasta_to_fourbit_fastafs.hpp"
#include "fastafs.hpp"



void flush_buffer(char *buffer, size_t n, char fill)
{
    for(size_t i = 0; i < n; i++) {
        buffer[i] = fill;
    }
}



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
    uint32_t padding = 4;
    uint32_t offset, position_until;

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
    uint32_t num_Ns; // number of N's until certain nucleotide is reached
    bool in_N;

    std::string fastafs_file = "tmp/test.fastafs";

    fasta_to_twobit_fastafs("test/data/test.fa", fastafs_file);
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    for(uint32_t i = 0 ; i <= 7; i++) {
        in_N = fs.data[1]->get_n_offset(i, &num_Ns);
        BOOST_CHECK_EQUAL(num_Ns, 0);
        BOOST_CHECK_EQUAL(in_N, false);
    }

    in_N = fs.data[1]->get_n_offset(8, &num_Ns);
    BOOST_CHECK_EQUAL(num_Ns, 0);// This is the first N, 0 were found before
    BOOST_CHECK_EQUAL(in_N, true);

    in_N = fs.data[1]->get_n_offset(9, &num_Ns);
    BOOST_CHECK_EQUAL(num_Ns, 1);
    BOOST_CHECK_EQUAL(in_N, true);

    in_N = fs.data[1]->get_n_offset(10, &num_Ns);
    BOOST_CHECK_EQUAL(num_Ns, 2);
    BOOST_CHECK_EQUAL(in_N, true);

    in_N = fs.data[1]->get_n_offset(11, &num_Ns);
    BOOST_CHECK_EQUAL(num_Ns, 3);
    BOOST_CHECK_EQUAL(in_N, true);

    for(uint32_t i = 12 ; i <= 15; i++) {
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
    uint32_t written;

    std::string test_name = "test";
    std::string fasta_file = "test/data/" + test_name + ".fa";
    std::string fastafs_file = "tmp/" + test_name + ".fastafs";

    fasta_to_twobit_fastafs(fasta_file, fastafs_file);
    fastafs fs = fastafs(test_name);
    fs.load(fastafs_file);

    char *buffer = new char[100];// buffer needs to be c buffer because of the fuse layer
    std::string std_buffer;

    // init caches
    ffs2f_init* cache_p1 = fs.init_ffs2f(1, false);
    ffs2f_init* cache_p4 = fs.init_ffs2f(4, false);
    ffs2f_init* cache_p5 = fs.init_ffs2f(5, false);
    ffs2f_init* cache_p999 = fs.init_ffs2f(999, false);

    // padding: 4

    written = fs.view_fasta_chunk_cached(cache_p4, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\n"), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 999 - longer than longest seq
    written = fs.view_fasta_chunk_cached(cache_p999, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTTCCCCAAAAGGGG >chr2 ACTGACTGNNNNACTG >chr3.1 ACTGACTGAAAAC >chr3.2 ACTGACTGAAAACC >chr3.3 ACTGACTGAAAACCC >chr4 ACTGNNNN >chr5 NNACTG
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTTCCCCAAAAGGGG\n>chr2\nACTGACTGNNNNACTG\n>chr3.1\nACTGACTGAAAAC\n>chr3.2\nACTGACTGAAAACC\n>chr3.3\nA"), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 5 - see if 2bit works
    written = fs.view_fasta_chunk_cached(cache_p5, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTTC CCCAA AAGGG G >chr2 ACTGA CTGNN NNACT G >chr3.1 ACTGA CTGAA AAC >chr3.2 ACTGA CTGAA AACC >chr3.3 ACTGA CTGAA AACCC >chr4 ACTGN NNN >chr5 NNACT G
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nTTTTC\nCCCAA\nAAGGG\nG\n>chr2\nACTGA\nCTGNN\nNNACT\nG\n>chr3.1\nACTGA\nCTGAA\nAAC\n>chr3.2\nACTGA\nCTGAA\nAACC"), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 1
    written = fs.view_fasta_chunk_cached(cache_p1, buffer, 100, 0);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G
    //----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\n"), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 1, offset 1
    written = fs.view_fasta_chunk_cached(cache_p1, buffer, 100, 1);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G
    //X----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("chr1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA"), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 1, offset 2
    written = fs.view_fasta_chunk_cached(cache_p1, buffer, 100, 2);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G
    //XX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("hr1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA\n"), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 1, offset 3
    written = fs.view_fasta_chunk_cached(cache_p1, buffer, 100, 3);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G
    //XXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("r1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA\nA"), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 1, offset 4
    written = fs.view_fasta_chunk_cached(cache_p1, buffer, 100, 4);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G
    //XXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("1\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA\nA\n"), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 1, offset 5
    written = fs.view_fasta_chunk_cached(cache_p1, buffer, 100, 5);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 T T T T C C C C A A A A G G G G >chr2 A C T G A C T G N N N N A C T G >chr3.1 A C T G A C T G A A A A C >chr3.2 A C T G A C T G A A A A C C >chr3.3 A C T G A C T G A A A A C C C >chr4 A C T G N N N N >chr5 N N A C T G
    //XXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("\nT\nT\nT\nT\nC\nC\nC\nC\nA\nA\nA\nA\nG\nG\nG\nG\n>chr2\nA\nC\nT\nG\nA\nC\nT\nG\nN\nN\nN\nN\nA\nC\nT\nG\n>chr3.1\nA\nC\nT\nG\nA\nC\nT\nG\nA\nA\nA"), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 4, offset: 6
    written = fs.view_fasta_chunk_cached(cache_p4, buffer, 100, 6);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTG ACTG AAAA CCC >chr4 ACTG NNNN >chr5 NNAC TG
    //XXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("TTTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>ch"), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 4, offset: 7
    written = fs.view_fasta_chunk_cached(cache_p4, buffer, 100, 7);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTG ACTG AAAA CCC >chr4 ACTG NNNN >chr5 NNAC TG
    //XXXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("TTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr"), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 4, offset: 8
    written = fs.view_fasta_chunk_cached(cache_p4, buffer, 100, 8);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTG ACTG AAAA CCC >chr4 ACTG NNNN >chr5 NNAC TG
    //XXXXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("TT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr3"), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 4, offset: 9
    written = fs.view_fasta_chunk_cached(cache_p4, buffer, 100, 9);
    BOOST_CHECK_EQUAL(written, 100);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTG ACTG AAAA CCC >chr4 ACTG NNNN >chr5 NNAC TG
    //XXXXXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(std_buffer.compare("T\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr3."), 0);
    flush_buffer(buffer, 100, '?');

    // padding: 4, offset: 10
    written = fs.view_fasta_chunk_cached(cache_p4, buffer, 100, 10);
    std_buffer = std::string(buffer, 100);
    //>chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTG ACTG AAAA CCC >chr4 ACTG NNNN >chr5 NNAC TG
    //XXXXXXXXXX----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|----.----|
    BOOST_CHECK_EQUAL(written, 100);
    BOOST_CHECK_EQUAL(std_buffer.compare("\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr3.3"), 0);
    flush_buffer(buffer, 100, '?');

    std::string full_file = ">chr1\nTTTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr3.3\nACTG\nACTG\nAAAA\nCCC\n>chr4\nACTG\nNNNN\n>chr5\nNNAC\nTG\n";
    //std::string full_file = ">chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTG ACTG AAAA CCC >chr4 ACTG NNNN >chr5 NNAC TG ";
    for(uint32_t offset = 0; offset < 62; ++offset) {
        std::string substr_file = full_file.substr(offset, 100);

        written = fs.view_fasta_chunk_cached(cache_p4, buffer, 100, offset);
        std_buffer = std::string(buffer, substr_file.size());

        BOOST_CHECK_EQUAL_MESSAGE(written, substr_file.size(), "Difference in size for size=" << substr_file.size() << " [found=" << written << "] for offset=" << offset);
        BOOST_CHECK_EQUAL_MESSAGE(std_buffer.compare(substr_file), 0, "Difference in content for offset=" << offset);

        flush_buffer(buffer, 100, '?');
    }

    delete[] buffer;

    delete cache_p1;
    delete cache_p4;
    delete cache_p5;
    delete cache_p999;
}



BOOST_AUTO_TEST_CASE(test_chunked_viewing_sub)
{
    uint32_t written;
    std::string test_name = "test";
    std::string fasta_file = "test/data/" + test_name + ".fa";
    std::string fastafs_file = "tmp/" + test_name + ".fastafs";

    fasta_to_twobit_fastafs(fasta_file, fastafs_file);
    fastafs fs = fastafs(test_name);
    fs.load(fastafs_file);

    ffs2f_init* cache_p100 = fs.init_ffs2f(100, false);

    char *buffer = new char[100];// buffer needs to be c buffer because of the fuse layer
    flush_buffer(buffer, 100, '?');

    //std::string std_buffer;
    // test fastafs_seq functions
    std::ifstream fh(fastafs_file.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    BOOST_REQUIRE(fh.is_open());

    // 1   2   3   4   5   6   7   8    9   10  11  12  13  14  15  16  17  18  19  20  21  22
    //[>] [c] [h] [r] [3] [.] [1] [\n] [A] [C] [T] [G] [A] [C] [T] [G] [A] [A] [A] [A] [C] [\n]
    BOOST_CHECK_EQUAL(fs.data[2]->fasta_filesize(100), 22);

    written = fs.data[2]->view_fasta_chunk_cached(cache_p100->sequences[2], buffer, 100, 0, &fh);
    BOOST_CHECK_EQUAL(written, 22);

    std::string std_buffer = std::string(buffer, written);
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr3.1\nACTGACTGAAAAC\n"), 0);

    flush_buffer(buffer, 100, '?');
    fh.close();

    delete[] buffer;

    delete cache_p100;
}



BOOST_AUTO_TEST_CASE(test_chunked_viewing2)
{
    std::string test_name = "test_003";
    std::string fasta_file = "test/data/" + test_name + ".fa";
    std::string fastafs_file = "tmp/" + test_name + ".fastafs";

    fasta_to_twobit_fastafs(fasta_file, fastafs_file);
    fastafs fs = fastafs(test_name);
    fs.load(fastafs_file);

    BOOST_REQUIRE_EQUAL(fs.flags.is_complete(), true);

    uint32_t written;
    char *buffer = new char[2110];// file size on disk is 2108 bytes
    flush_buffer(buffer, 2110, '\0');

    std::string std_buffer;
    std::ifstream fh(fasta_file.c_str());
    BOOST_REQUIRE(fh.is_open());

    size_t size;

    fh.seekg(0, std::ios::end);
    size = fh.tellg();

    BOOST_REQUIRE_EQUAL(size, 2108);

    fh.seekg(0, std::ios::beg);
    fh.read(buffer, 2108);
    fh.close();
    std::string full_file = std::string(buffer);

    BOOST_REQUIRE_EQUAL(full_file.size(), 2108);

    flush_buffer(buffer, 2110, '?');
    ffs2f_init* cache = fs.init_ffs2f(60, true);


    /* maak alle substrings:
      [....]
      [...]
      [..]
      [.]
       [...]
       [..]
       [.]
        [..]
        [.]
         [.]
     */
    for(uint32_t start_pos = 0; start_pos < full_file.size(); start_pos++) {
        for(uint32_t buffer_len = (uint32_t) full_file.size() - start_pos; buffer_len > 0; buffer_len--) {
            std::string substr_file = std::string(full_file, start_pos, buffer_len);

            written = fs.view_fasta_chunk_cached(cache, buffer, buffer_len, start_pos);
            std_buffer = std::string(buffer, substr_file.size());
            BOOST_CHECK_EQUAL_MESSAGE(written, substr_file.size(), "Difference in size for size=" << substr_file.size() << " [found=" << written << "] for offset=" << start_pos << " and of length: " << buffer_len);
            BOOST_CHECK_EQUAL_MESSAGE(std_buffer.compare(substr_file), 0, "Difference in content for offset=" << start_pos << " and of length: " << buffer_len);
            /* debug
            if(std_buffer.compare(substr_file) != 0) {
                printf("   %d:  %d  \n", start_pos, buffer_len);

                std::cout << "---- ref: ----\n";
                std::cout << substr_file << "\n";
                std::cout << "----found:----\n";
                std::cout << std_buffer << "\n";
                std::cout << "--------------\n";

                exit(1);
            }*/
            flush_buffer(buffer, 2110, '?');
        }
    }

    delete[] buffer;
    delete cache;
}



BOOST_AUTO_TEST_CASE(test_chunked_viewing_fourbit)
{
    std::string test_name = "test_004";
    std::string fasta_file = "test/data/" + test_name + ".fa";
    std::string fastafs_file = "tmp/" + test_name + ".fastafs";

    fasta_to_fourbit_fastafs(fasta_file, fastafs_file);
    fastafs fs = fastafs(test_name);
    fs.load(fastafs_file);

    BOOST_REQUIRE_EQUAL(fs.flags.is_complete(), true);
    BOOST_REQUIRE_EQUAL(fs.fasta_filesize(32), 98);


    char *buffer = new char[200];// buffer needs to be c buffer because of the fuse layer
    flush_buffer(buffer, 200, '?');

    ffs2f_init* cache_p1 = fs.init_ffs2f(1, true);
    ffs2f_init* cache_p4 = fs.init_ffs2f(4, true);
    ffs2f_init* cache_p5 = fs.init_ffs2f(5, true);
    ffs2f_init* cache_p32 = fs.init_ffs2f(32, true);// allow masking = T
    ffs2f_init* cache_p999 = fs.init_ffs2f(999, true);

    std::string std_buffer;
    uint32_t written;


    // padding = 32, offset = 0
    written = fs.view_fasta_chunk_cached(cache_p32, buffer, 200, 0);
    BOOST_CHECK_EQUAL(written, 98);
    std_buffer = std::string(buffer, 98);
    BOOST_CHECK_EQUAL(std_buffer.compare(">IUPAC\nNBKAHMDCUWGSYVTRHGWVUMTBSDN-----\n-----BGYADNHSMUTRCKWVsbhvdnrtgyc\nmkwuaAVTSDKNB---UGWMHYRC\n"), 0);
    flush_buffer(buffer, 200, '?');

    // padding = 32, offset = 1
    written = fs.view_fasta_chunk_cached(cache_p32, buffer, 200, 1);
    BOOST_CHECK_EQUAL(written, 97);
    std_buffer = std::string(buffer, 97);
    BOOST_CHECK_EQUAL(std_buffer.compare("IUPAC\nNBKAHMDCUWGSYVTRHGWVUMTBSDN-----\n-----BGYADNHSMUTRCKWVsbhvdnrtgyc\nmkwuaAVTSDKNB---UGWMHYRC\n"), 0);
    flush_buffer(buffer, 200, '?');

    // padding = 32, offset = 2
    written = fs.view_fasta_chunk_cached(cache_p32, buffer, 200, 2);
    BOOST_CHECK_EQUAL(written, 96);
    std_buffer = std::string(buffer, 96);
    BOOST_CHECK_EQUAL(std_buffer.compare("UPAC\nNBKAHMDCUWGSYVTRHGWVUMTBSDN-----\n-----BGYADNHSMUTRCKWVsbhvdnrtgyc\nmkwuaAVTSDKNB---UGWMHYRC\n"), 0);
    flush_buffer(buffer, 200, '?');

    // padding = 32, offset = 5
    written = fs.view_fasta_chunk_cached(cache_p32, buffer, 200, 5);
    BOOST_CHECK_EQUAL(written, 93);
    std_buffer = std::string(buffer, 93);
    BOOST_CHECK_EQUAL(std_buffer.compare("C\nNBKAHMDCUWGSYVTRHGWVUMTBSDN-----\n-----BGYADNHSMUTRCKWVsbhvdnrtgyc\nmkwuaAVTSDKNB---UGWMHYRC\n"), 0);
    flush_buffer(buffer, 200, '?');

    // padding = 32, offset = 6
    written = fs.view_fasta_chunk_cached(cache_p32, buffer, 200, 6);
    BOOST_CHECK_EQUAL(written, 92);
    std_buffer = std::string(buffer, 92);
    BOOST_CHECK_EQUAL(std_buffer.compare("\nNBKAHMDCUWGSYVTRHGWVUMTBSDN-----\n-----BGYADNHSMUTRCKWVsbhvdnrtgyc\nmkwuaAVTSDKNB---UGWMHYRC\n"), 0);
    flush_buffer(buffer, 200, '?');

    // padding = 32, offset = 7
    written = fs.view_fasta_chunk_cached(cache_p32, buffer, 200, 7);
    BOOST_CHECK_EQUAL(written, 91);
    std_buffer = std::string(buffer, 91);
    BOOST_CHECK_EQUAL(std_buffer.compare("NBKAHMDCUWGSYVTRHGWVUMTBSDN-----\n-----BGYADNHSMUTRCKWVsbhvdnrtgyc\nmkwuaAVTSDKNB---UGWMHYRC\n"), 0);
    flush_buffer(buffer, 200, '?');

    // padding = 32, offset = 8
    written = fs.view_fasta_chunk_cached(cache_p32, buffer, 200, 8);
    BOOST_CHECK_EQUAL(written, 90);
    std_buffer = std::string(buffer, 90);
    BOOST_CHECK_EQUAL(std_buffer.compare("BKAHMDCUWGSYVTRHGWVUMTBSDN-----\n-----BGYADNHSMUTRCKWVsbhvdnrtgyc\nmkwuaAVTSDKNB---UGWMHYRC\n"), 0);
    flush_buffer(buffer, 200, '?');

    // padding = 32, offset = 9
    written = fs.view_fasta_chunk_cached(cache_p32, buffer, 200, 9);
    BOOST_CHECK_EQUAL(written, 89);
    std_buffer = std::string(buffer, 89);
    BOOST_CHECK_EQUAL(std_buffer.compare("KAHMDCUWGSYVTRHGWVUMTBSDN-----\n-----BGYADNHSMUTRCKWVsbhvdnrtgyc\nmkwuaAVTSDKNB---UGWMHYRC\n"), 0);
    flush_buffer(buffer, 200, '?');

    // padding = 32, offset = 10
    written = fs.view_fasta_chunk_cached(cache_p32, buffer, 200, 10);
    BOOST_CHECK_EQUAL(written, 88);
    std_buffer = std::string(buffer, 88);
    BOOST_CHECK_EQUAL(std_buffer.compare("AHMDCUWGSYVTRHGWVUMTBSDN-----\n-----BGYADNHSMUTRCKWVsbhvdnrtgyc\nmkwuaAVTSDKNB---UGWMHYRC\n"), 0);
    flush_buffer(buffer, 200, '?');


    // padding = 1, offset = 0
    written = fs.view_fasta_chunk_cached(cache_p1, buffer, 200, 0);
    BOOST_CHECK_EQUAL(written, 183);
    std_buffer = std::string(buffer, 183);
    BOOST_CHECK_EQUAL(std_buffer.compare(">IUPAC\nN\nB\nK\nA\nH\nM\nD\nC\nU\nW\nG\nS\nY\nV\nT\nR\nH\nG\nW\nV\nU\nM\nT\nB\nS\nD\nN\n-\n-\n-\n-\n-\n-\n-\n-\n-\n-\nB\nG\nY\nA\nD\nN\nH\nS\nM\nU\nT\nR\nC\nK\nW\nV\ns\nb\nh\nv\nd\nn\nr\nt\ng\ny\nc\nm\nk\nw\nu\na\nA\nV\nT\nS\nD\nK\nN\nB\n-\n-\n-\nU\nG\nW\nM\nH\nY\nR\nC\n"), 0);
    flush_buffer(buffer, 200, '?');

    // padding = 5, offset = 0
    written = fs.view_fasta_chunk_cached(cache_p5, buffer, 200, 0);
    BOOST_CHECK_EQUAL(written, 113);
    std_buffer = std::string(buffer, 113);
    BOOST_CHECK_EQUAL(std_buffer.compare(">IUPAC\nNBKAH\nMDCUW\nGSYVT\nRHGWV\nUMTBS\nDN---\n-----\n--BGY\nADNHS\nMUTRC\nKWVsb\nhvdnr\ntgycm\nkwuaA\nVTSDK\nNB---\nUGWMH\nYRC\n"), 0);
    flush_buffer(buffer, 200, '?');

    // padding = 999, offset = 0
    written = fs.view_fasta_chunk_cached(cache_p999, buffer, 200, 0);
    BOOST_CHECK_EQUAL(written, 96);
    std_buffer = std::string(buffer, 96);
    BOOST_CHECK_EQUAL(std_buffer.compare(">IUPAC\nNBKAHMDCUWGSYVTRHGWVUMTBSDN----------BGYADNHSMUTRCKWVsbhvdnrtgycmkwuaAVTSDKNB---UGWMHYRC\n"), 0);
    flush_buffer(buffer, 200, '?');



    std::string full_file = ">IUPAC\nNBKA\nHMDC\nUWGS\nYVTR\nHGWV\nUMTB\nSDN-\n----\n----\n-BGY\nADNH\nSMUT\nRCKW\nVsbh\nvdnr\ntgyc\nmkwu\naAVT\nSDKN\nB---\nUGWM\nHYRC\n";// length = 117
    for(uint32_t offset = 0; offset < 62; ++offset) {
        std::string substr_file = full_file.substr(offset, 200);

        written = fs.view_fasta_chunk_cached(cache_p4, buffer, 200, offset);
        std_buffer = std::string(buffer, substr_file.size());

        BOOST_CHECK_EQUAL_MESSAGE(written, substr_file.size(), "Difference in size for size=" << substr_file.size() << " [found=" << written << "] for offset=" << offset);
        BOOST_CHECK_EQUAL_MESSAGE(std_buffer.compare(substr_file), 0, "Difference in content for offset=" << offset);

        flush_buffer(buffer, 200, '?');
    }

    delete[] buffer;

    delete cache_p1;
    delete cache_p4;
    delete cache_p5;
    delete cache_p999;
}



BOOST_AUTO_TEST_SUITE_END()

