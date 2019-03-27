#define BOOST_TEST_MODULE fastfs_test_view


#include <iostream>
#include <fstream>

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"



void flush_buffer(char *buffer, size_t n, char fill) {
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

    fasta_to_fastafs f = fasta_to_fastafs("test", "test/data/test.fa");
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

    std::string test_name = "test";
    std::string fasta_file = "test/data/" + test_name + ".fa";
    std::string fastafs_file = "tmp/" + test_name + ".fastafs";

    fasta_to_fastafs f = fasta_to_fastafs(test_name, fasta_file);
    f.cache();
    f.write(fastafs_file);


    fastafs fs = fastafs(test_name);
    fs.load(fastafs_file);

    char *buffer = new char[100];// buffer needs to be c buffer because of the fuse layer
    std::string std_buffer;


    flush_buffer(buffer, 100, '?');

/*



*/

    std::string full_file = ">chr1\nTTTT\nCCCC\nAAAA\nGGGG\n>chr2\nACTG\nACTG\nNNNN\nACTG\n>chr3.1\nACTG\nACTG\nAAAA\nC\n>chr3.2\nACTG\nACTG\nAAAA\nCC\n>chr3.3\nACTG\nACTG\nAAAA\nCCC\n>chr4\nACTG\nNNNN\n>chr5\nNNAC\nTG\n";
    //std::string full_file = ">chr1 TTTT CCCC AAAA GGGG >chr2 ACTG ACTG NNNN ACTG >chr3.1 ACTG ACTG AAAA C >chr3.2 ACTG ACTG AAAA CC >chr3.3 ACTG ACTG AAAA CCC >chr4 ACTG NNNN >chr5 NNAC TG ";
    for(unsigned int offset = 0; offset < 45; ++offset) {
        std::string substr_file = full_file.substr(offset, 30);

        written = fs.view_fasta_chunk(4, buffer, 30, offset);
        std_buffer = std::string(buffer, substr_file.size());

        BOOST_CHECK_EQUAL_MESSAGE(written, substr_file.size(), "Difference in size for size=" << substr_file.size() << " [found=" << written << "] for offset=" << offset );
        BOOST_CHECK_EQUAL_MESSAGE(std_buffer.compare(substr_file), 0, "Difference in content for offset=" << offset );
        flush_buffer(buffer, 30, '?');
        
        std::cout << substr_file;
        std::cout << "\n---\n";
        std::cout << std_buffer;
        
    }
    delete[] buffer;
}



BOOST_AUTO_TEST_SUITE_END()
