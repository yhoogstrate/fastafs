#define BOOST_TEST_MODULE fastfs_fastafs

#include <sys/types.h>
#include <sys/stat.h>


#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"



// @todo to utils?
void flush_buffer(char *buffer, size_t n, char fill)
{
    for(size_t i = 0; i < n; i++) {
        buffer[i] = fill;
    }
}



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
    fasta_to_fastafs("test/data/test.fa", fastafs_file);
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);
    BOOST_REQUIRE(fs.data.size() > 0);

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
}


BOOST_AUTO_TEST_CASE(test_fastafs_seq_fastafile_size_padding_0)
{
    // 1: create FASTAFS file
    std::string fastafs_file = "tmp/test.fastafs";
    fasta_to_fastafs("test/data/test.fa", fastafs_file);
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);
    BOOST_REQUIRE(fs.data.size() > 0);

    //                   1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
    // 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
    // >  c  h  r  1 \n  T  T  T  T  C  C  C  C  A  A  A  A  G  G  G  G \n
    BOOST_CHECK_EQUAL(fs.data[0]->fasta_filesize(fs.data[0]->n), 23);
    std::ifstream file(fs.filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    BOOST_REQUIRE(file.is_open());
    ffs2f_init* cache_p0 = fs.init_ffs2f(0, true);

    // then: check returncodes:
    uint32_t ret;
    char chunk[1];

    std::string ref = ">chr1\nttttccccaaaagggg\n";

    for(uint32_t i = 0; i < ref.size(); i++) {
        ret = fs.data[0]->view_fasta_chunk_cached(cache_p0->sequences[0], chunk, 1, i, &file);
        BOOST_CHECK_EQUAL(chunk[0], ref[i]); // test for '>'
        BOOST_CHECK_EQUAL(ret, 1);
    }

    // check if out of bound query returns 0
    ret = fs.data[0]->view_fasta_chunk_cached(cache_p0->sequences[0], chunk, 1, ref.size(), &file);
    BOOST_CHECK_EQUAL(ret, 0);

    file.close();

    delete cache_p0;
}


