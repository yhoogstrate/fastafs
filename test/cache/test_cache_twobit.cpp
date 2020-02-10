#define BOOST_TEST_MODULE fastfs_cache_twobit

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_twobit_fastafs.hpp"
//#include "fastafs.hpp"



BOOST_AUTO_TEST_SUITE(Testing)

/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(test_equality_twobit_byte)
{
    twobit_byte b = twobit_byte(ENCODE_HASH_TWOBIT_DNA);

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
 * @test tests whether 2bit/4bit data are indeed 1 byte in size
 */
BOOST_AUTO_TEST_CASE(Test_size)
{
    {
        twobit_byte b = twobit_byte(ENCODE_HASH_TWOBIT_DNA);
        BOOST_CHECK_EQUAL(sizeof(b.data), 1);
    }

    {
        fourbit_byte b = fourbit_byte();
        BOOST_CHECK_EQUAL(sizeof(b.data), 1);
    }
}



/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(test_cache)
{
    size_t written = fasta_to_twobit_fastafs("test/data/test.fa", "tmp/test_cache_test.fastafs");

    static std::string reference =
        // GENERIC-HEADER
        "\x0F\x0A\x46\x53"s//     [0, 3]
        "\x00\x00\x00\x00"s//     [4, 7] version
        "\x80\x00"s//             [8, 9] FASTAFS flag [ 10000000 | 00000000 ]
        "\x00\x00\x01\x37"s //    [10, 13] index position in file (153)

        // DATA
        "\x00\x00\x00\x10"s//     [14, 17] seq length (16) (of 2bit encoded bytes; n-blocks are excluded)
        "\x00\x55\xAA\xFF"s//     [18, 21] sequence
        "\x00\x00\x00\x00"s//     [22, 25] n-blocks (0)
        "\x75\x25\x5C\x6D\x90\x77\x89\x99\xAD\x36\x43\xA2\xE6\x9D\x43\x44"s// [26, 41] checksum
        "\x00\x00\x00\x01"s//     [42, ] m-blocks (1)
        "\x00\x00\x00\x00"s//     [50, 53] m-block starts (0)
        "\x00\x00\x00\x0F"s//     [54, 57] m-block starts (15)
        "\x00\x00\x00\x0C"s//     [58, 61] seq length (12) (of 2bit encoded bytes; n-blocks are excluded)
        "\x93\x93\x93"s//         [62, 64] sequence: ACTG ACTG nnnn ACTG = 10010011 10010011 00000000 10010011 = \x93 \x93 \x00 \x93
        "\x00\x00\x00\x01"s//     [65, 68] n-blocks (1)
        "\x00\x00\x00\x08"s//     [69, 72] n-block start[1] (08)
        "\x00\x00\x00\x0B"s//     [73, 76] n-block ends[1] (11)
        "\x8B\x56\x73\x72\x4A\x99\x65\xC2\x9A\x1D\x76\xFE\x70\x31\xAC\x8A"s// [69, 96] checksum
        "\x00\x00\x00\x01"s//     [97, 100] m-blocks (0)
        "\x00\x00\x00\x08"s//     [101, 104] m-block starts (8)
        "\x00\x00\x00\x0B"s//     [105, 108] m-block starts (11)
        "\x00\x00\x00\x0D"s//     [109, 112] seq length (13) (needs to become 2bit-encoded seq-len)
        "\x93\x93\xAA\x40"s//     [113, 116] sequence: last one is 01 00 00 00
        "\x00\x00\x00\x00"s//     [117, 120] n-blocks (0)
        "\x61\xDE\xBA\x32\xEC\x4C\x35\x76\xE3\x99\x8F\xA2\xD4\xB8\x72\x88"s// [121, 140] checksum
        "\x00\x00\x00\x01"s//     [141, 144] m-blocks (0)
        "\x00\x00\x00\x08"s//     [145, 148] m-block starts (8)
        "\x00\x00\x00\x0C"s//     [149, 152] m-block starts (12)
        "\x00\x00\x00\x0E"s//     [153, 156] seq length (14) (of 2bit encoded bytes; n-blocks are excluded)
        "\x93\x93\xAA\x50"s//     [157, 160] last one is 01 01 00 00
        "\x00\x00\x00\x00"s//     [161, 164] n-bocks (0)
        "\x99\xB9\x05\x60\xF2\x3C\x1B\xDA\x28\x71\xA6\xC9\x3F\xD6\xA2\x40"s// [165, 184] checksum
        "\x00\x00\x00\x01"s//     [185, 188] m-blocks (0)
        "\x00\x00\x00\x08"s//     [189, 192] m-block starts (8)
        "\x00\x00\x00\x0D"s//     [193, 196] m-block starts (13)
        "\x00\x00\x00\x0F"s//     [197, 200] seq length (15) (of 2bit encoded nucleotides; n-blocks are excluded)
        "\x93\x93\xAA\x54"s//     [201, 204] last one is 01 01 01 00
        "\x00\x00\x00\x00"s//     [205, 208] n-blocks (0)
        "\x36\x25\xAF\xDF\xBE\xB4\x37\x65\xB8\x5F\x61\x2E\x0A\xCB\x47\x39"s// [209, 228] checksum
        "\x00\x00\x00\x01"s//     [229, 232] m-blocks (0)
        "\x00\x00\x00\x08"s//     [233, 236] m-block starts (8)
        "\x00\x00\x00\x0E"s//     [237, 240] m-block starts (14)
        "\x00\x00\x00\x04"s//     [241, 244] seq length (4) (of 2bit encoded nucleotides; n-blocks are excluded)
        "\x93"s//                 [245, 245] sequence: ACTG NNNN = 10010011 00000000
        "\x00\x00\x00\x01"s//     [246, 249] n-blocks (1)
        "\x00\x00\x00\x04"s//     [250, 253] n-starts [1] (4)
        "\x00\x00\x00\x07"s//     [254, 257] n-ends [1] (7)
        "\xBD\x8C\x08\x0E\xD2\x5B\xA8\xA4\x54\xD9\x43\x4C\xB8\xD1\x4A\x68"s// [258, 277] checksum
        "\x00\x00\x00\x01"s//     [278, 281] m-blocks (0)
        "\x00\x00\x00\x04"s//     [282, 285] m-block starts (4)
        "\x00\x00\x00\x07"s//     [286, 289] m-block starts (7)
        "\x00\x00\x00\x04"s//     [290, 293] seq length (4) (of 2bit encoded nucleotides; n-blocks are excluded)
        "\x93"s//                 [294, 294] sequence: NNAC TG?? = 00001001 00110000
        "\x00\x00\x00\x01"s//     [295, 298] n-blocks (1)
        "\x00\x00\x00\x00"s//     [299, 302] n-starts[1] (0)
        "\x00\x00\x00\x01"s//     [303, 306] n-ends[1] (1)
        "\x98\x0E\xF3\xA1\xCD\x80\xAF\xEC\x95\x9D\xCF\x85\x2D\x02\x62\x46"s// [307, 326] checksum
        "\x00\x00\x00\x01"s//     [327, 330] m-blocks (0)
        "\x00\x00\x00\x00"s//     [331, 334] m-block starts (0)
        "\x00\x00\x00\x01"s//     [335, 338] m-block starts (1)

        // INDEX
        "\x00\x00\x00\x07"s     // [339, 342] 7 sequences
        "\x010\x00"             // [343, 344] complete, DNA and not circular
        "\x04"s "chr1"s         // [345, 349] name
        "\x00\x00\x00\x0E"s     // [350, 353] data position in file (14)
        "\x010\x00"             // [354, 355] complete, DNA and not circular
        "\x04"s "chr2"s         // [356, 360] name
        "\x00\x00\x00\x36"s     // [361, 364] data position in file (54)
        "\x010\x00"             // [, ] complete, DNA and not circular
        "\x06"s "chr3.1"s       // [, ] name
        "\x00\x00\x00\x65"s     // [, ] data position in file (101)
        "\x010\x00"             // [, ] complete, DNA and not circular
        "\x06"s "chr3.2"s       // [, ] name
        "\x00\x00\x00\x8D"s     // [, ] data position in file (141)
        "\x010\x00"             // [, ] complete, DNA and not circular
        "\x06"s "chr3.3"s       // [, ] name
        "\x00\x00\x00\xB5"s     // [, ] data position in file (181)
        "\x010\x00"             // [, ] complete, DNA and not circular
        "\x04"s "chr4"s         // [, ] name
        "\x00\x00\x00\xDD"s     // [, ] data position in file (221)
        "\x010\x00"             // [, ] complete, DNA and not circular
        "\x04"s "chr5"s         // [, ] name
        "\x00\x00\x01\x0A"s     // [, ] data position in file (290)

        // METADATA
        "\x00"s                 // [399] no metadata fields [padding will come soon?]

        // CRC32 checksums
        "\x1e\x77\x77\x22"s
        ;

    BOOST_CHECK_EQUAL(written, 403);

    //BOOST_CHECK(output.compare(uppercase) == 0 or output.compare(mixedcase) == 0);
    std::ifstream file("tmp/test_cache_test.fastafs", std::ios::in | std::ios::binary | std::ios::ate);
    BOOST_REQUIRE(file.is_open());

    std::streampos size;
    char * buffer;
    size = file.tellg();
    buffer = new char [size];

    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();
    for(unsigned int i = 0; i < size; i++) {
        BOOST_CHECK_EQUAL(buffer[i], reference[i]);

        /*
                printf("comparing char %i\n", i);
                if(reference[i] != buffer[i]) {
                    printf(" ** mismatch [%d] [ref] %d != [buf] %d (%c x %02hhX)\n", i, reference[i], buffer[i], buffer[i], buffer[i]);
                }
        */

    }

    delete[] buffer;


    // check computed file size
    fastafs f = fastafs("");
    f.load("tmp/test_cache_test.fastafs");
    BOOST_CHECK_EQUAL(f.fastafs_filesize(), 403);
}




/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(test_cache_forwards_backwards)
{
    // generate FASTAFS file from FASTA file
    fasta_to_twobit_fastafs("test/data/test.fa", "tmp/test_cache_test.fastafs");

    // load the FASTAFS file
    fastafs f2 = fastafs("test");
    f2.load("tmp/test_cache_test.fastafs");

    const uint32_t padding = 60;
    ffs2f_init* cache_p60_uc = f2.init_ffs2f(padding, false); // upper case
    ffs2f_init* cache_p60_mc = f2.init_ffs2f(padding, true); // mixed case

    // upper case test
    const uint32_t write_size = 32;
    char buffer[write_size + 1] = "";
    buffer[32] = '\0';
    uint32_t written = 0;
    uint32_t w = 0;
    std::string output = "";

    while(written < f2.fasta_filesize(padding)) {
        w = f2.view_fasta_chunk(cache_p60_uc, buffer, write_size, written);
        output.append(buffer, w);
        written += w;
    }

    std::string uppercase = ">chr1\nTTTTCCCCAAAAGGGG\n>chr2\nACTGACTGNNNNACTG\n>chr3.1\nACTGACTGAAAAC\n>chr3.2\nACTGACTGAAAACC\n>chr3.3\nACTGACTGAAAACCC\n>chr4\nACTGNNNN\n>chr5\nNNACTG\n";
    BOOST_CHECK(output.compare(uppercase) == 0);

    // mixed case test
    buffer[32] = '\0';
    written = 0;
    w = 0;
    output = "";

    while(written < f2.fasta_filesize(padding)) {
        w = f2.view_fasta_chunk(cache_p60_mc, buffer, write_size, written);
        output.append(buffer, w);
        written += w;
    }
    std::string mixedcase = ">chr1\nttttccccaaaagggg\n>chr2\nACTGACTGnnnnACTG\n>chr3.1\nACTGACTGaaaac\n>chr3.2\nACTGACTGaaaacc\n>chr3.3\nACTGACTGaaaaccc\n>chr4\nACTGnnnn\n>chr5\nnnACTG\n";

    // check case insensitive; without masking included
    BOOST_CHECK(output.compare(mixedcase) == 0);

    delete cache_p60_uc;
    delete cache_p60_mc;
}






/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(test_cache_with_newlines)
{
    // generate FASTAFS file from FASTA file
    fasta_to_twobit_fastafs("test/data/test_003.fa", "tmp/test_cache_test_003.fastafs");

    // load the FASTAFS file
    fastafs f2 = fastafs("test");
    f2.load("tmp/test_cache_test_003.fastafs");

    const uint32_t padding = 60;
    ffs2f_init* cache_p60 = f2.init_ffs2f(padding, false);

    const uint32_t write_size = 32;
    char buffer[write_size + 1] = "";
    buffer[32] = '\0';
    uint32_t written = 0;
    uint32_t w = 0;
    std::string output = "";

    while(written < f2.fasta_filesize(padding)) {
        w = f2.view_fasta_chunk(cache_p60, buffer, write_size, written);
        output.append(buffer, w);
        written += w;
    }

    std::string uppercase = ">seq1\nTCCCTTGCNNAGGGNTGTNTTTNTNTANNNGTNGGAGACCNGANTAAAGTCACACCACAT\nCACGNNNTCAGCCTNAAACGGCGNATTAAGTGATCCTANCAACGATCTATCCGNANACAC\nAAATANGTTGACCTCGCGTCNNAGGCACCCGNNANAAGNANGCGCCGCACGGGNNGGNGN\nTACTAACGTCNNNCGCATGTNAGATGGACCNGTNNGCNANTTACCGNNCATACNCCNNNC\nCNAGAATTTCGTATNANAACNCAATTANGGTGGTCCGNNNGGTGNCAACCCTACCTTANA\nTNGGAACATATTNTAGGNCNCGNAATAAANGAGAGTAGNCCCANCTCANGANNACNNGGA\nCTCGCAGCCCNTTNAGCANNCAGCGGCGCNNTGGNTNGTTCTCANTNNNTNCAGGCGTNC\nGAAACTCGTGANNANAGACNGGTTGCNNCGNNANGNTNAGCCTCNTCGNAGCAGTTNTTA\nCGGTNCAACTNTAAGCCAGGTCANCAGTCNNN\n>seq2\nNNAATCATNCGAGNCTATNATANNAGNACNCTGTAGACCNNATACTNNATCAANNNTCTG\nTCANTNCNCAGNCNGGNGNNTTGNTNNATNGANGACTACGGTGCAGTTCGGTTGTGCGCC\nNCNNGATCTNTNNTACCGCGANCCCGGTNGNTGCTNCGATANTCANAACAGCAGAGANNG\nTTGNACAAATTTCTAGGNAACTTNCCCCCTACAANAGNNCGTACNNCNGTCTNAGTCACT\nANGCNCTGCNAAANGCGGCGATTCTTACNNGTNTCCGCGGGNTNNGTTCTCCACACTANA\nNCCNTATCNGACTANTATCATTNNCAANGAGTTAAGNCCAATNTACGCAAGTNAATTNNG\nAACGCCNCNACACCNTNGGANAGCNTGTTNTCACGNGTGACGCNNTGNATTNTATGCTTN\nAGAGTTANGCGGNCNGCGTCTGTGNTCGNGGGNCNTATAGGCGNCNNTGGCNGCCCCGTT\nNAGNNCTNGNNANTCNTGGNAGCCNGCGGTNN\n>seq3\nANTNANNNCNCNCNTNCNGNANTNNNGNTNGNNNANANANGNTNCNNNTNCNCNANCNTN\nTNNNGNTNGNTNNNANTNGNTNANNNTNCNANNNCNTNTNTNGNGNGNCNNNNNCNCNAN\nNNTNANNNANNNANTNCNGNCNANGNGNCNANTNANANANANNNTNANTNCNGNANANNN\nTNTNCNTNANCNGNNNNNANANGNCNNNGNANCNCNNNCNANNNTNTNANCNNNGNANNN\nANTNNNCNTNTNTNGNANGNCNTNGNANNNANTNGNGNTNTNNNGNANANTNGNCNTNCN\nGNGNTNGNCNGNGNANANNNANTNNNANGNGNGNTNCNGNGNANGNANANTNGNANANTN\nCNTNANCNTNCNTNCNTNANCNTNTNGNANGNGNGNNNCNTNANTNCNCNANTNCNGNCN\nNNTNANGNANTNCNCNCNANGNCNCNANGNGNGNGNCNCNANTNTNNNANANCNTNGNGN\nGNANTNANGNTNGNNNGNTNANGNNNANCNAN\n>seq4\nNTNGNTNGNGNTNCNANGNTNCNTNCNANCNGNNNANGNANGNGNTNTNGNNNCNCNTNN\nNNNANANTNCNANTNGNGNTNCNGNTNCNGNCNNNGNGNANANANNNTNGNNNGNNNANT\nNTNANTNTNNNCNCNANCNTNTNNNANTNTNANCNGNANNNNNCNGNANTNANCNGNGNG\nNTNGNTNTNGNNNANCNCNGNANCNNNCNNNTNCNTNTNGNCNANGNCNNNNNANCNANC\nNNNANCNANCNGNGNCNANGNNNGNTNCNANGNCNNNCNCNTNTNNNANCNCNNNANANN\nNTNCNGNTNNNCNCNCNTNCNNNCNNNGNANTNANCNNNCNTNTNCNANGNCNANCNTNC\nNNNNNCNANTNNNGNANGNCNNNANNNNNCNANANCNANCNNNTNCNANTNNNTNNNNNT\nNCNTNANCNTNGNGNCNTNCNNNCNTNANNNGNCNGNTNGNANTNGNTNNNNNNNGNNNG\nNTNCNNNNNANANGNANTNGNGNTNTNGNNNA\n";
    BOOST_CHECK(output.compare(uppercase) == 0);

    delete cache_p60;
}




/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(test_cache_hybrid)
{
    // generate FASTAFS file from FASTA file
    fasta_to_twobit_fastafs("test/data/test_006.fa", "tmp/test_cache_test_006.fastafs");


    static std::string reference =
        // GENERIC-HEADER - size: 14
        "\x0F\x0A\x46\x53"s//     [0, 3]
        "\x00\x00\x00\x00"s//     [4, 7] version
        "\x80\x00"s//             FASTAFS flag [ 10000000 | 00000000 ]
        "\x00\x00\x01\x01"s //    [index position in file

        // DATA - size: 43
        "\x00\x00\x00\x1B"s//     27 x ACTG's
        "\x92\xD2\x78\x7B\x1B\x98\xD0"s// sequence
        "\x00\x00\x00\x01"s//     n-blocks (0)
        "\x00\x00\x00\x1B"s//     n-block starts (27)
        "\x00\x00\x00\x1B"s//     n-block ends (27)
        "\xDE\x43\x9D\x7D\x4C\xF7\x09\xF5\x8C\x8B\x31\xFD\x0A\x98\xC4\xD6"s//checksum
        "\x00\x00\x00\x00"s//     m-blocks (0)

        // - size: 43
        "\x00\x00\x00\x1B"s//     27 x ACTG's
        "\x92\xD2\x78\x7B\x1B\x98\xD0"s// sequence
        "\x00\x00\x00\x01"s//     n-blocks (0)
        "\x00\x00\x00\x1B"s//     n-block starts (27)
        "\x00\x00\x00\x1B"s//     n-block ends (27)
        "\xF2\xD4\x96\x2C\x55\x08\x81\xCC\x3D\xEF\x86\xDE\xB2\x84\x90\x22"s//checksum
        "\x00\x00\x00\x00"s//     m-blocks (0)

        // target - size: 30
        "\x00\x00\x00\x04"s//     4 x IUPEC
        "\xFF\xCE"s//     NN=\xFF DV=\x??
        "\x00\x00\x00\x00"s//     n-blocks (0)
        "\xB9\xE7\xA7\x5E\xE7\x63\x95\xBE\x6C\xC3\x7C\x3C\xF2\xDB\x32\x31"s// checksum
        "\x00\x00\x00\x00"s//     m-blocks (0)

        // - size: 43
        "\x00\x00\x00\x1B"s//     27 x ACTG's
        "\x92\xD2\x78\x7B\x1B\x98\xD0"s// sequence
        "\x00\x00\x00\x01"s//     n-blocks (0)
        "\x00\x00\x00\x1B"s//     n-block starts (27)
        "\x00\x00\x00\x1B"s//     n-block ends (27)
        "\xDE\x43\x9D\x7D\x4C\xF7\x09\xF5\x8C\x8B\x31\xFD\x0A\x98\xC4\xD6"s//checksum
        "\x00\x00\x00\x00"s//     m-blocks (0)

        // - size: 43
        "\x00\x00\x00\x1B"s//     27 x ACTG's
        "\x92\xD2\x78\x7B\x1B\x98\xD0"s// sequence
        "\x00\x00\x00\x01"s//     n-blocks (0)
        "\x00\x00\x00\x1B"s//     n-block starts (27)
        "\x00\x00\x00\x1B"s//     n-block ends (27)
        "\xF2\xD4\x96\x2C\x55\x08\x81\xCC\x3D\xEF\x86\xDE\xB2\x84\x90\x22"s//checksum
        "\x00\x00\x00\x00"s//     m-blocks (0)

        // target2 - size: 41
        "\x00\x00\x00\x0A"s//     4 x IUPEC
        "\x6D\xE0\xBD\x46\xE7"s//  sequence
        "\x00\x00\x00\x01"s//     n-blocks (1)
        "\x00\x00\x00\x04"s//     n block [1]
        "\x00\x00\x00\x07"s//     n block [1]
        "\x18\xB4\x56\x52\x68\x67\x4F\x40\xE7\xBA\x73\x92\x53\xA5\xA6\x6C"s// checksum
        "\x00\x00\x00\x00"s//    m-blocks (0)

        // INDEX
        "\x00\x00\x00\x06"s     // n sequences

        "\x10\x00"             // [343, 344] complete, DNA and not circular
        "\x0B"s "seq.1[ACTG]"s         // [345, 349] name
        "\x00\x00\x00\x0E"s     //  data position in file (14)

        "\x90\x00"             // RNA
        "\x0B"s "seq.2[ACUG]"s         // [356, 360] name
        "\x00\x00\x00\x39"s     // 57

        "\x50\x00"             // [, ] complete, DNA and not circular
        "\x0C"s "seq.3[IUPEC]"s       // [, ] name
        "\x00\x00\x00\x64"s     // 100

        "\x10\x00"             // [343, 344] complete, DNA and not circular
        "\x0B"s "seq.4[ACTG]"s         // [345, 349] name
        "\x00\x00\x00\x82"s     // [, ] data position in file (141)

        "\x90\x00"             // RNA
        "\x0B"s "seq.5[ACUG]"s         // [356, 360] name
        "\x00\x00\x00\xAD"s     // [, ] data position in file (181)

        "\x50\x00"             // [, ] complete, DNA and not circular
        "\x0C"s "seq.6[IUPEC]"s         // [, ] name
        "\x00\x00\x00\xD8"s     // [, ] data position in file (221)


        // METADATA
        "\x00"s                 // [399] no metadata fields [padding will come soon?]

        // CRC32 checksums
        "\xD3\xBC\xFF\xFE"s // only part that is not yet checked
        ;

    //BOOST_CHECK_EQUAL(written, 403);

    //BOOST_CHECK(output.compare(uppercase) == 0 or output.compare(mixedcase) == 0);
    std::ifstream file("tmp/test_cache_test_006.fastafs", std::ios::in | std::ios::binary | std::ios::ate);
    BOOST_REQUIRE(file.is_open());

    std::streampos size;
    char * buffer;
    size = file.tellg();
    BOOST_CHECK_EQUAL(reference.size(), 376);
    BOOST_CHECK_EQUAL(size, 376);
    buffer = new char [size];

    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();
    for(unsigned int i = 0; i < size; i++) {
        BOOST_CHECK_EQUAL(buffer[i], reference[i]);

        if(reference[i] != buffer[i]) {
            printf("comparing char %i    ** mismatch [ref] %d  %02hhX  != [buf] %d (%c x %02hhX)\n\n", i, reference[i], reference[i], buffer[i], buffer[i], buffer[i]);
        }

    }

    delete[] buffer;


    // check computed file size
    fastafs f = fastafs("");
    f.load("tmp/test_cache_test_006.fastafs");
    BOOST_CHECK_EQUAL(f.fastafs_filesize(), 376);

    const size_t padding = 10;
    ffs2f_init* cache_p10 = f.init_ffs2f(padding, true); // mixed case

    {
        // upper case test
        const uint32_t write_size = 32;
        char buffer2[write_size + 1] = "";
        buffer2[32] = '\0';
        uint32_t written = 0;
        uint32_t w = 0;
        std::string output = "";

        while(written < f.fasta_filesize(padding)) {
            w = f.view_fasta_chunk(cache_p10, buffer2, write_size, written);
            output.append(buffer2, w);
            written += w;
        }

        std::string uppercase = ">seq.1[ACTG]\nACTAGCTACG\nATCGAGTCAG\nACATGCTN\n>seq.2[ACUG]\nACUAGCUACG\nAUCGAGUCAG\nACAUGCUN\n>seq.3[IUPEC]\nNNDV\n>seq.4[ACTG]\nACTAGCTACG\nATCGAGTCAG\nACATGCTN\n>seq.5[ACUG]\nACUAGCUACG\nAUCGAGUCAG\nACAUGCUN\n>seq.6[IUPEC]\nYHVA----BH\nUYVK\n";
        //std::string uppercase = ">seq.1[ACTG]\nACTAGCTACG\nATCGAGTCAG\nACATGCTN\n>seq.2[ACUG]\nACTAGCTACG\nATCGAGTCAG\nACATGCTN\n>seq.3[IUPEC]\nNNDV\n>seq.4[ACTG]\nACTAGCTACG\nATCGAGTCAG\nACATGCTN\n>seq.5[ACUG]\nACTAGCTACG\nATCGAGTCAG\nACATGCTN\n>seq.6[IUPEC]\nYHVA----BH\nUYVK\n";
        BOOST_CHECK(output.compare(uppercase) == 0);
        std::cout << "[" << output << "]\n";
    }
}





BOOST_AUTO_TEST_CASE(test_equality_fourbit_byte)
{
    fourbit_byte b = fourbit_byte();

    char *seq1;
    char *seq2;

    const char *seq;// don't dereference, pointer to static four_bit property

    // test 0000 0000 -> 00000000 -> 0
    b.set(4, 0);// A => 0
    b.set(0, 0);
    BOOST_CHECK_EQUAL(b.data, 0);

    seq1 = b.get(1);
    seq2 = b.get(2);
    seq = b.get();

    BOOST_CHECK_EQUAL(strcmp(seq1, "A"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "AA"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq, "AA"), 0);

    delete[] seq1;
    delete[] seq2;

    // test 11 10  11 11 -> 00001100 -> 239
    b.set(4, 14); // V: 14
    b.set(0, 15); // N: 15
    BOOST_CHECK_EQUAL(b.data, 239);

    seq1 = b.get(1);
    seq2 = b.get(2);
    seq = b.get();

    BOOST_CHECK_EQUAL(strcmp(seq1, "V"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "VN"), 0);

    delete[] seq1;
    delete[] seq2;

    // GT: 0010 0011
    b.set(4, 2); // G
    b.set(0, 3); // T
    BOOST_CHECK_EQUAL(b.data, 35);

    seq1 = b.get(1);
    seq2 = b.get(2);
    seq = b.get();

    BOOST_CHECK_EQUAL(strcmp(seq1, "G"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "GT"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq, "GT"), 0);

    delete[] seq1;
    delete[] seq2;


    // set to UR (0100 0101)
    b.set(4, 4);
    b.set(0, 5);
    BOOST_CHECK_EQUAL(b.data, 69);

    seq1 = b.get(1);
    seq2 = b.get(2);
    seq = b.get();

    BOOST_CHECK_EQUAL(strcmp(seq1, "U"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "UR"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq, "UR"), 0);

    delete[] seq1;
    delete[] seq2;


    // set to AN (0000 1111)
    b.set(4, 0);
    b.set(0, 15);
    BOOST_CHECK_EQUAL(b.data, 15);

    seq1 = b.get(1);
    seq2 = b.get(2);
    seq = b.get();

    BOOST_CHECK_EQUAL(strcmp(seq1, "A"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "AN"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq, "AN"), 0);

    delete[] seq1;
    delete[] seq2;


    // set to NA (1111 0000)
    b.set(4, 15);
    b.set(0, 0);
    BOOST_CHECK_EQUAL(b.data, 240);

    seq1 = b.get(1);
    seq2 = b.get(2);
    seq = b.get();

    BOOST_CHECK_EQUAL(strcmp(seq1, "N"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq2, "NA"), 0);
    BOOST_CHECK_EQUAL(strcmp(seq, "NA"), 0);

    delete[] seq1;
    delete[] seq2;
}





BOOST_AUTO_TEST_CASE(test_cache_004)
{
    size_t written = fasta_to_twobit_fastafs("test/data/test_004.fa", "tmp/test_004.fastafs");

    static std::string reference =
        // GENERIC-HEADER
        "\x0F\x0A\x46\x53"s//     [0, 3]
        "\x00\x00\x00\x00"s//     [4, 7] version
        "\x80\x00"s//             [8, 9] FASTAFS flag [ 00000000 | 00000001 ]
        "\x00\x00\x00\x68"s //    [10, 13] index position in file (104?)

        // DATA
        "\x00\x00\x00\x4B"s//     [14, 17] seq length (75)
        "\xFB\x70\xD8\xC1\x4A\x29\x6E\x35\xD2\xAE"s// [18, 27]    sequence (four bit format; n chars = 76/2 = 38)
        "\x48\x3B\x9C\xFB\x26\x0C\xFD\x98\x43\x51"s// [28, 37]
        "\x7A\xE9\xBD\xEC\xF5\x32\x61\x87\xA4\x00"s// [38, 47]
        "\xE3\x9C\x7F\xB4\x2A\x8D\x65\x10"s// [48, 56]
        "\x00\x00\x00\x02"s//     [, ] n-blocks (2)
        "\x00\x00\x00\x1B"s//     [, ] n-block[0] starts (27)
        "\x00\x00\x00\x4D"s//     [, ] n-block[1] starts (77)
        "\x00\x00\x00\x24"s//     [, ] n-block[0] ends (36|37)
        "\x00\x00\x00\x4F"s//     [, ] n-block[1] ends (79)
        "\x4A\x4D\x43\xFF\x09\x08\x29\xCD\x05\x9A\x08\x3C\x48\x3F\xEB\x3C"s// [76, ] checksum
        "\x00\x00\x00\x01"s//     [92, ] m-blocks (1)
        "\x00\x00\x00\x35"s//     [96, ] m-block starts (53)
        "\x00\x00\x00\x44"s//     [100, ] m-block starts (68)

        // INDEX
        "\x00\x00\x00\x01"s     // [104, ] 1 sequences
        "\x50\x00"              // [, ] complete, IUPEC [01010000]
        "\x05"s "IUPAC"s        // [, ] name
        "\x00\x00\x00\x0E"s     // [, ] data position in file (14)

        // METADATA
        "\x00"s                  // [120] no metadata fields [padding will come soon?]

        // CRC32
        "\x3d\xbf\x6e\xbf"s
        ;

    BOOST_CHECK_EQUAL(written, 125);

    //BOOST_CHECK(output.compare(uppercase) == 0 or output.compare(mixedcase) == 0);
    std::ifstream file("tmp/test_004.fastafs", std::ios::in | std::ios::binary | std::ios::ate);
    BOOST_REQUIRE(file.is_open());

    std::streampos size;
    char * buffer;
    size = file.tellg();
    buffer = new char [size];

    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    BOOST_CHECK_EQUAL(file.gcount(), size);
    file.close();

    //BOOST_CHECK_UNEQUAL(ret, -1);


    for(unsigned int i = 0; i < size; i++) {
        BOOST_CHECK_EQUAL(buffer[i], reference[i]);

        if(reference[i] != buffer[i]) {
            printf("comparing char %u   ** mismatch   [ref] %d %02hhX != [buf] %d (%c x %02hhX)\n", i, reference[i], reference[i], buffer[i], buffer[i], buffer[i]);
        }

    }

    delete[] buffer;



    // check fastafs filesize
    fastafs f = fastafs("");
    f.load("tmp/test_004.fastafs");
    BOOST_CHECK_EQUAL(f.fastafs_filesize(), 125);
}





BOOST_AUTO_TEST_CASE(test_cache_2)
{
    size_t written = fasta_to_twobit_fastafs("test/data/test_005.fa", "tmp/test_005.fastafs");

	BOOST_CHECK_EQUAL(written, 241);

    // check fastafs filesize
    fastafs f = fastafs("");
    f.load("tmp/test_005.fastafs");
    BOOST_CHECK_EQUAL(f.data[0]->name.size(), 131);
	
    
}



BOOST_AUTO_TEST_SUITE_END()
