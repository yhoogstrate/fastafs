#define BOOST_TEST_MODULE twobit_byte

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "twobit_byte.hpp"


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_twobit_conversions)
{
    char seq[5];
    const char *seq_ret;
    seq[4] = '\0';

    {
        twobit_byte_dna t = twobit_byte_dna();
        seq[0] = 'A';
        seq[1] = 'A';
        seq[2] = 'A';
        seq[3] = 'A';
        t.set(seq);//10101010 = 170
        BOOST_CHECK_EQUAL(t.data, 170);

        seq[0] = 'T';
        seq[1] = 'A';
        seq[2] = 'A';
        seq[3] = 'A';
        t.set(seq);
        BOOST_CHECK_EQUAL(t.data, 42);

        seq[0] = 'A';
        seq[1] = 'C';
        seq[2] = 'T';
        seq[3] = 'G';
        t.set(seq);
        BOOST_CHECK_EQUAL(t.data, 147);

        seq[0] = 'N';
        seq[1] = 'C';
        seq[2] = 'T';
        seq[3] = 'N';
        t.set(seq);//00 01 00 00
        BOOST_CHECK_EQUAL(t.data, 16);
    }

    {
        twobit_byte_rna t = twobit_byte_rna();
        seq[0] = 'A';
        seq[1] = 'A';
        seq[2] = 'A';
        seq[3] = 'A';
        t.set(seq);//10101010 = 170
        BOOST_CHECK_EQUAL(t.data, 170);

        seq_ret = t.get();
        BOOST_CHECK_EQUAL(seq_ret[0], 'A');
        BOOST_CHECK_EQUAL(seq_ret[1], 'A');
        BOOST_CHECK_EQUAL(seq_ret[2], 'A');
        BOOST_CHECK_EQUAL(seq_ret[3], 'A');


        seq[0] = 'T';
        seq[1] = 'A';
        seq[2] = 'A';
        seq[3] = 'A';
        t.set(seq);
        BOOST_CHECK_EQUAL(t.data, 42);
        seq_ret = t.get();
        BOOST_CHECK_EQUAL(seq_ret[0], 'U');
        BOOST_CHECK_EQUAL(seq_ret[1], 'A');
        BOOST_CHECK_EQUAL(seq_ret[2], 'A');
        BOOST_CHECK_EQUAL(seq_ret[3], 'A');


        seq[0] = 'A';
        seq[1] = 'C';
        seq[2] = 'T';
        seq[3] = 'G';
        t.set(seq);
        BOOST_CHECK_EQUAL(t.data, 147);
        seq_ret = t.get();
        BOOST_CHECK_EQUAL(seq_ret[0], 'A');
        BOOST_CHECK_EQUAL(seq_ret[1], 'C');
        BOOST_CHECK_EQUAL(seq_ret[2], 'U');
        BOOST_CHECK_EQUAL(seq_ret[3], 'G');


        seq[0] = 'N';
        seq[1] = 'C';
        seq[2] = 'T';
        seq[3] = 'N';
        t.set(seq);//00 01 00 00
        BOOST_CHECK_EQUAL(t.data, 16);

        seq_ret = t.get();
        BOOST_CHECK_EQUAL(seq_ret[0], 'U');// N gets stored as U?
        BOOST_CHECK_EQUAL(seq_ret[1], 'C');
        BOOST_CHECK_EQUAL(seq_ret[2], 'U');
        BOOST_CHECK_EQUAL(seq_ret[3], 'U');// N gets stored as U?


    }


}

BOOST_AUTO_TEST_CASE(test_twobit_static_offset_conversion_test)
{
    BOOST_CHECK_EQUAL(twobit_byte::iterator_to_offset(0), 6);
    BOOST_CHECK_EQUAL(twobit_byte::iterator_to_offset(1), 4);
    BOOST_CHECK_EQUAL(twobit_byte::iterator_to_offset(2), 2);
    BOOST_CHECK_EQUAL(twobit_byte::iterator_to_offset(3), 0);
    BOOST_CHECK_EQUAL(twobit_byte::iterator_to_offset(4), 6);
    BOOST_CHECK_EQUAL(twobit_byte::iterator_to_offset(5), 4);
    BOOST_CHECK_EQUAL(twobit_byte::iterator_to_offset(6), 2);
    BOOST_CHECK_EQUAL(twobit_byte::iterator_to_offset(7), 0);
    BOOST_CHECK_EQUAL(twobit_byte::iterator_to_offset(8), 6);
    BOOST_CHECK_EQUAL(twobit_byte::iterator_to_offset(9), 4);
    BOOST_CHECK_EQUAL(twobit_byte::iterator_to_offset(10), 2);
    BOOST_CHECK_EQUAL(twobit_byte::iterator_to_offset(11), 0);
}




BOOST_AUTO_TEST_SUITE_END()
