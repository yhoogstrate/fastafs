#define BOOST_TEST_MODULE sequence_region

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "SequenceRegion.hpp"


//#include <iostream>
//#include <fstream>


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_sequence_region)
{
	/*
 * Goal is to parse the following strings:
 * "chr1"
 * "chr1:"
 * "chr1:123-456"
 * "chr1:123-"
 * "chr1:123-456:asdasd" error
 * "chr1:-456" == "chr1:0-456"
 */
 
    char arg[] = "/seq/chr1";

    SequenceRegion sr = SequenceRegion(&(arg[5]));
}

BOOST_AUTO_TEST_SUITE_END()
