#define BOOST_TEST_MODULE fastfs_cache

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

//#include "twobit_byte.hpp"
#include "fasta_to_fastafs.hpp"



BOOST_AUTO_TEST_SUITE(Testing)

/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(test_equality_twobit_byte)
{
    twobit_byte b = twobit_byte();
    char *seq1;
    char *seq2;
    char *seq3;
    char *seq4;
    const char *seq;// don't dereference, pointer to static two_bit property

    // test 00 00 00 00 -> 00000000 -> 0
    b.set(6, NUCLEOTIDE_T);
    b.set(4, NUCLEOTIDE_T);
    b.set(2, NUCLEOTIDE_T);
    b.set(0, NUCLEOTIDE_T);
    BOOST_CHECK_EQUAL(b.data, 0);
    seq1 = b.get(1);
    seq2 = b.get(2);
    seq3 = b.get(3);
    seq4 = b.get(4);
    seq = b.get();
    BOOST_CHECK_EQUAL(strcmp(seq1, "T"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "TT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq3, "TTT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq4, "TTTT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq, "TTTT"), 0);
    delete[] seq1;
    delete[] seq2;
    delete[] seq3;
    delete[] seq4;


    // test 00 00 11 00 -> 00001100 -> 8+4 -> 12
    b.set(6, NUCLEOTIDE_T);
    b.set(4, NUCLEOTIDE_T);
    b.set(2, NUCLEOTIDE_G);
    b.set(0, NUCLEOTIDE_T);
    BOOST_CHECK_EQUAL(b.data, 12);
    seq1 = b.get(1);
    seq2 = b.get(2);
    seq3 = b.get(3);
    seq4 = b.get(4);
    seq = b.get();
    BOOST_CHECK_EQUAL(strcmp(seq1, "T"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "TT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq3, "TTG"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq4, "TTGT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq, "TTGT"), 0);
    delete[] seq1;
    delete[] seq2;
    delete[] seq3;
    delete[] seq4;


    // test 00 11 00 00 -> 00110000 -> 16+32 -> 48
    b.set(6, NUCLEOTIDE_T);
    b.set(4, NUCLEOTIDE_G);
    b.set(2, NUCLEOTIDE_T);
    b.set(0, NUCLEOTIDE_T);
    BOOST_CHECK_EQUAL(b.data, 48);
    seq1 = b.get(1);
    seq2 = b.get(2);
    seq3 = b.get(3);
    seq4 = b.get(4);
    seq = b.get();
    BOOST_CHECK_EQUAL(strcmp(seq1, "T"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "TG"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq3, "TGT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq4, "TGTT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq, "TGTT"), 0);
    delete[] seq1;
    delete[] seq2;
    delete[] seq3;
    delete[] seq4;


    // test 11 00 00 00 -> 11000000 -> 64+128 -> 192
    b.set(6, NUCLEOTIDE_G);
    b.set(4, NUCLEOTIDE_T);
    b.set(2, NUCLEOTIDE_T);
    b.set(0, NUCLEOTIDE_T);
    BOOST_CHECK_EQUAL(b.data, 192);
    seq1 = b.get(1);
    seq2 = b.get(2);
    seq3 = b.get(3);
    seq4 = b.get(4);
    seq = b.get();
    BOOST_CHECK_EQUAL(strcmp(seq1, "G"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "GT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq3, "GTT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq4, "GTTT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq, "GTTT"), 0);
    delete[] seq1;
    delete[] seq2;
    delete[] seq3;
    delete[] seq4;


    // test 10 01 01 10 -> 10010110 -> 2 + 4 + 16 + 128 -> 150
    b.set(6, NUCLEOTIDE_A);
    b.set(4, NUCLEOTIDE_C);
    b.set(2, NUCLEOTIDE_C);
    b.set(0, NUCLEOTIDE_A);
    BOOST_CHECK_EQUAL(b.data, 150);
    seq1 = b.get(1);
    seq2 = b.get(2);
    seq3 = b.get(3);
    seq4 = b.get(4);
    seq = b.get();
    BOOST_CHECK_EQUAL(strcmp(seq1, "A"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "AC"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq3, "ACC"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq4, "ACCA"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq, "ACCA"), 0);
    delete[] seq1;
    delete[] seq2;
    delete[] seq3;
    delete[] seq4;



    // test 11 11 11 11 -> 11111111 -> 255
    b.set(6, NUCLEOTIDE_G);
    b.set(4, NUCLEOTIDE_G);
    b.set(2, NUCLEOTIDE_G);
    b.set(0, NUCLEOTIDE_G);
    BOOST_CHECK_EQUAL(b.data, 255);
    seq1 = b.get(1);
    seq2 = b.get(2);
    seq3 = b.get(3);
    seq4 = b.get(4);
    seq = b.get();
    BOOST_CHECK_EQUAL(strcmp(seq1, "G"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "GG"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq3, "GGG"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq4, "GGGG"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq, "GGGG"), 0);
    delete[] seq1;
    delete[] seq2;
    delete[] seq3;
    delete[] seq4;

    // test 00 00 00 00 -> 00000000 -> 0
    b.set(6, NUCLEOTIDE_T);
    b.set(4, NUCLEOTIDE_T);
    b.set(2, NUCLEOTIDE_T);
    b.set(0, NUCLEOTIDE_T);
    BOOST_CHECK_EQUAL(b.data, 0);
    seq1 = b.get(1);
    seq2 = b.get(2);
    seq3 = b.get(3);
    seq4 = b.get(4);
    seq = b.get();
    BOOST_CHECK_EQUAL(strcmp(seq1, "T"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "TT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq3, "TTT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq4, "TTTT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq, "TTTT"), 0);
    delete[] seq1;
    delete[] seq2;
    delete[] seq3;
    delete[] seq4;
}



/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(Test_size)
{
    twobit_byte b = twobit_byte();

    BOOST_CHECK_EQUAL(sizeof(b.data), 1);
}



/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(test_cache)
{
    fasta_to_fastafs f = fasta_to_fastafs("test", "test/data/test.fa");
    f.cache();
    f.write("tmp/test_cachce_test.fastafs");
    
    static std::string reference = 
        // GENERIC-HEADER
        FASTAFS_MAGIC + //        [0, 3] 
        FASTAFS_VERSION + //      [4, 7] 
        "\x00\x00\x00\x01"s//     [8, 11] still version 0.0.0.1
        "\x00\x00\x01\x1D"s //    [12, 15] index position in file (285)
        
        // DATA
        "\x00\x00\x00\x10"s//     [12+4, 12+7] seq length (16) (of 2bit encoded bytes; n-blocks are excluded)
        "\x00\x55\xAA\xFF"s//     [12+8, 12+11] sequence
        "\x00\x00\x00\x00"s//     [12+12, 12+15] n-blocks (0)
        "????????????????????"s// [12+16, 12+35] checksum
        "\x00\x00\x00\x00"s//     [12+36, 12+39] m-blocks (0)
        
        "\x00\x00\x00\x0C"s//     [12+40, 12+43] seq length (12) (of 2bit encoded bytes; n-blocks are excluded)
        "\x93\x93\x93"s//         [12+44, 12+46] sequence: ACTG ACTG nnnn ACTG = 10010011 10010011 00000000 10010011 = \x93 \x93 \x00 \x93
        "\x00\x00\x00\x01"s//     [12+47, 12+50] n-blocks (1)
        "\x00\x00\x00\x08"s//     [12+51, 12+54] n-block start[1] (08)
        "\x00\x00\x00\x0B"s//     [12+55, 12+58] n-block ends[1] (11)
        "????????????????????"s// [12+59, 12+78] checksum
        "\x00\x00\x00\x00"s//     [12+79, 12+82] m-blocks (0)
        
        "\x00\x00\x00\x0D"s//     [12+83, 12+86] seq length (13) (needs to become 2bit-encoded seq-len)
        "\x93\x93\xAA\x40"s//     [12+87, 12+90] sequence: last one is 01 00 00 00
        "\x00\x00\x00\x00"s//     [12+91, 12+94] n-blocks (0)
        "????????????????????"s// [12+95, 12+114] checksum
        "\x00\x00\x00\x00"s//     [12+115, 12+118] m-blocks (0)
        
        "\x00\x00\x00\x0E"s// [12+119, 12+122] seq length (14) (of 2bit encoded bytes; n-blocks are excluded)
        "\x93\x93\xAA\x50"s// [12+123, 12+126] last one is 01 01 00 00
        "\x00\x00\x00\x00"s// [12+127, 12+130] n-bocks (0)
        "????????????????????"s// [12+131, 12+150] checksum
        "\x00\x00\x00\x00"s// [12+151, 12+154] m-blocks (0)
        
        "\x00\x00\x00\x0F"s// [12+155, 12+158] seq length (15) (of 2bit encoded nucleotides; n-blocks are excluded)
        "\x93\x93\xAA\x54"s// [12+159, 12+162] last one is 01 01 01 00
        "\x00\x00\x00\x00"s// [12+163, 12+166] n-blocks (0)
        "????????????????????"s// [12+167, 12+186] checksum
        "\x00\x00\x00\x00"s// [12+187, 12+190] m-blocks (0)
        
        "\x00\x00\x00\x04"s// [12+191, 12+194] seq length (4) (of 2bit encoded nucleotides; n-blocks are excluded)
        "\x93"s//               [12+195, 12+195] sequence: ACTG NNNN = 10010011 00000000
        "\x00\x00\x00\x01"s// [12+196, 12+199] n-blocks (1)
        "\x00\x00\x00\x04"s// [12+200, 12+203] n-starts [1] (4)
        "\x00\x00\x00\x07"s// [12+204, 12+207] n-ends [1] (7)
        "????????????????????"s// [12+208, 12+227] checksum
        "\x00\x00\x00\x00"s//   [12+228, 12+231] m-blocks (0)
        
        "\x00\x00\x00\x04"s// [12+232, 12+235] seq length (4) (of 2bit encoded nucleotides; n-blocks are excluded)
        "\x09"s// [12+236, 12+236] sequence: NNAC TG?? = 00001001 00110000
        "\x00\x00\x00\x01"s// [12+237, 12+240] n-blocks (1)
        "\x00\x00\x00\x00"s// [12+241, 12+244] n-starts[1] (0)
        "\x00\x00\x00\x01"s// [12+245, 12+248] n-ends[1] (1)
        "????????????????????"s// [12+249, 12+268] checksum
        "\x00\x00\x00\x00"s// [12+269, 12+272] m-blocks (0)

    // INDEX
        "\x00\x00\x00\x07"s     // [12+273=285, 12+276] 7 sequences
        
        "\x08"                  // [, ] complete, DNA and not circular
        "\x04"s "chr1"s         // [, ] name
        "\x55\x00\x00\x00"s     // [, ] data position in file (12+4)
        
        "\x08"                  // [, ] complete, DNA and not circular
        "\x04"s "chr2"s         // [, ] name
        "\x69\x00\x00\x00"s     // [, ] data position in file (12+40)
        
        "\x08"                  // [, ] complete, DNA and not circular
        "\x06"s "chr3.1"s       // [, ] name
        "\x85\x00\x00\x00"s     // [, ] data position in file (12+83)
        
        "\x08"                  // [, ] complete, DNA and not circular
        "\x06"s "chr3.2"s       // [, ] name
        "\x99\x00\x00\x00"s     // [, ] data position in file (12+119)
        
        "\x08"                  // [, ] complete, DNA and not circular
        "\x06"s "chr3.3"s       // [, ] name
        "\xAD\x00\x00\x00"s     // [, ] data position in file (12+155)
        
        "\x08"                  // [, ] complete, DNA and not circular
        "\x04"s "chr4"s         // [, ] name
        "\xC1\x00\x00\x00"s     // [, ] data position in file (12+191)
        
        "\x08"                  // [, ] complete, DNA and not circular
        "\x04"s "chr5"s         // [, ] name
        "\xDB\x00\x00\x00"s     // [, ] data position in file (12+232)
    
    // METADATA
        "\x00"                  // no metadata fields [padding will come soon?]
    ;
    //BOOST_CHECK(output.compare(uppercase) == 0 or output.compare(mixedcase) == 0);
}





/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(test_cache_forwards_backwards)
{
    fasta_to_fastafs f = fasta_to_fastafs("test", "test/data/test.fa");
    f.cache();
    f.write("tmp/test_cachce_test.fastafs");
    
    fastafs f2 = fastafs("test");
    f2.load("tmp/test_cachce_test.fastafs");

    const uint32_t padding = 60;
    const uint32_t write_size = 32;
    char buffer[write_size + 1] = "";
    buffer[32] = '\0';

    uint32_t written = 0;
    uint32_t w = 0;
    std::string output = "";
    while(written < f2.fasta_filesize(padding)) {
        w = f2.view_fasta_chunk(padding, buffer, write_size, written);
        output.append(buffer, w);
        written += w;
    }

    std::string uppercase = ">chr1\nTTTTCCCCAAAAGGGG\n>chr2\nACTGACTGNNNNACTG\n>chr3.1\nACTGACTGAAAAC\n>chr3.2\nACTGACTGAAAACC\n>chr3.3\nACTGACTGAAAACCC\n>chr4\nACTGNNNN\n>chr5\nNNACTG\n";
    std::string mixedcase = ">chr1\nttttccccaaaagggg\n>chr2\nACTGACTGnnnnACTG\n>chr3.1\nACTGACTGaaaac\n>chr3.2\nACTGACTGaaaacc\n>chr3.3\nACTGACTGaaaaccc\n>chr4\nACTGnnnn\n>chr5\nnnACTG\n";

    // check case insensitive; without masking included
    BOOST_CHECK(output.compare(uppercase) == 0 or output.compare(mixedcase) == 0);
}





BOOST_AUTO_TEST_SUITE_END()
