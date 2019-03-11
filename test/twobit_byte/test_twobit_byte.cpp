#define BOOST_TEST_MODULE twobit_byte

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "twobit_byte.hpp"


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_twobit_conversions)
{
    char buf[5];
    char seq[5];
    seq[4] = '\0';

    twobit_byte t;
    unsigned int i;

    for(i = 0; i < 5; i++) {// iterate over 0, 1, 2, 3 and 4
        buf[i] = '\0';
    }

    seq[0] = 'A';
    seq[1] = 'A';
    seq[2] = 'A';
    seq[3] = 'A';
    t.set(seq);//10101010 = 170
    printf("[%s] -> %i ~ %u -> [%s]\n", seq, (signed char) t.data, (unsigned char) t.data, t.get());
    BOOST_CHECK_EQUAL(t.data, 170);

    seq[0] = 'T';
    seq[1] = 'A';
    seq[2] = 'A';
    seq[3] = 'A';
    t.set(seq);
    printf("[%s] -> %i ~ %u -> [%s]\n", seq, (signed char) t.data, (unsigned char) t.data, t.get());
    BOOST_CHECK_EQUAL(t.data, 42);

    seq[0] = 'A';
    seq[1] = 'C';
    seq[2] = 'T';
    seq[3] = 'G';
    t.set(seq);
    printf("[%s] -> %i ~ %u -> [%s]\n", seq, (signed char) t.data, (unsigned char) t.data, t.get());
    BOOST_CHECK_EQUAL(t.data, 147);

    seq[0] = 'N';
    seq[1] = 'C';
    seq[2] = 'T';
    seq[3] = 'N';
    t.set(seq);//00 01 00 00
    printf("[%s] -> %i ~ %u -> [%s]\n", seq, (signed char) t.data, (unsigned char) t.data, t.get());
    BOOST_CHECK_EQUAL(t.data, 16);
}




BOOST_AUTO_TEST_SUITE_END()
