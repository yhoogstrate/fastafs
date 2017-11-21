#define BOOST_TEST_MODULE fastfs_cache

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "two_bit_byte.hpp"



BOOST_AUTO_TEST_SUITE(Testing)

/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(test_equality_two_bit_byte)
{
	two_bit_byte b = two_bit_byte();
	char *seq1;
	char *seq2;
	char *seq3;
	char *seq4;
	
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
	BOOST_CHECK_EQUAL(strcmp(seq1, "T"), 0);
	BOOST_CHECK_EQUAL(strcmp(seq2, "TT"), 0);
	BOOST_CHECK_EQUAL(strcmp(seq3, "TTT"), 0);
	BOOST_CHECK_EQUAL(strcmp(seq4, "TTTT"), 0);
	delete seq1;
	delete seq2;
	delete seq3;
	delete seq4;
	
	
	// test 00 00 11 00 -> 00001100 -> 8+4 -> 12
	b.set(6, NUCLEOTIDE_T);
	b.set(4, NUCLEOTIDE_T);
	b.set(2, NUCLEOTIDE_G);
	b.set(0, NUCLEOTIDE_T);
	printf("[%i] [%s]\n", b.data, b.get());
	BOOST_CHECK_EQUAL(b.data, 12);
	
	
	// test 00 11 00 00 -> 00110000 -> 16+32 -> 48
	b.set(6, NUCLEOTIDE_T);
	b.set(4, NUCLEOTIDE_G);
	b.set(2, NUCLEOTIDE_T);
	b.set(0, NUCLEOTIDE_T);
	printf("[%i] [%s]\n", b.data, b.get());
	BOOST_CHECK_EQUAL(b.data, 48);
	
	
	// test 11 00 00 00 -> 11000000 -> 64+128 -> 192
	b.set(6, NUCLEOTIDE_G);
	b.set(4, NUCLEOTIDE_T);
	b.set(2, NUCLEOTIDE_T);
	b.set(0, NUCLEOTIDE_T);
	printf("[%i] [%s]\n", b.data, b.get());
	BOOST_CHECK_EQUAL(b.data, 192);
	
	
	// test 10 01 01 10 -> 10010110 -> 2 + 4 + 16 + 128 -> 150
	b.set(6, NUCLEOTIDE_A);
	b.set(4, NUCLEOTIDE_C);
	b.set(2, NUCLEOTIDE_C);
	b.set(0, NUCLEOTIDE_A);
	printf("[%i] [%s]\n", b.data, b.get());
	BOOST_CHECK_EQUAL(b.data, 150);
	
	
	// test 11 11 11 11 -> 11111111 -> 255
	b.set(6, NUCLEOTIDE_G);
	b.set(4, NUCLEOTIDE_G);
	b.set(2, NUCLEOTIDE_G);
	b.set(0, NUCLEOTIDE_G);
	printf("[%i] [%s]\n", b.data, b.get());
	BOOST_CHECK_EQUAL(b.data, 255);
	
	
	// test 00 00 00 00 -> 00000000 -> 0
	b.set(6, NUCLEOTIDE_T);
	b.set(4, NUCLEOTIDE_T);
	b.set(2, NUCLEOTIDE_T);
	b.set(0, NUCLEOTIDE_T);
	printf("[%i] [%s]\n", b.data, b.get());
	BOOST_CHECK_EQUAL(b.data, 0);
	
}



/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(Test_size)
{
}

BOOST_AUTO_TEST_SUITE_END()
