#define BOOST_TEST_MODULE fastfs_cache_fourbit

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fourbit_fastafs.hpp"



BOOST_AUTO_TEST_SUITE(Testing)

/**
 * @brief
 *
 * @test
 */
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



/**
 * @brief
 *
 * @test tests whether a fourbit object is indeed stored as a single byte
 */
BOOST_AUTO_TEST_CASE(Test_size)
{
    fourbit_byte b = fourbit_byte();
    BOOST_CHECK_EQUAL(sizeof(b.data), 1);
}




BOOST_AUTO_TEST_CASE(test_cache)
{
    size_t written = fasta_to_fourbit_fastafs("test/data/test_004.fa", "tmp/test_004.fastafs");

    static std::string reference =
        // GENERIC-HEADER
        "\x0F\x0A\x46\x53"s//     [0, 3]
        "\x00\x00\x00\x00"s//     [4, 7] version
        "\x00\x01"s//             [8, 9] FASTAFS flag [ 00000000 | 00000001 ]
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
        "\xEE\x09\x2F\x63\x4F\x6C\x87\xD0\x6B\x57\x1F\x07\xD1\x42\x73\x00"s// [76, ] checksum
        "\x00\x00\x00\x01"s//     [92, ] m-blocks (1)
        "\x00\x00\x00\x35"s//     [96, ] m-block starts (53)
        "\x00\x00\x00\x44"s//     [100, ] m-block starts (68)

        // INDEX
        "\x00\x00\x00\x01"s     // [104, ] 1 sequences
        "\x00\x0A"              // [, ] complete, IUPEC
        "\x05"s "IUPAC"s        // [, ] name
        "\x00\x00\x00\x0E"s     // [, ] data position in file (14)

        // METADATA
        "\x00"                  // [120] no metadata fields [padding will come soon?]
        ;

    BOOST_CHECK_EQUAL(written, 121);

    //BOOST_CHECK(output.compare(uppercase) == 0 or output.compare(mixedcase) == 0);
    std::ifstream file("tmp/test_004.fastafs", std::ios::in | std::ios::binary | std::ios::ate);
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

        if(reference[i] != buffer[i]) {
            printf("comparing char %i\n", i);
            printf(" ** mismatch [%d] [ref] %d != [buf] %d (%c x %02hhX)\n", i, reference[i], buffer[i], buffer[i], buffer[i]);
        }

    }

    delete[] buffer;
}



BOOST_AUTO_TEST_SUITE_END()
