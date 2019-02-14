#define BOOST_TEST_MODULE fastfs_cache

#include <sys/types.h>
#include <sys/stat.h>


#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"


BOOST_AUTO_TEST_SUITE(Testing)

BOOST_AUTO_TEST_CASE(init)
{
    // make tmp dir
    mkdir("tmp", S_IRWXU);
}


BOOST_AUTO_TEST_CASE(test_fastafs_seq_fastafile_size)
{
    // 1: create FASTAFS file
    std::string fastafs_file = "tmp/test.fastafs";

    fasta_to_fastafs f = fasta_to_fastafs("test", "test/data/test.fa");
    f.cache();
    f.write(fastafs_file);

    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    BOOST_REQUIRE(fs.data.size() > 0);

    //                   1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
    // 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
    // >  c  h  r  1 \n  T  T  T  T  C  C  C  C  A  A  A  A  G  G  G  G \n
    BOOST_CHECK_EQUAL(fs.data[0]->fasta_filesize(100), 23);
    BOOST_CHECK_EQUAL(fs.data[0]->fasta_filesize(16), 23);

    //                   1  2  3  4  5  6  7  8  9 10 11 12 13 14 15    16
    // 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
    // >  c  h  r  1 \n  T  T  T  T  C  C  C  C  A  A  A  A  G  G  G \n  G \n
    BOOST_CHECK_EQUAL(fs.data[0]->fasta_filesize(15), 24);



    std::ifstream file (fs.filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    BOOST_REQUIRE(file.is_open());


    // then: check returncodes:
    unsigned int ret;
    char chunk[4];

    for(unsigned int i = 0; i < 23; i++) {
        ret = fs.data[0]->view_fasta_chunk(100, chunk, i, 1, &file);
        BOOST_CHECK_EQUAL(ret, 1);
    }
    for(unsigned int i = 23; i < 23 + 5; i++) {
        ret = fs.data[0]->view_fasta_chunk(100, chunk, i, 1, &file);
        BOOST_CHECK_EQUAL(ret, 0);
    }


    chunk[1] = '\0';
    chunk[2] = '\1';
    chunk[3] = '\2';

    unsigned int i = 0;

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], '>');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'c');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'h');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'r');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], '1');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], '\n');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'T');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'T');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'T');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'T');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'C');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'C');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'C');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'C');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'A');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'A');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'A');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'A');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'G');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'G');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'G');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'G');

    ret = fs.data[0]->view_fasta_chunk(23, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], '\n');

    BOOST_CHECK_EQUAL(chunk[1], '\0');
    BOOST_CHECK_EQUAL(chunk[2], '\1');
    BOOST_CHECK_EQUAL(chunk[3], '\2');


    file.close();
}


BOOST_AUTO_TEST_CASE(test_fastafs_seq_fastafile_size_padding_0)
{
    // 1: create FASTAFS file
    std::string fastafs_file = "tmp/test.fastafs";

    fasta_to_fastafs f = fasta_to_fastafs("test", "test/data/test.fa");
    f.cache();
    f.write(fastafs_file);

    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    BOOST_REQUIRE(fs.data.size() > 0);

    //                   1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
    // 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
    // >  c  h  r  1 \n  T  T  T  T  C  C  C  C  A  A  A  A  G  G  G  G \n
    BOOST_CHECK_EQUAL(fs.data[0]->fasta_filesize(0), 23);

    std::ifstream file (fs.filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    BOOST_REQUIRE(file.is_open());


    // then: check returncodes:
    unsigned int ret;
    char chunk[1];

    unsigned int i = 0;


    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], '>');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'c');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'h');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'r');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], '1');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], '\n');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'T');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'T');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'T');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'T');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'C');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'C');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'C');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'C');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'A');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'A');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'A');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'A');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'G');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'G');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'G');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], 'G');

    ret = fs.data[0]->view_fasta_chunk(0, chunk, i++, 1, &file);
    BOOST_CHECK_EQUAL(chunk[0], '\n');

    file.close();
}


BOOST_AUTO_TEST_CASE(test_fastafs_seq_sha1)
{
    // 1: create FASTAFS file
    std::string fastafs_file = "tmp/test.fastafs";

    fasta_to_fastafs f = fasta_to_fastafs("test", "test/data/test.fa");
    f.cache();
    f.write(fastafs_file);

    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    //printf("[%i]\n", fs.data.size());
    BOOST_REQUIRE(fs.data.size() > 0);

    std::ifstream file (fs.filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);    
    BOOST_REQUIRE(file.is_open());
    
    //printf("[%s]\n", fs.data[0]->sha1(&file).c_str());
    fs.data[0]->sha1(&file);

    BOOST_CHECK_EQUAL(fs.data[0]->sha1(&file), "2c0cae1d4e272b3ba63e7dd7e3c0efe62f2aaa2f");
}




/**
 * @description test contains a sequence that intially failed chunked_view with chunk size > 1
 */
BOOST_AUTO_TEST_CASE(test_fastafs_seq_sha1b)
{

    // 1: create FASTAFS file
    std::string fastafs_file = "tmp/test.fastafs";

    fasta_to_fastafs f = fasta_to_fastafs("test", "test/data/test_002.fa");
    f.cache();
    f.write(fastafs_file);

    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    BOOST_REQUIRE(fs.data.size() > 0);

    std::ifstream file (fs.filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);    
    BOOST_REQUIRE(file.is_open());
    
    BOOST_CHECK_EQUAL(fs.check_integrity(false), 0);
}





BOOST_AUTO_TEST_SUITE_END()