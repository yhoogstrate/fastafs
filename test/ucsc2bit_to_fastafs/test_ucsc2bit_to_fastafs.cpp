#define BOOST_TEST_MODULE ucsc2bit_to_fastafs

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "utils.hpp"
#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"
#include "ucsc2bit_to_fastafs.hpp"

#include <vector>


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_ucsc2bit_to_fasta)
{
    std::string fastafs_file = "tmp/test.fastafs";
    std::string fastafs_file2 = "tmp/test.regenerated.fastafs";
    std::string ucsc2bit_file = "tmp/test.2bit";

    // 01 fasta_to_fastafs()
    fasta_to_fastafs("test/data/test.fa", fastafs_file, false);

    // 02 load fastafs
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    // 03 fastafs::chunked ucsc2bit view of the file
    std::fstream ucsc2bit_out_stream(ucsc2bit_file.c_str(), std::ios :: out | std::ios :: binary);
    if(ucsc2bit_out_stream.is_open()) {
        char buffer[READ_BUFFER_SIZE +  1];

        uint32_t written = fs.view_ucsc2bit_chunk(buffer, READ_BUFFER_SIZE, 0);
        std::string buffer_s = std_string_nullbyte_safe(buffer, written);

        ucsc2bit_out_stream << buffer_s;
        ucsc2bit_out_stream.close();
    } else {
        throw std::runtime_error("Could not write to file: " + ucsc2bit_file);
    }

    // 04 ucsc2bit_to_fasta
    size_t written = ucsc2bit_to_fastafs(ucsc2bit_file, fastafs_file2);

    // - comparison based on iterators: <https://stackoverflow.com/questions/15022036/how-to-compare-files-with-boost-test>
    std::ifstream ifs1(fastafs_file);
    std::ifstream ifs2(fastafs_file2);
    std::istream_iterator<char> b1(ifs1), e1;
    std::istream_iterator<char> b2(ifs2), e2;

    BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
    BOOST_CHECK_EQUAL(written, (size_t) 403);
}



BOOST_AUTO_TEST_SUITE_END()