BOOST_AUTO_TEST_CASE(test_fastafs_seq_fastafile_size_padding_0__no_masking)
{
    // 1: create FASTAFS file
    std::string fastafs_file = "tmp/test.fastafs";
    fasta_to_fastafs("test/data/test.fa", fastafs_file);

    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    BOOST_REQUIRE(fs.data.size() > 0);

    //                   1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
    // 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
    // >  c  h  r  1 \n  T  T  T  T  C  C  C  C  A  A  A  A  G  G  G  G \n
    BOOST_CHECK_EQUAL(fs.data[0]->fasta_filesize(fs.data[0]->n), 23);
    std::ifstream file(fs.filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    BOOST_REQUIRE(file.is_open());

    ffs2f_init* cache_p0 = fs.init_ffs2f(0, false); // no masking; everything must be uppercase

    // then: check returncodes:
    uint32_t ret;
    char chunk[1];
    std::string ref = ">chr1\nTTTTCCCCAAAAGGGG\n";

    for(uint32_t i = 0; i < ref.size(); i++) {
        ret = fs.data[0]->view_fasta_chunk_cached(cache_p0->sequences[0], chunk, 1, i, &file);
        BOOST_CHECK_EQUAL(chunk[0], ref[i]); // test for '>'
        BOOST_CHECK_EQUAL(ret, 1);
    }

    // check if out of bound query returns 0
    ret = fs.data[0]->view_fasta_chunk_cached(cache_p0->sequences[0], chunk, 1, ref.size(), &file);
    BOOST_CHECK_EQUAL(ret, 0);

    file.close();

    delete cache_p0;
}


BOOST_AUTO_TEST_CASE(test_fastafs_seq_sha1)
{
    // 1: create FASTAFS file
    std::string fastafs_file = "tmp/test.fastafs";
    fasta_to_fastafs("test/data/test.fa", fastafs_file);
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    ffs2f_init* cache_p0 = fs.init_ffs2f(0, false); // allow masking = false, alles moet in capital / upper case
    BOOST_REQUIRE(fs.data.size() > 0);

    std::ifstream file(fs.filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    BOOST_REQUIRE(file.is_open());

    //fs.data[0]->sha1(cache_p0->sequences[0], &file);
    BOOST_CHECK_EQUAL(fs.data[0]->sha1(cache_p0->sequences[0], &file), "2c0cae1d4e272b3ba63e7dd7e3c0efe62f2aaa2f");

    delete cache_p0;
}


BOOST_AUTO_TEST_CASE(test_fastafs_seq_md5)
{
    // 1: create FASTAFS file
    std::string fastafs_file = "tmp/test.fastafs";
    fasta_to_fastafs("test/data/test.fa", fastafs_file);
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    ffs2f_init* cache = fs.init_ffs2f(0, false); // allow masking = false, alles moet in capital / upper case
    BOOST_REQUIRE(fs.data.size() > 0);

    std::ifstream file(fs.filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    BOOST_REQUIRE(file.is_open());

    BOOST_CHECK_EQUAL(fs.data[0]->md5(cache->sequences[0], &file), "75255c6d90778999ad3643a2e69d4344");
    BOOST_CHECK_EQUAL(fs.data[1]->md5(cache->sequences[1], &file), "8b5673724a9965c29a1d76fe7031ac8a");
    BOOST_CHECK_EQUAL(fs.data[2]->md5(cache->sequences[2], &file), "61deba32ec4c3576e3998fa2d4b87288");
    BOOST_CHECK_EQUAL(fs.data[3]->md5(cache->sequences[3], &file), "99b90560f23c1bda2871a6c93fd6a240");
    BOOST_CHECK_EQUAL(fs.data[4]->md5(cache->sequences[4], &file), "3625afdfbeb43765b85f612e0acb4739");
    BOOST_CHECK_EQUAL(fs.data[5]->md5(cache->sequences[5], &file), "bd8c080ed25ba8a454d9434cb8d14a68");
    BOOST_CHECK_EQUAL(fs.data[6]->md5(cache->sequences[6], &file), "980ef3a1cd80afec959dcf852d026246");

    delete cache;
}



/**
 * @description test contains a sequence that intially failed chunked_view with chunk size > 1
 */
BOOST_AUTO_TEST_CASE(test_fastafs_seq_sha1b)
{
    // 1: create FASTAFS file
    std::string fastafs_file = "tmp/test.fastafs";
    fasta_to_fastafs("test/data/test_002.fa", fastafs_file);
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    BOOST_REQUIRE(fs.data.size() > 0);

    std::ifstream file(fs.filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    BOOST_REQUIRE(file.is_open());

    BOOST_CHECK_EQUAL(fs.check_integrity(), 0);
}




/**
 * @description test contains a sequence that intially failed chunked_view with chunk size > 1
 */
BOOST_AUTO_TEST_CASE(test_fastafs_seq__get_n_offset)
{
    // pretend the following sequence:
    // NNxxNxNNxxxN
    std::string seq = "NNxxNxNNxxxN";

    // starts:         0   4 6    11
    // ends:            1  4  7   11
    fastafs_seq f = fastafs_seq();
    f.n = (uint32_t) seq.size();
    f.n_starts = {0, 4, 6, 11};
    f.n_ends = {1, 4, 7, 11};
    uint32_t n_passed;
    bool in_n;

    // NNxxNxNNxxxN
    // |
    in_n = f.get_n_offset(0, &n_passed);
    BOOST_CHECK_EQUAL(n_passed, 0);
    BOOST_CHECK_EQUAL(in_n, true);

    // NNxxNxNNxxxN
    // -|
    in_n = f.get_n_offset(1, &n_passed);
    BOOST_CHECK_EQUAL(n_passed, 1);
    BOOST_CHECK_EQUAL(in_n, true);

    // NNxxNxNNxxxN
    // --|
    in_n = f.get_n_offset(2, &n_passed);
    BOOST_CHECK_EQUAL(n_passed, 2);
    BOOST_CHECK_EQUAL(in_n, false);

    // NNxxNxNNxxxN
    // -- |
    in_n = f.get_n_offset(3, &n_passed);
    BOOST_CHECK_EQUAL(n_passed, 2);
    BOOST_CHECK_EQUAL(in_n, false);

    // NNxxNxNNxxxN
    // --  |
    in_n = f.get_n_offset(4, &n_passed);
    BOOST_CHECK_EQUAL(n_passed, 2);
    BOOST_CHECK_EQUAL(in_n, true);

    // NNxxNxNNxxxN
    // --  -|
    in_n = f.get_n_offset(5, &n_passed);
    BOOST_CHECK_EQUAL(n_passed, 3);
    BOOST_CHECK_EQUAL(in_n, false);

    // NNxxNxNNxxxN
    // --  - |
    in_n = f.get_n_offset(6, &n_passed);
    BOOST_CHECK_EQUAL(n_passed, 3);
    BOOST_CHECK_EQUAL(in_n, true);

    // NNxxNxNNxxxN
    // --  - -|
    in_n = f.get_n_offset(7, &n_passed);
    BOOST_CHECK_EQUAL(n_passed, 4);
    BOOST_CHECK_EQUAL(in_n, true);

    // NNxxNxNNxxxN
    // --  - --|
    in_n = f.get_n_offset(8, &n_passed);
    BOOST_CHECK_EQUAL(n_passed, 5);
    BOOST_CHECK_EQUAL(in_n, false);

    // NNxxNxNNxxxN
    // --  - -- |
    in_n = f.get_n_offset(9, &n_passed);
    BOOST_CHECK_EQUAL(n_passed, 5);
    BOOST_CHECK_EQUAL(in_n, false);

    // NNxxNxNNxxxN
    // --  - --  |
    in_n = f.get_n_offset(10, &n_passed);
    BOOST_CHECK_EQUAL(n_passed, 5);
    BOOST_CHECK_EQUAL(in_n, false);

    // NNxxNxNNxxxN
    // --  - --   |
    in_n = f.get_n_offset(11, &n_passed);
    BOOST_CHECK_EQUAL(n_passed, 5);
    BOOST_CHECK_EQUAL(in_n, true);

    // I could test 12, but that behavious is not yet defined, and doesn't need to?
}



/**
 * @description tests size and content of creating dict files
 */
BOOST_AUTO_TEST_CASE(test_fastafs__dict_virtualization)
{
    /*
    @HD	VN:1.0	SO:unsorted
    @SQ	SN:chr1	LN:16	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
    @SQ	SN:chr2	LN:16	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
    @SQ	SN:chr3.1	LN:13	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
    @SQ	SN:chr3.2	LN:14	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
    @SQ	SN:chr3.3	LN:15	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
    @SQ	SN:chr4	LN:8	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
    @SQ	SN:chr5	LN:6	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
     */

    std::string fastafs_file = "tmp/test.fastafs";
    fasta_to_fastafs("test/data/test.fa", fastafs_file);

    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    BOOST_REQUIRE(fs.data.size() > 0);
    BOOST_CHECK_EQUAL(fs.dict_filesize(), 538);


    std::string ref = "@HD	VN:1.0	SO:unsorted\n@SQ	SN:chr1	LN:16	M5:75255c6d90778999ad3643a2e69d4344	UR:fastafs:///test\n@SQ	SN:chr2	LN:16	M5:8b5673724a9965c29a1d76fe7031ac8a	UR:fastafs:///test\n@SQ	SN:chr3.1	LN:13	M5:61deba32ec4c3576e3998fa2d4b87288	UR:fastafs:///test\n@SQ	SN:chr3.2	LN:14	M5:99b90560f23c1bda2871a6c93fd6a240	UR:fastafs:///test\n@SQ	SN:chr3.3	LN:15	M5:3625afdfbeb43765b85f612e0acb4739	UR:fastafs:///test\n@SQ	SN:chr4	LN:8	M5:bd8c080ed25ba8a454d9434cb8d14a68	UR:fastafs:///test\n@SQ	SN:chr5	LN:6	M5:980ef3a1cd80afec959dcf852d026246	UR:fastafs:///test\n";

    size_t written;
    char *buffer;

    // for buffer size ...  {
    buffer = new char[READ_BUFFER_SIZE + 1];
    flush_buffer(buffer, READ_BUFFER_SIZE, '\0');

    written = fs.view_dict_chunk(buffer, READ_BUFFER_SIZE, 0); // char *buffer, size_t READ_BUFFER_SIZE, off_t file_offset

    BOOST_CHECK_EQUAL(written, 538);
    BOOST_CHECK_EQUAL(std::string(buffer, written).compare(ref), 0);

    delete[] buffer;
    // }
}




BOOST_AUTO_TEST_SUITE_END()
