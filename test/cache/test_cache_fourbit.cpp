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
        "\x00\x00\x00\x4C"s//     [14, 17] seq length (76)
        "\x00\x55\xAA\xFF\x00\x00\x00\x00\x00\x00"s// 1    sequence (four bit format; n chars = 76/2 = 38)
        "\x00\x55\xAA\xFF\x00\x00\x00\x00\x00\x00"s// 2
        "\x00\x55\xAA\xFF\x00\x00\x00\x00\x00\x00"s// 3
        "\x00\x55\xAA\xFF\x00\x00\x00\x00"s// 4
        "\x00\x55\xAA\xFF\x00"s// 12
        "\x00\x00\x00\x02"s//     [22, 25] n-blocks (2)
        "\x00\x00\x00\x1A"s//     [50, 53] n-block[0] starts (26)
        "\x00\x00\x00\x24"s//     [54, 57] n-block[0] ends (36|37)
        "\x00\x00\x00\x4D"s//     [50, 53] n-block[1] starts (77)
        "\x00\x00\x00\x4F"s//     [54, 57] n-block[1] ends (79)
        "\x75\x25\x5C\x6D\x90\x77\x89\x99\xAD\x36\x43\xA2\xE6\x9D\x43\x44"s// [26, 45] checksum
        "\x00\x00\x00\x01"s//     [46, 49] m-blocks (1)
        "\x00\x00\x00\x00"s//     [50, 53] m-block starts (53)
        "\x00\x00\x00\x0F"s//     [54, 57] m-block starts (65)

        // INDEX
        "\x00\x00\x00\x01"s     // [339, 342] 1 sequences
        "\x00\xA0"              // [343, 344] complete, IUPEC
        "\x05"s "IUPAC"s        // [345, 349] name
        "\x00\x00\x00\x0E"s     // [350, 353] data position in file (14)

        // METADATA
        "\x00"                  // [399] no metadata fields [padding will come soon?]
        ;

		BOOST_CHECK_EQUAL(written, 121);
}



BOOST_AUTO_TEST_SUITE_END()
