#define BOOST_TEST_MODULE fastfs_cache

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "twobit_byte.hpp"



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

BOOST_AUTO_TEST_SUITE_END()
