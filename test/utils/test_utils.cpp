#define BOOST_TEST_MODULE fastfs_cache

#include <sys/types.h>
#include <sys/stat.h>


#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "utils.hpp"


BOOST_AUTO_TEST_SUITE(Testing)

/**
 * @description test contains a sequence that intially failed chunked_view with chunk size > 1
 */
BOOST_AUTO_TEST_CASE(test_utils__twobytes_to_uint)
{
    {
        // test: 00000000 00000000 = 0
        char example[] = "\x00\x00";
        BOOST_CHECK_EQUAL(twobytes_to_uint(&example[0]), 0);
    }

    {
        // test: 00000000 00000001 = 1
        char example[] = "\x00\x01";
        BOOST_CHECK_EQUAL(twobytes_to_uint(&example[0]), 1);
    }

    {
        // test: 00000000 00000010 = 2
        char example[] = "\x00\x02";
        BOOST_CHECK_EQUAL(twobytes_to_uint(&example[0]), 2);
    }

    {
        // test: 00000000 11111111 = 255
        char example[] = "\x00\xFF";
        BOOST_CHECK_EQUAL(twobytes_to_uint(&example[0]), 255);
    }

    {
        // test: 00000001 00000000 = 256
        char example[] = "\x01\x00";
        BOOST_CHECK_EQUAL(twobytes_to_uint(&example[0]), 256);
    }

    {
        // test: 00000001 00000001 = 257
        char example[] = "\x01\x01";
        BOOST_CHECK_EQUAL(twobytes_to_uint(&example[0]), 257);
    }

    {
        // test: 11111111 11111111 = 65535
        char example[] = "\xFF\xFF";
        BOOST_CHECK_EQUAL(twobytes_to_uint(&example[0]), 65535);
    }
}




BOOST_AUTO_TEST_SUITE_END()
