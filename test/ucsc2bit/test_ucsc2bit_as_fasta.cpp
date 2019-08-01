#define BOOST_TEST_MODULE ucsc2bit_as_fasta

#include <sys/types.h>
#include <sys/stat.h>


#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "ucsc2bit.hpp"
#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"


/*
// @todo to utils?
void flush_buffer(char *buffer, size_t n, char fill)
{
    for(size_t i = 0; i < n; i++) {
        buffer[i] = fill;
    }
}
*/


BOOST_AUTO_TEST_SUITE(Testing)

BOOST_AUTO_TEST_CASE(init)
{
    // make tmp dir
    mkdir("tmp", S_IRWXU);
}


BOOST_AUTO_TEST_CASE(test_ucsc2bit_to_fasta_file)
{
    //BOOST_CHECK_EQUAL(5, 6);

    // 1: FASTA to FASTAFS file:
    std::string fastafs_file = "tmp/test.fastafs";
    fasta_to_fastafs("test/data/test.fa", fastafs_file);
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);
    BOOST_REQUIRE(fs.data.size() > 0);

    // 2: FASTAFS to UCSC2BIT file:
    std::string ucsc2bit_file = "tmp/test.2bit";
    char buffer[READ_BUFFER_SIZE + 1];
    std::fstream ucsc2bit_out_stream(ucsc2bit_file.c_str(), std::ios :: out | std::ios :: binary);
    if(ucsc2bit_out_stream.is_open()) {
        uint32_t written = fs.view_ucsc2bit_chunk(buffer, READ_BUFFER_SIZE, 0);
        std::string buffer_s = std_string_nullbyte_safe(buffer, written);

        ucsc2bit_out_stream << buffer_s;
        ucsc2bit_out_stream.close();
    } else {
        throw std::runtime_error("Could not write to file: " + ucsc2bit_file);
    }


    ucsc2bit u2b = ucsc2bit("test");
    u2b.load(ucsc2bit_file);

    // testing
    BOOST_CHECK_EQUAL(u2b.n(), 88);
    BOOST_CHECK_EQUAL(u2b.fasta_filesize(100), 143);
    BOOST_CHECK_EQUAL(u2b.fasta_filesize(4), 160);


    //uint32_t fastafs::view_ucsc2bit_chunk(char *buffer, size_t buffer_size, off_t file_offset)

    /*
        //                   1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
        // 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
        // >  c  h  r  1 \n  t  t  t  t  c  c  c  c  a  a  a  a  g  g  g  g \n
        BOOST_CHECK_EQUAL(fs.data[0]->fasta_filesize(100), 23);
        BOOST_CHECK_EQUAL(fs.data[0]->fasta_filesize(16), 23);

        //                   1  2  3  4  5  6  7  8  9 10 11 12 13 14 15    16
        // 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
        // >  c  h  r  1 \n  t  t  t  t  c  c  c  c  a  a  a  a  g  g  g \n  g \n
        BOOST_CHECK_EQUAL(fs.data[0]->fasta_filesize(15), 24);
        std::ifstream file(fs.filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
        BOOST_REQUIRE(file.is_open());

        ffs2f_init* cache_p100 = fs.init_ffs2f(100, true);
        ffs2f_init* cache_p23 = fs.init_ffs2f(23, true);

        // then: check returncodes:
        uint32_t ret;
        char chunk[4];
        for(uint32_t i = 0; i < 23; i++) {
            ret = fs.data[0]->view_fasta_chunk_cached(cache_p100->sequences[0], chunk, 1, i, &file);
            BOOST_CHECK_EQUAL(ret, 1);
        }
        for(uint32_t i = 23; i < 23 + 5; i++) {
            ret = fs.data[0]->view_fasta_chunk_cached(cache_p100->sequences[0], chunk, 1, i, &file);
            BOOST_CHECK_EQUAL(ret, 0);
        }

        chunk[1] = '\0';
        chunk[2] = '\1';
        chunk[3] = '\2';

        std::string ref = ">chr1\nttttccccaaaagggg\n";
        for(uint32_t i = 0; i < ref.size(); i++) {
            ret = fs.data[0]->view_fasta_chunk_cached(cache_p23->sequences[0], chunk, 1, i, &file);
            BOOST_CHECK_EQUAL(chunk[0], ref[i]); // test for '>'
            BOOST_CHECK_EQUAL(ret, 1);
        }

        BOOST_CHECK_EQUAL(chunk[1], '\0');
        BOOST_CHECK_EQUAL(chunk[2], '\1');
        BOOST_CHECK_EQUAL(chunk[3], '\2');

        file.close();

        delete cache_p100;
        delete cache_p23;
        */
}




BOOST_AUTO_TEST_SUITE_END()
