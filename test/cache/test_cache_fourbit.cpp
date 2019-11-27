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





BOOST_AUTO_TEST_SUITE_END()
