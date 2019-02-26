
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

          [ offset = 105 \x69               ]
0000001e: 01101001 00000000 00000000 00000000 00000110 01100011  i....c

                                                       [ offset
00000024: 01101000 01110010 00110011 00101110 00110001 10000101  hr3.1.

          = 133 \x85               ]
0000002a: 00000000 00000000 00000000 00000110 01100011 01101000  ....ch

                                              [ offset = 153   
00000030: 01110010 00110011 00101110 00110010 10011001 00000000  r3.2..

          \x99
00000036: 00000000 00000000 00000110 01100011 01101000 01110010  ...chr

                                    [offset = 173 \xAD
0000003c: 00110011 00101110 00110011 10101101 00000000 00000000  3.3...
00000042: 00000000 00000100 01100011 01101000 01110010 00110100  ..chr4

          [ offset = 193 \xC1               ]
00000048: 11000001 00000000 00000000 00000000 00000100 01100011  .....c

          h        r        5      ] [ offset5 = 219 \xDB
0000004e: 01101000 01110010 00110101 11011011 00000000 00000000  hr5...

                 ] [ dna size = 16                   ] [ n block
00000054: 00000000 00010000 00000000 00000000 00000000 00000000  ......

          = 0                      ] [ m block = 4             
0000005a: 00000000 00000000 00000000 00000000 00000000 00000000  ......

                 ] [ reserved = 4                    ] [ TTTT
00000060: 00000000 00000000 00000000 00000000 00000000 00000000  ......

          CCCC     AAAA     GGGG   ] [ dna size = 16           
00000066: 01010101 10101010 11111111 00010000 00000000 00000000  U.....

*                ] [ n-blocks = 1                    ] [       
0000006c: 00000000 00000001 00000000 00000000 00000000 00001000  ......

          n-block 1 starts at = 8  ]
00000072: 00000000 00000000 00000000 00000100 00000000 00000000  ......
00000078: 00000000 00000000 00000000 00000000 00000000 00000000  ......
0000007e: 00000000 00000000 00000000 10010011 10010011 00000000  ......
00000084: 10010011 00001101 00000000 00000000 00000000 00000000  ......
0000008a: 00000000 00000000 00000000 00000000 00000000 00000000  ......
00000090: 00000000 00000000 00000000 00000000 00000000 10010011  ......
00000096: 10010011 10101010 01000000 00001110 00000000 00000000  ..@...
0000009c: 00000000 00000000 00000000 00000000 00000000 00000000  ......
000000a2: 00000000 00000000 00000000 00000000 00000000 00000000  ......

                                              CC??
000000a8: 00000000 10010011 10010011 10101010 01010000 00001111  ....P.
000000ae: 00000000 00000000 00000000 00000000 00000000 00000000  ......
000000b4: 00000000 00000000 00000000 00000000 00000000 00000000  ......
000000ba: 00000000 00000000 00000000 10010011 10010011 10101010  ......

          CCC?     [ n=8                             ] [nblox=1
000000c0: 01010100 00001000 00000000 00000000 00000000 00000001  T.....

                                   ] [ n blokc starts = 4      
000000c6: 00000000 00000000 00000000 00000100 00000000 00000000  ......

                 ] [ n block len = 4                 ] [ m = 0
000000cc: 00000000 00000100 00000000 00000000 00000000 00000000  ......

                                   ] [ reserved]
000000d2: 00000000 00000000 00000000 00000000 00000000 00000000  ......

                 ] [ ACTG   NNNN   ] [ n-seq=6  
000000d8: 00000000 10010011 00000000 00000110 00000000 00000000  ......

                 ] [ n-blocks = 1                    ] [ first
000000de: 00000000 00000001 00000000 00000000 00000000 00000000  ......

          n black starts at 0      ]
000000e4: 00000000 00000000 00000000 00000010 00000000 00000000  ......
000000ea: 00000000 00000000 00000000 00000000 00000000 00000000  ......
000000f0: 00000000 00000000 00000000 00001001 00110000           ....0


padding seq1 = 
strlen("chr1chr2chr3.1chr3.2chr3.3chr4chr5") = 34
4+4+4+4 + 34 + (7 * (4+1)) = 85

dnasize=4
85 + 4 + 4 + (0*8) + 4 + 4 + 4 = 101

*/



#define BOOST_TEST_MODULE fastfs_cache

#include <sys/types.h>
#include <sys/stat.h>


#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"



std::string std_string_nullbyte_safe(char *ref, size_t pos, size_t len) {
    std::string s = "";
    
    for(size_t i = pos; i < len; i++) {
        s.push_back(ref[i]);
    }
    
    return s;
}


