
/*
2bit variant of:
>chr1
TTTTCCCCAAAAGGGG
>chr2
ACTGACTGNNNNACTG
>chr3.1
ACTGACTGAAAAC
>chr3.2
ACTGACTGAAAACC
>chr3.3
ACTGACTGAAAACCC
>chr4
ACTGNNNN
>chr5
NNACTG 

is:
          [ magic                           ] [ version
00000000: 01000011 00100111 01000001 00011010 00000000 00000000  C'A...

                          ] [ n-seq                           ]
00000006: 00000000 00000000 00000111 00000000 00000000 00000000  ......

          [ reserved: 4 x \0                ] n-name=7 [ c
0000000c: 00000000 00000000 00000000 00000000 00000100 01100011  .....c

          h        r        1      ] [ offset chr1 = 85        
00000012: 01101000 01110010 00110001 01010101 00000000 00000000  hr1U..

                 ] [len=4 ] [ c      h        r        2      ]
00000018: 00000000 00000100 01100011 01101000 01110010 00110010  ..chr2


0000001e: 01101001 00000000 00000000 00000000 00000110 01100011  i....c
00000024: 01101000 01110010 00110011 00101110 00110001 10000101  hr3.1.
0000002a: 00000000 00000000 00000000 00000110 01100011 01101000  ....ch
00000030: 01110010 00110011 00101110 00110010 10011001 00000000  r3.2..
00000036: 00000000 00000000 00000110 01100011 01101000 01110010  ...chr
0000003c: 00110011 00101110 00110011 10101101 00000000 00000000  3.3...
00000042: 00000000 00000100 01100011 01101000 01110010 00110100  ..chr4
00000048: 11000001 00000000 00000000 00000000 00000100 01100011  .....c
0000004e: 01101000 01110010 00110101 11011011 00000000 00000000  hr5...
00000054: 00000000 00010000 00000000 00000000 00000000 00000000  ......
0000005a: 00000000 00000000 00000000 00000000 00000000 00000000  ......
00000060: 00000000 00000000 00000000 00000000 00000000 00000000  ......
00000066: 01010101 10101010 11111111 00010000 00000000 00000000  U.....
0000006c: 00000000 00000001 00000000 00000000 00000000 00001000  ......
00000072: 00000000 00000000 00000000 00000100 00000000 00000000  ......
00000078: 00000000 00000000 00000000 00000000 00000000 00000000  ......
0000007e: 00000000 00000000 00000000 10010011 10010011 00000000  ......
00000084: 10010011 00001101 00000000 00000000 00000000 00000000  ......
0000008a: 00000000 00000000 00000000 00000000 00000000 00000000  ......
00000090: 00000000 00000000 00000000 00000000 00000000 10010011  ......
00000096: 10010011 10101010 01000000 00001110 00000000 00000000  ..@...
0000009c: 00000000 00000000 00000000 00000000 00000000 00000000  ......
000000a2: 00000000 00000000 00000000 00000000 00000000 00000000  ......
000000a8: 00000000 10010011 10010011 10101010 01010000 00001111  ....P.
000000ae: 00000000 00000000 00000000 00000000 00000000 00000000  ......
000000b4: 00000000 00000000 00000000 00000000 00000000 00000000  ......
000000ba: 00000000 00000000 00000000 10010011 10010011 10101010  ......
000000c0: 01010100 00001000 00000000 00000000 00000000 00000001  T.....
000000c6: 00000000 00000000 00000000 00000100 00000000 00000000  ......
000000cc: 00000000 00000100 00000000 00000000 00000000 00000000  ......
000000d2: 00000000 00000000 00000000 00000000 00000000 00000000  ......
000000d8: 00000000 10010011 00000000 00000110 00000000 00000000  ......
000000de: 00000000 00000001 00000000 00000000 00000000 00000000  ......
000000e4: 00000000 00000000 00000000 00000010 00000000 00000000  ......
000000ea: 00000000 00000000 00000000 00000000 00000000 00000000  ......
000000f0: 00000000 00000000 00000000 00001001 00110000           ....0


padding seq1 = 
strlen("chr1chr2chr3.1chr3.2chr3.3chr4chr5") = 34
4+4+4+4 + 34 + (7 * (4+1)) = 85

*/



#define BOOST_TEST_MODULE fastfs_cache

#include <sys/types.h>
#include <sys/stat.h>


#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"



using namespace std::literals; // https://stackoverflow.com/questions/48255276/why-does-stdstring-x00-report-length-of-0


BOOST_AUTO_TEST_SUITE(Testing)

BOOST_AUTO_TEST_CASE(init)
{
    // make tmp dir
    mkdir("tmp", S_IRWXU);
}


/**
 * @description test contains a sequence that intially failed chunked_view with chunk size > 1
 */
BOOST_AUTO_TEST_CASE(test_fastafs_view_chunked_2bit)
{
    //const char UCSC2BIT_HEADER[9] = TWOBIT_MAGIC TWOBIT_VERSION "\0";
    // 1: create FASTAFS file
    std::string fastafs_file = "tmp/test.fastafs";

    fasta_to_fastafs f = fasta_to_fastafs("test", "test/data/test.fa");
    f.cache();
    f.write(fastafs_file);

    fastafs fs = fastafs("test");
    fs.load(fastafs_file);

    BOOST_REQUIRE(fs.data.size() > 0);

    std::ifstream file (fs.filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);    
    BOOST_REQUIRE(file.is_open());
    
    // check ucsc2bit header:
    char buffer[1024 + 1];
    std::string reference = UCSC2BIT_MAGIC + UCSC2BIT_VERSION + "\x07\00\00\00"s "\00\00\00\00"s // literals bypass a char* conversion and preserve nullbytes
                            "\x04"s "chr1"s "\x55\00\00\00"s;

    unsigned int complen;

    // test header
    complen = 8;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std::string(buffer, complen), 0, complen), 0);
    
    // test ... + n-seq
    complen += 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std::string(buffer, complen)), 0);

    // test ... + reserved
    complen += 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std::string(buffer, complen)), 0);

    // test ... + sequence 1 name
    complen += 5;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std::string(buffer, complen)), 0);

    // test ... + sequence 1 offset
    complen += 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std::string(buffer, complen)), 0);




    // debug
    for(unsigned int i =0; i < reference.size(); i++) {
        printf("[%i]  ref:%i\t == buf:%i\n",i, (unsigned int) reference[i], (unsigned int) buffer[i]);
    }

}





BOOST_AUTO_TEST_SUITE_END()
