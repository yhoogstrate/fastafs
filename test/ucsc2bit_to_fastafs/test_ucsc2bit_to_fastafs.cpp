#define BOOST_TEST_MODULE ucsc2bit_to_fastafs

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "utils.hpp"
#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"
//#include "ucsc2bit_to_fastafs.hpp"



BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_ucsc2bit_to_fasta)
{
    std::string fastafs_file = "tmp/test.fastafs";
    std::string ucsc2bit_file = "tmp/test.2bit";

    // 01 fasta_to_fastafs()
    fasta_to_fastafs f = fasta_to_fastafs("test", "test/data/test.fa");
    f.cache();
    f.write(fastafs_file);

    // 02 load fastafs
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    // 03 fastafs::chunked ucsc2bit view of the file
    char buffer[4096 +  1];
    std::fstream ucsc2bit_out_stream(ucsc2bit_file.c_str(), std::ios :: out | std::ios :: binary);
    if(ucsc2bit_out_stream.is_open()) {
        unsigned int written = fs.view_ucsc2bit_chunk(buffer, 4096, 0);
        std::string buffer_s = std_string_nullbyte_safe(buffer, written);
        ucsc2bit_out_stream << buffer_s;
        ucsc2bit_out_stream.close();
    } else {
        throw std::runtime_error("Could not write to file: " + ucsc2bit_file);
    }

    // 04 ucsc2bit_to_fasta
    // comp fasta with initial fasta

    printf("todo");
}




BOOST_AUTO_TEST_SUITE_END()