std::string std_string_nullbyte_safe(char *ref, size_t len) {
    std::string s = "";
    
    for(size_t i = 0; i < len; i++) {
        s.push_back(ref[i]);
    }
    
    return s;
}




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
    static std::string reference = UCSC2BIT_MAGIC + UCSC2BIT_VERSION + "\x07\x00\x00\x00"s "\x00\x00\x00\x00"s // literals bypass a char* conversion and preserve nullbytes
                            "\x04"s "chr1"s   "\x55\x00\x00\x00"s
                            "\x04"s "chr2"s   "\x69\x00\x00\x00"s
                            "\x06"s "chr3.1"s "\x85\x00\x00\x00"s
                            "\x06"s "chr3.2"s "\x99\x00\x00\x00"s
                            "\x06"s "chr3.3"s "\xAD\x00\x00\x00"s
                            "\x04"s "chr4"s   "\xC1\x00\x00\x00"s
                            "\x04"s "chr5"s   "\xDB\x00\x00\x00"s
                            "\x10\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x00\x55\xAA\xFF"s // sequence
                            "\x10\x00\x00\x00"s "\01\x00\x00\x00"s "\x08\x00\x00\x00"s "\x04\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x93\x93\x00\x93"s // ACTG ACTG nnnn ACTG = 10010011 10010011 00000000 10010011 = \x93 \x93 \x00 \x93
                            "\x0D\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x93\x93\xAA\x40"s// last one is 01 00 00 00
                            "\x0E\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x93\x93\xAA\x50"s// last one is 01 01 00 00
                            "\x0F\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x93\x93\xAA\x54"s// last one is 01 01 01 00
                            "\x08\x00\x00\x00"s "\x01\x00\x00\x00"s "\x04\x00\x00\x00"s "\x04\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x93\x00" // ACTG NNNN = 10010011 00000000
                            "\x06\x00\x00\x00"s "\x01\x00\x00\x00"s "\x00\x00\x00\x00"s "\x02\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x09\x30" // NNAC TG?? = 00001001 00110000
                            ;
    unsigned int complen;

    // test header
    complen = 8;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen), 0, complen), 0);
    
    // test ... + n-seq
    complen += 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + reserved
    complen += 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 1 name
    complen += 5;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 1 offset
    complen += 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 2 name
    complen += 5;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 2 offset
    complen += 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 3 name
    complen += 7;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 3 offset
    complen += 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 4 name
    complen += 7;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 4 offset
    complen += 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 5 name
    complen += 7;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 5 offset
    complen += 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 6 name
    complen += 5;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 6 offset
    complen += 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 7 name
    complen += 5;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 7 offset
    complen += 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 1 data-block (without sequence)
    complen += 4 + 4 + 4 + 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 1 sequence-data-block
    complen += (16+3)/4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 2 data-block (without sequence) [ n, n-blocks, n-start 1, n-len 1, n-mblock, reserved]
    complen += 4 + 4 + 4 + 4 + 4 + 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 2 sequence-data-block
    complen += (16+3)/4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 3 data-block (without sequence) [ n, n-blocks, n-mblock, reserved]
    complen += 4 + 4 + 4 + 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 3 sequence-data-block
    complen += (13+3)/4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 4 data-block (without sequence) [ n, n-blocks, n-mblock, reserved]
    complen += 4 + 4 + 4 + 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 4 sequence-data-block
    complen += (14+3)/4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 5 data-block (without sequence) [ n, n-blocks, n-mblock, reserved]
    complen += 4 + 4 + 4 + 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 5 sequence-data-block
    complen += (15+3)/4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 6 data-block (without sequence) [ n, n-blocks, n-start 1, n-len 1, n-mblock, reserved]
    complen += 4 + 4 + 4 + 4 + 4 + 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 6 sequence-data-block
    complen += (8+3)/4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 7 data-block (without sequence) [ n, n-blocks, n-start 1, n-len 1, n-mblock, reserved]
    complen += 4 + 4 + 4 + 4 + 4 + 4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // test ... + sequence 7 sequence-data-block
    complen += (6+3)/4;
    fs.view_ucsc2bit_chunk(buffer, complen, 0);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);

    // out of bound check
    unsigned int written = fs.view_ucsc2bit_chunk(buffer, complen + 4, 0);
    BOOST_CHECK_EQUAL(written, complen);
    BOOST_CHECK_EQUAL(reference.compare(0, complen, std_string_nullbyte_safe(buffer, complen)), 0);
    

    // debug toolkit
    /*
    for(unsigned int i = 0; i < reference.size() && i < complen; i++) {
        printf("[%i]  ref:%i ~ %u\t == buf:%i ~ %u",i, (signed char) reference[i], (unsigned char) reference[i], (signed char) buffer[i], (unsigned char) buffer[i]);
        if(reference[i] != buffer[i])
        {
            printf("   ERR/MISMATCH");
        }
        printf("\n");
    }
    */
}


