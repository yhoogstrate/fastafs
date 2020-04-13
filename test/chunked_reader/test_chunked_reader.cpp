
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



BOOST_AUTO_TEST_CASE(test_chunked_reading_small_file)
{
    std::string test_name = "test";
    std::string fasta_file = "test/data/" + test_name + ".fa";
    std::string fastafs_file = "tmp/" + test_name + ".fastafs";
    std::string fastafs_file_zstd = "tmp/" + test_name + ".fastafs.zst";

    fasta_to_fastafs(fasta_file, fastafs_file, false);
    ZSTD_seekable_compressFile_orDie((const char*) fastafs_file.c_str(),
                         (const char*) fastafs_file_zstd.c_str(),
                         (int) ZSTD_COMPRESSION_QUALIITY,
                         (unsigned) ZSTD_SEEKABLE_FRAME_SIZE);
    
    
    char buffer[READ_BUFFER_SIZE + 1];
    std::string std_buffer;
    buffer[1024] = '\0';
    size_t written;

    std::string reference1 = "\x0f\x0a\x46\x53\x00\x00\x00\x00\x80\x00\x00\x00\x01\x37\x00\x00\x00\x10\x00\x55\xaa\xff\x00\x00\x00\x00\x75\x25\x5c\x6d\x90\x77\x89\x99\xad\x36\x43\xa2\xe6\x9d\x43\x44\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x0f\x00\x00\x00\x0c\x93\x93\x93\x00\x00\x00\x01\x00\x00\x00\x08\x00\x00\x00\x0b\x8b\x56\x73\x72\x4a\x99\x65\xc2\x9a\x1d\x76\xfe\x70\x31\xac\x8a\x00\x00\x00\x01\x00\x00\x00\x08\x00\x00\x00\x0b\x00\x00\x00\x0d\x93\x93\xaa\x40\x00\x00\x00\x00\x61\xde\xba\x32\xec\x4c\x35\x76\xe3\x99\x8f\xa2\xd4\xb8\x72\x88\x00\x00\x00\x01\x00\x00\x00\x08\x00\x00\x00\x0c\x00\x00\x00\x0e\x93\x93\xaa\x50\x00\x00\x00\x00\x99\xb9\x05\x60\xf2\x3c\x1b\xda\x28\x71\xa6\xc9\x3f\xd6\xa2\x40\x00\x00\x00\x01\x00\x00\x00\x08\x00\x00\x00\x0d\x00\x00\x00\x0f\x93\x93\xaa\x54\x00\x00\x00\x00\x36\x25\xaf\xdf\xbe\xb4\x37\x65\xb8\x5f\x61\x2e\x0a\xcb\x47\x39\x00\x00\x00\x01\x00\x00\x00\x08\x00\x00\x00\x0e\x00\x00\x00\x04\x93\x00\x00\x00\x01\x00\x00\x00\x04\x00\x00\x00\x07\xbd\x8c\x08\x0e\xd2\x5b\xa8\xa4\x54\xd9\x43\x4c\xb8\xd1\x4a\x68\x00\x00\x00\x01\x00\x00\x00\x04\x00\x00\x00\x07\x00\x00\x00\x04\x93\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x01\x98\x0e\xf3\xa1\xcd\x80\xaf\xec\x95\x9d\xcf\x85\x2d\x02\x62\x46\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x07\x10\x00\x04\x63\x68\x72\x31\x00\x00\x00\x0e\x10\x00\x04\x63\x68\x72\x32\x00\x00\x00\x36\x10\x00\x06\x63\x68\x72\x33\x2e\x31\x00\x00\x00\x65\x10\x00\x06\x63\x68\x72\x33\x2e\x32\x00\x00\x00\x8d\x10\x00\x06\x63\x68\x72\x33\x2e\x33\x00\x00\x00\xb5\x10\x00\x04\x63\x68\x72\x34\x00\x00\x00\xdd\x10\x00\x04\x63\x68\x72\x35\x00\x00\x01\x0a\x00\x1e\x77\x77\x22"s;// xxd -p
    std::string reference2 = "\x0f\x0a\x46\x53"s;
    std::string reference3 = "\x0a\x46\x53\x00"s;

    {
        chunked_reader r_flat = chunked_reader(fastafs_file.c_str());
        //chunked_reader r_flat = chunked_reader("tmp/asd.txt");
        written = r_flat.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 403);
        std_buffer = std::string(buffer, written);
        BOOST_CHECK_EQUAL_MESSAGE(std_buffer.compare(reference1), 0, "Difference in content");

        written = r_flat.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 0);

        // test what happens when file is closed
        written = r_flat.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 0);
        
        r_flat.seek(0); // reset to first pos in file
        written = r_flat.read(buffer, 4);
        BOOST_CHECK_EQUAL(written, 4);
        std_buffer = std::string(buffer, written);
        BOOST_CHECK_EQUAL_MESSAGE(std_buffer.compare(reference2), 0, "Difference in content");
        
        r_flat.seek(1); // reset to first pos in file
        written = r_flat.read(buffer, 4);
        BOOST_CHECK_EQUAL(written, 4);
        std_buffer = std::string(buffer, written);
        BOOST_CHECK_EQUAL_MESSAGE(std_buffer.compare(reference3), 0, "Difference in content");
    }

    {
        chunked_reader r_zstd = chunked_reader(fastafs_file_zstd.c_str());

        written = r_zstd.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 403);
        std_buffer = std::string(buffer, written);
        BOOST_CHECK_EQUAL_MESSAGE(std_buffer.compare(reference1), 0, "Difference in content");
      
        written = r_zstd.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 0);

        // test what happens when file is closed
        written = r_zstd.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 0);
    }
}
/*

BOOST_AUTO_TEST_CASE(test_chunked_reading_large_file)
{
    // this file needs two buffers as its size is 1593

    std::string test_name = "test_007";
    std::string fasta_file = "test/data/" + test_name + ".fa";
    std::string fastafs_file = "tmp/" + test_name + ".fastafs";
    std::string fastafs_file_zstd = "tmp/" + test_name + ".fastafs.zst";

    fasta_to_fastafs(fasta_file, fastafs_file, false);
    ZSTD_seekable_compressFile_orDie((const char*) fastafs_file.c_str(),
                         (const char*) fastafs_file_zstd.c_str(),
                         (int) ZSTD_COMPRESSION_QUALIITY,
                         (unsigned) ZSTD_SEEKABLE_FRAME_SIZE);
    
    
    char buffer[READ_BUFFER_SIZE + 1];
    std::string std_buffer;
    buffer[1024] = '\0';
    size_t written;

    std::string reference1 = "\x0f\x0a\x46\x53\x00\x00\x00\x00\x80\x00\x00\x00\x06\x20\x00\x00\x17\xd7\xf4\xbd\xdd\x5d\x39\xcc\xce\x7e\xe8\x6e\x9d\x92\x70\x2d\x96\x68\x9f\xba\x83\xe1\x99\x2d\x9f\xe4\xed\x65\x3f\x09\x88\x5d\x28\x5c\xc0\x99\x36\x80\x87\xdc\x02\xc0\xe5\x5a\xef\xae\x56\x95\x59\x91\xb6\xde\x35\xf4\x1c\x60\x1e\x30\xd1\x77\x1c\x70\x2d\xda\xed\xc5\xfc\x58\x8a\x28\x94\x2b\x4f\x96\x97\x18\xa0\x65\x22\x48\xa6\x06\x1b\x65\x7f\xf4\x82\x8f\xe3\x05\xde\x00\x70\xb7\xb5\xa4\x1e\xc3\x43\xe9\x49\x92\x8b\x47\xa6\xdd\x97\xd4\x93\x4d\xb4\xd0\x76\xc7\x4d\xeb\x71\x48\x77\x43\x91\xcd\xe5\x8f\x8d\xa2\xcb\x28\x53\xcf\x82\xa4\xd5\x85\x78\xae\x37\xd9\x19\x13\x54\x52\x0c\x7d\xcb\x2a\xfd\x1b\x38\x66\xaa\xd3\x23\xe6\xf7\x20\xd5\x0a\xf1\x4b\x59\xe6\x0b\xbe\x42\xa9\x5e\x7d\xce\xec\x73\xd9\x8b\xc6\x4b\x35\xe4\x69\xbc\x10\x35\x8a\x0e\x09\x2b\xf1\x9f\x38\x15\x57\x21\x08\xe1\xa6\x6e\xf1\x8c\x52\x08\x1b\x85\x50\xe0\x1e\x01\x35\x3a\x0a\x72\x1b\xb3\xda\xfd\x78\x36\x10\xb7\x1a\x2e\x93\xd4\x63\xab\x0b\x98\xfb\x4b\x97\x47\x7f\x61\x0f\x36\x7f\xfe\x02\x36\x2e\x30\xa5\xdb\x8f\xde\xd0\xc0\xc6\x9c\x3a\x7b\x71\x24\x1e\xc3\x04\xac\x31\x7a\xf5\xf3\x33\x26\x99\xa6\x4f\x43\x6c\x46\x5c\x4d\xf5\xb8\x43\x1d\xd7\x73\x3e\xe9\xb6\x3b\xdf\xff\xf6\xf3\x2a\x34\x3f\x39\x60\x4b\xed\xde\xf4\x2f\x5d\xe7\xab\xfe\xa1\x4d\x11\x9c\xcc\x41\xf8\x3c\xdd\x18\xea\xea\x45\x3e\xa5\x0b\xb5\x7b\x38\x5e\x26\x72\xdd\x24\x51\x48\xcf\x79\xa7\xd9\x06\x2e\xe8\xfb\x5d\x3d\x4a\x81\x0d\x15\x48\xd0\x84\x15\x0f\x15\x5b\xc3\x9d\x48\xc4\x9e\x2f\x45\xd4\x1c\x24\xc4\x90\x60\xe6\xa1\x19\x6f\x2d\x3a\xf9\x52\x0d\x06\x93\x21\xb4\xc2\x43\xd7\xce\x5b\xaa\x42\x20\x35\x6c\x45\xa2\xea\xd8\xe0\xc7\x90\xe3\x4a\x3e\xb1\x65\xaf\x5c\xe3\x23\x58\x65\x88\x92\x0b\x98\xc4\x3f\x7b\xb4\x42\x6c\x77\x7f\xf3\x51\x1a\x17\x89\x1f\x03\x66\x95\xbb\x83\x3f\xfb\xd1\x8c\x46\x40\x7f\xd1\xff\x7a\xbb\xb2\xcc\xaa\xc9\xfe\xbe\x7a\xff\x5b\xf7\x17\xe0\x4d\xca\x6b\xf2\xef\x0f\x0c\x48\x90\x5c\x6d\xa4\x53\xf4\xfb\xe3\xfe\x38\x61\xdb\x32\xe6\x6e\x35\x86\xad\xad\x33\x3b\x7f\x92\x7f\xf9\x4c\xbd\x92\xbe\x41\x4f\x23\x37\xa2\x6e\xd9\x7d\x82\x47\xa4\x8f\x77\x51\xdb\x2f\xd6\xda\xcb\x1d\x7b\x2b\xe3\x29\x6f\x03\xad\xce\x05\xa7\xab\x34\x52\xb8\x94\xd3\x08\x5b\x9f\x0d\xec\x27\x09\xce\xb5\x82\x89\x43\xe0\xc3\xc3\x7f\xad\xeb\x30\x0a\x5c\xa8\x88\xc8\x38\x02\x18\x4d\xda\x80\x02\xf5\xb0\x0b\xbf\x3b\xbc\x11\x6b\xe7\xfd\x4b\x4a\xe9\x48\x31\x9f\x3a\x83\x80\x7b\x21\x73\xf8\x99\x43\x1b\xd6\x1a\xb6\xce\xe4\xff\x0e\x58\x33\x86\xd0\x09\x70\x14\x63\xc6\x45\x8f\x2a\x5f\xc8\xb2\x82\xdc\x4f\x99\x81\xa8\x87\xe4\xbf\xc5\xfe\x35\x81\x73\x63\x21\xf1\x82\xdb\x73\xfe\xe2\x1b\x5f\xff\x07\x8b\xb4\xef\xb6\x6a\x92\x9c\xcf\x6d\x09\xb1\xc1\x78\xa4\x56\x37\xe4\x6a\xf9\x01\x1e\x8c\x51\x14\x10\x34\xbd\xb0\x4f\xc6\xcb\xd6\xf4\xee\xed\x7c\x23\xa2\x80\xde\x5d\x76\x9d\x09\xd8\x1d\x45\x21\xc1\xad\xe9\x74\xf2\x61\xd4\x0b\xc7\x0d\x6a\xab\x25\x7c\x19\xa3\xf0\x88\x87\x7b\xba\xf0\x37\x3f\x59\x8f\x7f\x8e\x25\xbb\x80\x70\xf2\xe3\xf5\x0a\xa5\xb5\x2c\x43\x6f\xf1\x7b\xd3\x48\x86\x9a\xa2\xb1\x42\x89\xf3\x00\x0e\x9d\x99\xca\x5e\xb0\x2a\xf7\x46\xe6\xfb\xb9\x22\xc9\x14\xb9\x75\x95\x82\x87\x0d\x9a\x54\x80\xf6\xbc\x1f\xd9\xcb\x09\x0c\x4b\x5e\x38\xa1\x10\xaa\x32\xb1\xfa\xcc\xba\x37\x37\x01\x6d\x7f\xf1\x9d\x49\x35\x6a\x5b\xec\xec\xfb\x6a\x46\xca\x41\x03\x35\xfb\x56\xef\x5b\xe2\x44\xa0\x9e\xf8\x99\xde\x92\x17\x12\x98\x5e\x11\xe0\x73\x94\x23\xc9\x81\x61\xcc\x8a\xb4\x72\x5d\x6e\x1b\xfb\xa4\x3c\x79\x06\x12\xd3\x00\x47\xa7\x8e\x8c\x42\x9d\xa4\xfd\x34\xcd\xf0\x94\xdc\x3c\x84\xe3\xf7\xfc\x16\xd8\x0d\x4a\x9d\x05\xe1\xff\x1b\x47\xf1\xdc\xdf\xa4\x86\x09\xc1\xfe\xde\x45\xe4\x43\xfd\x0d\x05\xf4\x3f\xb5\x2e\xe7\x48\xde\xc8\x2b\x8a\x5f\xee\x28\x66\x09\xb4\x65\x12\x77\x23\x6a\xe2\x80\xa4\xc2\xa5\x1e\xbe\xd9\x8e\xae\x56\x4d\x56\xfe\xed\xe8\x0e\x39\xab\xba\x68\xfd\x39\x2c\x22\x30\x80\x31\xfe\x34\x46\x7d\xea\x3c\x8e\x5b\x87\xef\xac\x2d\xe3\x80\x19\x5a\xd8\xba\x63\xd5\xb4\x59\xc0\x38\xff\xc5\xd8\x00\x75\x8e\x31\x7c\x1f\x90\x98\xdc\x4a\x9c\x67\x84\x12\x87\xb2\x06\xcc\x5c\x41\xc4\xa2\x22\x88\x2d\xf5\x43\xdc\x5f\xe8\x71\xa0\x0f\xbd\xa8\x33\x6f\x83\xbf\xc0\x3a\xfd\xa7\xf9\x8a\x93\x12\x94\x0a\x9e\x39\x68\x60\xc2\xfe\x0a\x2c\x13\xb6\x25\x5a\x85\x62\x1c\x5b"s;// xxd -p
    std::string reference2 = "\x44\x2c\x05\x5b\xe6\x92\x56\x6b\x2f\xf6\x4f\xfb\xdc\x46\x9c\xe2\xbd\xac\xc0\x0d\x53\x44\x4d\x29\xd3\xe3\x61\x06\x77\xfb\x0c\x1b\xfa\x05\x17\x3b\x32\xc8\x6c\xd3\x0e\xa8\x18\xde\x64\xfb\x8a\xb8\x84\xf6\x3f\x17\xc4\x1f\xea\x8c\xea\xd5\x42\xc1\xb3\xdb\x68\x90\x8a\x24\x2f\x0c\xc5\x9b\xb6\xd6\x16\x5d\x3d\x38\xf1\xf6\x80\xf2\x56\x47\xf3\x95\x64\x7e\x50\x14\x02\x73\xa9\x0a\x04\x01\xcc\xf3\x1b\x3c\x9a\xfd\x98\x86\xdf\x54\xe6\x36\x50\xe9\xc0\x46\xd7\xae\x54\xd1\xe4\xaf\x98\xc3\xa6\xee\x44\xce\x8c\x16\xdf\x33\x87\x0b\xca\x12\x91\xac\xa4\xbe\x4e\xdb\xb2\x32\x21\x21\x16\xdb\x0c\x5f\xe3\x33\xbd\xa9\x8a\x88\xed\x3e\x65\x46\x4d\x8b\x16\xf0\x73\xe7\x76\x3d\x42\xb5\xe1\xba\x14\xe8\xd9\x99\x4f\x67\xc2\x20\x0d\x41\x07\x27\x61\x3a\x28\x49\x6f\x73\xdb\x44\xdb\xe2\x5e\x54\x4e\x1c\xe0\xd4\x66\x1e\xfe\x0c\x96\x52\xb3\x79\x00\x9d\x87\xed\xee\xc6\x82\x5e\xdc\x8f\xcd\xc8\xaa\x1c\x44\x76\x22\x14\x99\xef\x56\x73\x0e\x93\x14\x77\xa3\xa4\x52\xa7\xad\x55\x6c\xe2\x1a\x6a\x57\xd1\xb8\x4a\x8f\x3a\xa9\xcf\xab\x20\x25\xc8\xa8\x13\x30\x3c\x78\xbd\x3e\x9d\x73\x8f\xd9\x10\x9c\x15\xa8\x8a\x58\x70\x34\x38\xbb\xff\x26\x6d\x42\xcd\x2f\x8f\x7c\x20\x39\xa5\x37\x70\xf1\x1f\x65\x8a\xc5\xa3\x4f\x02\x57\x35\x17\x1b\x91\xa2\xa6\xd4\x67\x1c\x54\xde\xb4\xaf\x53\x99\x92\x23\xc1\x3d\xcc\x62\x9c\x21\xd9\xb5\xde\x5f\xd6\x1e\xa5\x4a\x45\x7e\x10\x74\xc4\x9e\x7f\x3b\xdd\xf6\x6c\xb6\xf2\xc9\xb6\xbe\x01\x45\x2e\x4a\x3b\xaf\x41\x05\x91\x38\x68\x35\x36\x0e\x1a\xc7\xc9\x52\x6d\xc1\x9c\x9e\x50\x29\x7b\x3e\xe0\x39\x67\x32\xe8\xae\xaa\xac\x0c\xbb\x18\x4c\x11\x3b\x58\xc8\x80\x88\xf1\x6d\x7a\x3d\x36\xd0\x8e\xc1\xb1\xf8\xbb\xa9\xd1\xd6\x8f\x07\x6b\x12\x1a\x5b\xf1\xea\xed\x94\x1b\xe1\x1b\xe7\x0e\x75\x3d\x4e\xcf\x5b\x91\x2e\x78\x55\xd8\x8d\x1f\x1b\x09\x60\x38\xd2\xb8\xaa\x1f\xb5\x9d\x2c\xd4\x5c\x44\x78\x1f\x88\x4c\xaf\xa6\x2c\xeb\xca\x00\x51\xbe\xc9\x2e\x60\xaf\x0d\xb4\x02\xb3\x47\x0a\x3f\x4b\xbc\xc4\xa4\xff\xbb\xb3\x0e\x4f\xb3\xf0\x71\x3a\x84\x9a\x3d\x36\x33\x25\xeb\x2f\x76\x66\x5e\xc3\xd0\x66\xfc\xd4\x10\x3b\x78\x15\x61\x2d\xfc\xe6\x05\x7e\xda\x86\x43\x15\xb9\x78\xc2\x8b\x98\x42\x3e\x56\x42\x69\xba\xa2\xf3\x1e\xec\x00\x00\x00\x00\x21\x83\x67\xa8\x14\xed\xdc\x51\xeb\x96\x93\x98\x74\x4d\x13\x7c\x00\x00\x00\x00\x00\x00\x00\x01\x10\x00\x09\x6c\x65\x6e\x2d\x6c\x69\x6d\x69\x74\x00\x00\x00\x0e\x00\x98\x32\x91\x09"s;// xxd -p

    {
        chunked_reader r_flat = chunked_reader(fastafs_file.c_str());

        written = r_flat.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 1024);
        std_buffer = std::string(buffer, written);
        BOOST_CHECK_EQUAL_MESSAGE(std_buffer.compare(reference1), 0, "Difference in content 1st read");

        written = r_flat.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 569);
        std_buffer = std::string(buffer, written);
        BOOST_CHECK_EQUAL_MESSAGE(std_buffer.compare(reference2), 0, "Difference in content 2nd read");

        written = r_flat.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 0);

        written = r_flat.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 0);
    }

    {
        chunked_reader r_zstd = chunked_reader(fastafs_file_zstd.c_str());

        written = r_zstd.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 1024);
        std_buffer = std::string(buffer, written);
        BOOST_CHECK_EQUAL_MESSAGE(std_buffer.compare(reference1), 0, "Difference in content 1st read");

        written = r_zstd.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 569);
        std_buffer = std::string(buffer, written);
        BOOST_CHECK_EQUAL_MESSAGE(std_buffer.compare(reference2), 0, "Difference in content 2nd read");

        written = r_zstd.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 0);

        written = r_zstd.read(buffer, 1024);
        BOOST_CHECK_EQUAL(written, 0);
    }
}
*/



BOOST_AUTO_TEST_SUITE_END()

