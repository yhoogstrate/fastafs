#define BOOST_TEST_MODULE fourbit_byte

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fourbit_byte.hpp"


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_fourbit_conversions)
{
    char seq[3];
    const char* seq_get;
    seq[2] = '\0';
    fourbit_byte t;

    seq[0] = 'A';
    seq[1] = 'C';
    t.set(seq);
    seq_get = t.get();
    printf("[%s] -> %i ~ %u -> [%s]\n", seq, (signed char) t.data, (unsigned char) t.data, seq_get);
    BOOST_CHECK_EQUAL(seq[0], seq_get[0]);
    BOOST_CHECK_EQUAL(seq[1], seq_get[1]);
    BOOST_CHECK_EQUAL(t.data, 1);

    seq[0] = 'G';
    seq[1] = 'T';
    t.set(seq);
    seq_get = t.get();
    printf("[%s] -> %i ~ %u -> [%s]\n", seq, (signed char) t.data, (unsigned char) t.data, seq_get);
    BOOST_CHECK_EQUAL(seq[0], seq_get[0]);
    BOOST_CHECK_EQUAL(seq[1], seq_get[1]);
    BOOST_CHECK_EQUAL(t.data, 35);

    seq[0] = 'U';
    seq[1] = 'R';
    t.set(seq);
    seq_get = t.get();
    printf("[%s] -> %i ~ %u -> [%s]\n", seq, (signed char) t.data, (unsigned char) t.data, seq_get);
    BOOST_CHECK_EQUAL(seq[0], seq_get[0]);
    BOOST_CHECK_EQUAL(seq[1], seq_get[1]);
    BOOST_CHECK_EQUAL(t.data, 69);

    seq[0] = 'Y';
    seq[1] = 'K';
    t.set(seq);
    seq_get = t.get();
    printf("[%s] -> %i ~ %u -> [%s]\n", seq, (signed char) t.data, (unsigned char) t.data, seq_get);
    BOOST_CHECK_EQUAL(seq[0], seq_get[0]);
    BOOST_CHECK_EQUAL(seq[1], seq_get[1]);
    BOOST_CHECK_EQUAL(t.data, 103);

    seq[0] = 'M';
    seq[1] = 'S';
    t.set(seq);
    seq_get = t.get();
    printf("[%s] -> %i ~ %u -> [%s]\n", seq, (signed char) t.data, (unsigned char) t.data, seq_get);
    BOOST_CHECK_EQUAL(seq[0], seq_get[0]);
    BOOST_CHECK_EQUAL(seq[1], seq_get[1]);
    BOOST_CHECK_EQUAL(t.data, 137);

    seq[0] = 'W';
    seq[1] = 'B';
    t.set(seq);
    seq_get = t.get();
    printf("[%s] -> %i ~ %u -> [%s]\n", seq, (signed char) t.data, (unsigned char) t.data, seq_get);
    BOOST_CHECK_EQUAL(seq[0], seq_get[0]);
    BOOST_CHECK_EQUAL(seq[1], seq_get[1]);
    BOOST_CHECK_EQUAL(t.data, 171);

    seq[0] = 'D';
    seq[1] = 'H';
    t.set(seq);
    seq_get = t.get();
    printf("[%s] -> %i ~ %u -> [%s]\n", seq, (signed char) t.data, (unsigned char) t.data, seq_get);
    BOOST_CHECK_EQUAL(seq[0], seq_get[0]);
    BOOST_CHECK_EQUAL(seq[1], seq_get[1]);
    BOOST_CHECK_EQUAL(t.data, 205);

    seq[0] = 'V';
    seq[1] = 'N';
    t.set(seq);
    seq_get = t.get();
    printf("[%s] -> %i ~ %u -> [%s]\n", seq, (signed char) t.data, (unsigned char) t.data, seq_get);
    BOOST_CHECK_EQUAL(seq[0], seq_get[0]);
    BOOST_CHECK_EQUAL(seq[1], seq_get[1]);
    BOOST_CHECK_EQUAL(t.data, 239);

}

BOOST_AUTO_TEST_CASE(test_fourbit_static_offset_conversion_test)
{
    BOOST_CHECK_EQUAL(fourbit_byte::iterator_to_offset(0), 6);
    BOOST_CHECK_EQUAL(fourbit_byte::iterator_to_offset(1), 4);
    BOOST_CHECK_EQUAL(fourbit_byte::iterator_to_offset(2), 2);
    BOOST_CHECK_EQUAL(fourbit_byte::iterator_to_offset(3), 0);
    BOOST_CHECK_EQUAL(fourbit_byte::iterator_to_offset(4), 6);
    BOOST_CHECK_EQUAL(fourbit_byte::iterator_to_offset(5), 4);
    BOOST_CHECK_EQUAL(fourbit_byte::iterator_to_offset(6), 2);
    BOOST_CHECK_EQUAL(fourbit_byte::iterator_to_offset(7), 0);
    BOOST_CHECK_EQUAL(fourbit_byte::iterator_to_offset(8), 6);
    BOOST_CHECK_EQUAL(fourbit_byte::iterator_to_offset(9), 4);
    BOOST_CHECK_EQUAL(fourbit_byte::iterator_to_offset(10), 2);
    BOOST_CHECK_EQUAL(fourbit_byte::iterator_to_offset(11), 0);
}




BOOST_AUTO_TEST_SUITE_END()
