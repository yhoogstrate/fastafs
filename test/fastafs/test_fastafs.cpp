#define BOOST_TEST_MODULE fastfs_cache

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"



BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_fastafs_seq_static_func)
{
    // 1: create FASTAFS file
    // then: check returncodes of:
    // 2: fastafs[chr1].view_fasta_chunk();
    // and require 0 when eof is reached
}




BOOST_AUTO_TEST_SUITE_END()
