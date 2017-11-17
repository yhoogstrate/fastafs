#define BOOST_TEST_MODULE fastfs_cache

#include "two_bit_byte.hpp"

#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(Testing)

/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(test_equality_two_bit_byte)
{
	two_bit_byte b = two_bit_byte();
	b.set(0, 11);
	
	BOOST_CHECK_EQUAL(1, 1);

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
