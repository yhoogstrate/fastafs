#define BOOST_TEST_MODULE ucsc2bit_as_fasta

#include <sys/types.h>
#include <sys/stat.h>


#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "ucsc2bit.hpp"
#include "fasta_to_twobit_fastafs.hpp"
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


BOOST_AUTO_TEST_CASE(test_ucsc2bit_to_fasta_file)
{
    //BOOST_CHECK_EQUAL(5, 6);

    // 1: FASTA to FASTAFS file:
    std::string fastafs_file = "tmp/test.fastafs";
    fasta_to_twobit_fastafs("test/data/test.fa", fastafs_file);
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


    size_t written;
    std::string std_buffer;
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');


    written = u2b.view_fasta_chunk(60, buffer, 143 + 100, 0);
    BOOST_CHECK_EQUAL(written, 143);
    std_buffer = std::string(buffer, 143);
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\nttttccccaaaagggg\n>chr2\nACTGACTGnnnnACTG\n>chr3.1\nACTGACTGaaaac\n>chr3.2\nACTGACTGaaaacc\n>chr3.3\nACTGACTGaaaaccc\n>chr4\nACTGnnnn\n>chr5\nnnACTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(60, buffer, 142 + 100, 1);
    BOOST_CHECK_EQUAL(written, 142);
    std_buffer = std::string(buffer, 142);
    BOOST_CHECK_EQUAL(std_buffer.compare("chr1\nttttccccaaaagggg\n>chr2\nACTGACTGnnnnACTG\n>chr3.1\nACTGACTGaaaac\n>chr3.2\nACTGACTGaaaacc\n>chr3.3\nACTGACTGaaaaccc\n>chr4\nACTGnnnn\n>chr5\nnnACTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(60, buffer, 141 + 100, 2);
    BOOST_CHECK_EQUAL(written, 141);
    std_buffer = std::string(buffer, 141);
    BOOST_CHECK_EQUAL(std_buffer.compare("hr1\nttttccccaaaagggg\n>chr2\nACTGACTGnnnnACTG\n>chr3.1\nACTGACTGaaaac\n>chr3.2\nACTGACTGaaaacc\n>chr3.3\nACTGACTGaaaaccc\n>chr4\nACTGnnnn\n>chr5\nnnACTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(60, buffer, 140 + 100, 3);
    BOOST_CHECK_EQUAL(written, 140);
    std_buffer = std::string(buffer, 140);
    BOOST_CHECK_EQUAL(std_buffer.compare("r1\nttttccccaaaagggg\n>chr2\nACTGACTGnnnnACTG\n>chr3.1\nACTGACTGaaaac\n>chr3.2\nACTGACTGaaaacc\n>chr3.3\nACTGACTGaaaaccc\n>chr4\nACTGnnnn\n>chr5\nnnACTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(60, buffer, 139 + 100, 4);
    BOOST_CHECK_EQUAL(written, 139);
    std_buffer = std::string(buffer, 139);
    BOOST_CHECK_EQUAL(std_buffer.compare("1\nttttccccaaaagggg\n>chr2\nACTGACTGnnnnACTG\n>chr3.1\nACTGACTGaaaac\n>chr3.2\nACTGACTGaaaacc\n>chr3.3\nACTGACTGaaaaccc\n>chr4\nACTGnnnn\n>chr5\nnnACTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(60, buffer, 138 + 100, 5);
    BOOST_CHECK_EQUAL(written, 138);
    std_buffer = std::string(buffer, 138);
    BOOST_CHECK_EQUAL(std_buffer.compare("\nttttccccaaaagggg\n>chr2\nACTGACTGnnnnACTG\n>chr3.1\nACTGACTGaaaac\n>chr3.2\nACTGACTGaaaacc\n>chr3.3\nACTGACTGaaaaccc\n>chr4\nACTGnnnn\n>chr5\nnnACTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(60, buffer, 137 + 100, 6);
    BOOST_CHECK_EQUAL(written, 137);
    std_buffer = std::string(buffer, 137);
    BOOST_CHECK_EQUAL(std_buffer.compare("ttttccccaaaagggg\n>chr2\nACTGACTGnnnnACTG\n>chr3.1\nACTGACTGaaaac\n>chr3.2\nACTGACTGaaaacc\n>chr3.3\nACTGACTGaaaaccc\n>chr4\nACTGnnnn\n>chr5\nnnACTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(60, buffer, 136 + 100, 7);
    BOOST_CHECK_EQUAL(written, 136);
    std_buffer = std::string(buffer, 136);
    BOOST_CHECK_EQUAL(std_buffer.compare("tttccccaaaagggg\n>chr2\nACTGACTGnnnnACTG\n>chr3.1\nACTGACTGaaaac\n>chr3.2\nACTGACTGaaaacc\n>chr3.3\nACTGACTGaaaaccc\n>chr4\nACTGnnnn\n>chr5\nnnACTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(60, buffer, 1, 7);
    BOOST_CHECK_EQUAL(written, 1);
    std_buffer = std::string(buffer, 1);
    BOOST_CHECK_EQUAL(std_buffer.compare("t"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 160 + 100, 0);
    BOOST_CHECK_EQUAL(written, 160);
    std_buffer = std::string(buffer, 160);
    BOOST_CHECK_EQUAL(std_buffer.compare(">chr1\ntttt\ncccc\naaaa\ngggg\n>chr2\nACTG\nACTG\nnnnn\nACTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 150 + 100, 10);
    BOOST_CHECK_EQUAL(written, 150);
    std_buffer = std::string(buffer, 150);
    BOOST_CHECK_EQUAL(std_buffer.compare("\ncccc\naaaa\ngggg\n>chr2\nACTG\nACTG\nnnnn\nACTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 149 + 100, 11);
    BOOST_CHECK_EQUAL(written, 149);
    std_buffer = std::string(buffer, 149);
    BOOST_CHECK_EQUAL(std_buffer.compare("cccc\naaaa\ngggg\n>chr2\nACTG\nACTG\nnnnn\nACTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 148 + 100, 12);
    BOOST_CHECK_EQUAL(written, 148);
    std_buffer = std::string(buffer, 148);
    BOOST_CHECK_EQUAL(std_buffer.compare("ccc\naaaa\ngggg\n>chr2\nACTG\nACTG\nnnnn\nACTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 120 + 100, 40);
    BOOST_CHECK_EQUAL(written, 120);
    std_buffer = std::string(buffer, 120);
    BOOST_CHECK_EQUAL(std_buffer.compare("G\nnnnn\nACTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 119 + 100, 41);
    BOOST_CHECK_EQUAL(written, 119);
    std_buffer = std::string(buffer, 119);
    BOOST_CHECK_EQUAL(std_buffer.compare("\nnnnn\nACTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 118 + 100, 42);
    BOOST_CHECK_EQUAL(written, 118);
    std_buffer = std::string(buffer, 118);
    BOOST_CHECK_EQUAL(std_buffer.compare("nnnn\nACTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 117 + 100, 43);
    BOOST_CHECK_EQUAL(written, 117);
    std_buffer = std::string(buffer, 117);
    BOOST_CHECK_EQUAL(std_buffer.compare("nnn\nACTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 116 + 100, 44);
    BOOST_CHECK_EQUAL(written, 116);
    std_buffer = std::string(buffer, 116);
    BOOST_CHECK_EQUAL(std_buffer.compare("nn\nACTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 115 + 100, 45);
    BOOST_CHECK_EQUAL(written, 115);
    std_buffer = std::string(buffer, 115);
    BOOST_CHECK_EQUAL(std_buffer.compare("n\nACTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 114 + 100, 46);
    BOOST_CHECK_EQUAL(written, 114);
    std_buffer = std::string(buffer, 114);
    BOOST_CHECK_EQUAL(std_buffer.compare("\nACTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 113 + 100, 47);
    BOOST_CHECK_EQUAL(written, 113);
    std_buffer = std::string(buffer, 113);
    BOOST_CHECK_EQUAL(std_buffer.compare("ACTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 112 + 100, 48);
    BOOST_CHECK_EQUAL(written, 112);
    std_buffer = std::string(buffer, 112);
    BOOST_CHECK_EQUAL(std_buffer.compare("CTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>chr5\nnnAC\nTG\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 97, 48);
    BOOST_CHECK_EQUAL(written, 97);
    std_buffer = std::string(buffer, 97);
    BOOST_CHECK_EQUAL(std_buffer.compare("CTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 98, 48);
    BOOST_CHECK_EQUAL(written, 98);
    std_buffer = std::string(buffer, 98);
    BOOST_CHECK_EQUAL(std_buffer.compare("CTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

    written = u2b.view_fasta_chunk(4, buffer, 99, 48);
    BOOST_CHECK_EQUAL(written, 99);
    std_buffer = std::string(buffer, 99);
    BOOST_CHECK_EQUAL(std_buffer.compare("CTG\n>chr3.1\nACTG\nACTG\naaaa\nc\n>chr3.2\nACTG\nACTG\naaaa\ncc\n>chr3.3\nACTG\nACTG\naaaa\nccc\n>chr4\nACTG\nnnnn\n>"), 0);
    flush_buffer(buffer, READ_BUFFER_SIZE, '?');

}




BOOST_AUTO_TEST_SUITE_END()
