#define BOOST_TEST_MODULE sequence_region

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "SequenceRegion.hpp"


//#include <iostream>
//#include <fstream>


BOOST_AUTO_TEST_SUITE(Testing)

/*
 * Goal is to parse the following strings:
 * "chr1"
 * "chr1:"
 * "chr1:123" # single base
 * "chr1:123-456"
 * "chr1:123-"
 * "chr1:123-456:asdasd" error
 * "chr1:-456" == "chr1:0-456"
 */


BOOST_AUTO_TEST_CASE(test_sequence_region)
{
    {
        char arg[] = "/seq/chr1";
        SequenceRegion sr = SequenceRegion(&(arg[5]));
        
        BOOST_CHECK_EQUAL(sr.seq_name , "chr1");
    }

    {
        char arg[] = "/seq/chr1:";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chr1");
    }


    {
        char arg[] = "/seq/chr1:-123";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chr1");

        BOOST_CHECK_EQUAL(sr.start , 0); // zero based as defined in SAM specification
        BOOST_CHECK_EQUAL(sr.end , 123); // 
    }


}

BOOST_AUTO_TEST_SUITE_END()