BOOST_AUTO_TEST_CASE(test_fastafs_view_chunked_2bit_with_offset)
{
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
    static std::string reference = UCSC2BIT_MAGIC + UCSC2BIT_VERSION + "\x07\x00\x00\x00"s "\x00\x00\x00\x00"s // literals bypass a char* conversion and preserve nullbytes
                            "\x04"s "chr1"s   "\x55\x00\x00\x00"s
                            "\x04"s "chr2"s   "\x69\x00\x00\x00"s
                            "\x06"s "chr3.1"s "\x85\x00\x00\x00"s
                            "\x06"s "chr3.2"s "\x99\x00\x00\x00"s
                            "\x06"s "chr3.3"s "\xAD\x00\x00\x00"s
                            "\x04"s "chr4"s   "\xC1\x00\x00\x00"s
                            "\x04"s "chr5"s   "\xDB\x00\x00\x00"s
                            "\x10\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x00\x55\xAA\xFF"s // sequence
                            "\x10\x00\x00\x00"s "\01\x00\x00\x00"s "\x08\x00\x00\x00"s "\x04\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x93\x93\x00\x93"s // ACTG ACTG nnnn ACTG = 10010011 10010011 00000000 10010011 = \x93 \x93 \x00 \x93
                            "\x0D\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x93\x93\xAA\x40"s// last one is 01 00 00 00
                            "\x0E\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x93\x93\xAA\x50"s// last one is 01 01 00 00
                            "\x0F\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x93\x93\xAA\x54"s// last one is 01 01 01 00
                            "\x08\x00\x00\x00"s "\x01\x00\x00\x00"s "\x04\x00\x00\x00"s "\x04\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x93\x00" // ACTG NNNN = 10010011 00000000
                            "\x06\x00\x00\x00"s "\x01\x00\x00\x00"s "\x00\x00\x00\x00"s "\x02\x00\x00\x00"s "\x00\x00\x00\x00"s "\x00\x00\x00\x00"s
                                "\x09\x30" // NNAC TG?? = 00001001 00110000
                            ;
    unsigned int file_offset, complen;

    // offset 1,4 byte, starting in the MAGIC HEADER:
    file_offset = 1;
    complen = 3;
    fs.view_ucsc2bit_chunk(buffer, complen, file_offset);
    
    BOOST_CHECK_EQUAL(reference.compare(file_offset, complen, std_string_nullbyte_safe(buffer, 0 , complen), 0, complen), 0);

    //for(unsigned int i = 0; i < complen; i++) {
        //printf("ref[%i]: %u\t == buf[%i]: %u",i + file_offset,  (signed char) reference[i + file_offset], i, (signed char) buffer[i], (unsigned char) buffer[i]);
        //if(reference[i + file_offset] != buffer[i])
        //{
            //printf("   ERR/MISMATCH");
        //}
        //printf("\n");
    //}
    //printf("---\n");


    // offset 3,8 byte, starting in the VERSION HEADER:
    file_offset  = 3;
    complen = 5;
    fs.view_ucsc2bit_chunk(buffer, complen, file_offset);
    
    BOOST_CHECK_EQUAL(reference.compare(file_offset, complen, std_string_nullbyte_safe(buffer, 0 , complen), 0, complen), 0);

    for(unsigned int i = 0; i < complen; i++) {
        printf("ref[%i]: %u\t == buf[%i]: %u",i + file_offset,  (signed char) reference[i + file_offset], i, (signed char) buffer[i], (unsigned char) buffer[i]);
        if(reference[i + file_offset] != buffer[i])
        {
            printf("   ERR/MISMATCH");
        }
        printf("\n");
    }
    printf("---\n");


    // offset 1 byte, starting in the n-seq:
    file_offset += 4;
    complen = 5;
    fs.view_ucsc2bit_chunk(buffer, complen, file_offset);
    
    BOOST_CHECK_EQUAL(reference.compare(file_offset, complen, std_string_nullbyte_safe(buffer, 0 , complen), 0, complen), 0);

    for(unsigned int i = 0; i < complen; i++) {
        printf("ref[%i]: %u\t == buf[%i]: %u",i + file_offset,  (signed char) reference[i + file_offset], i, (signed char) buffer[i], (unsigned char) buffer[i]);
        if(reference[i + file_offset] != buffer[i])
        {
            printf("   ERR/MISMATCH");
        }
        printf("\n");
    }
    printf("---\n");


    // offset 1 byte, starting in the reserved zero:
    file_offset += 4;
    complen = 5;
    fs.view_ucsc2bit_chunk(buffer, complen, file_offset);
    
    BOOST_CHECK_EQUAL(reference.compare(file_offset, complen, std_string_nullbyte_safe(buffer, 0 , complen), 0, complen), 0);

    for(unsigned int i = 0; i < complen; i++) {
        printf("ref[%i]: %u\t == buf[%i]: %u",i + file_offset,  (signed char) reference[i + file_offset], i, (signed char) buffer[i], (unsigned char) buffer[i]);
        if(reference[i + file_offset] != buffer[i])
        {
            printf("   ERR/MISMATCH");
        }
        printf("\n");
    }
    printf("---\n");

}





BOOST_AUTO_TEST_SUITE_END()
