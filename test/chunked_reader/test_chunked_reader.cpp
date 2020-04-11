#define BOOST_TEST_MODULE fastfs_test_chunked_reader


#include <iostream>
#include <fstream>

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"
#include "chunked_reader.hpp"

#include "zstd_seekable_utils.hpp"



void flush_buffer(char *buffer, size_t n, char fill)
{
    for(size_t i = 0; i < n; i++) {
        buffer[i] = fill;
    }
}



BOOST_AUTO_TEST_SUITE(Testing)



BOOST_AUTO_TEST_CASE(test_chunked_reading)
{
    uint32_t written;

    std::string test_name = "test";
    std::string fasta_file = "test/data/" + test_name + ".fa";
    std::string fastafs_file = "tmp/" + test_name + ".fastafs";
    std::string fastafs_file_zstd = "tmp/" + test_name + ".fastafs.zst";

    fasta_to_fastafs(fasta_file, fastafs_file, false);
    ZSTD_seekable_compressFile_orDie((const char*) fastafs_file.c_str(),
                         (const char*) fastafs_file_zstd.c_str(),
                         (int) ZSTD_COMPRESSION_QUALIITY,
                         (unsigned) ZSTD_SEEKABLE_FRAME_SIZE);
    
    
    char buffer[17];
    //chunked_reader r_flat = chunked_reader(fastafs_file.c_str());
    chunked_reader r_flat = chunked_reader("tmp/test.txt");
    r_flat.read(buffer, 2);
    printf("[%s]\n", buffer);
    //chunked_reader r_zstd = chunked_reader(fastafs_file_zstd.c_str());

    
}



BOOST_AUTO_TEST_SUITE_END()

