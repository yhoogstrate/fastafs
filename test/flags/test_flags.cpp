#define BOOST_TEST_MODULE flags

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "flags.hpp"


//#include <iostream>
//#include <fstream>


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_fastafs_flags)
{
    fastafs_flags f;

    unsigned char buffer[2 + 1];
    buffer[2] = '\0';

    // test: 00000000 00000000
    buffer[0] = '\x0';
    buffer[1] = '\x0';
    f.set(buffer);

    BOOST_CHECK_EQUAL(f.is_complete(), false);
    BOOST_CHECK_EQUAL(f.is_incomplete(), true);


    // test: 10000000 00000000
    buffer[0] = '\x80'; // worked with writing to file and checking with `xxd -b file` ~ this is binary equivalent to 10000000
    buffer[1] = '\x0';
    f.set(buffer);

    BOOST_CHECK_EQUAL(f.is_complete(), true);
    BOOST_CHECK_EQUAL(f.is_incomplete(), false);


    // test: 11111111 00000000
    buffer[0] = '\xFF';
    buffer[1] = '\x0';
    f.set(buffer);

    BOOST_CHECK_EQUAL(f.is_complete(), true);
    BOOST_CHECK_EQUAL(f.is_incomplete(), false);

    // test: 00000001 00000000
    buffer[0] = '\x01';
    buffer[1] = '\x0';
    f.set(buffer);

    BOOST_CHECK_EQUAL(f.is_complete(), false);
    BOOST_CHECK_EQUAL(f.is_incomplete(), true);



    // re-test: 00000000 00000000
    buffer[0] = '\x0';
    buffer[1] = '\x0';
    f.set(buffer);

    BOOST_CHECK_EQUAL(f.is_complete(), false);
    BOOST_CHECK_EQUAL(f.is_incomplete(), true);

    f.set_complete();
    BOOST_CHECK_EQUAL(f.is_complete(), true);
    BOOST_CHECK_EQUAL(f.is_incomplete(), false);
    f.set_complete();
    BOOST_CHECK_EQUAL(f.is_complete(), true);
    BOOST_CHECK_EQUAL(f.is_incomplete(), false);
    f.set_complete();
    BOOST_CHECK_EQUAL(f.is_complete(), true);
    BOOST_CHECK_EQUAL(f.is_incomplete(), false);

    f.set_incomplete();
    BOOST_CHECK_EQUAL(f.is_complete(), false);
    BOOST_CHECK_EQUAL(f.is_incomplete(), true);
    f.set_incomplete();
    BOOST_CHECK_EQUAL(f.is_complete(), false);
    BOOST_CHECK_EQUAL(f.is_incomplete(), true);
    f.set_incomplete();
    BOOST_CHECK_EQUAL(f.is_complete(), false);
    BOOST_CHECK_EQUAL(f.is_incomplete(), true);
}


BOOST_AUTO_TEST_CASE(test_fastafs_sequence_flags)
{
    fastafs_sequence_flags fs;

    fs.set_dna();
    fs.set_rna();
    fs.set_iupec_nucleotide();
    BOOST_CHECK_EQUAL(fs.is_iupec_nucleotide(), true);
    BOOST_CHECK_EQUAL(fs.is_dna(), false);
    BOOST_CHECK_EQUAL(fs.is_rna(), false);


    fs.set_iupec_nucleotide();
    fs.set_rna();
    fs.set_dna();
    fs.set_rna();
    fs.set_iupec_nucleotide();
    BOOST_CHECK_EQUAL(fs.is_iupec_nucleotide(), true);
    BOOST_CHECK_EQUAL(fs.is_dna(), false);
    BOOST_CHECK_EQUAL(fs.is_rna(), false);


    fs.set_iupec_nucleotide();
    fs.set_rna();
    fs.set_dna();
    fs.set_dna();
    fs.set_iupec_nucleotide();
    fs.set_rna();
    BOOST_CHECK_EQUAL(fs.is_iupec_nucleotide(), false);
    BOOST_CHECK_EQUAL(fs.is_dna(), false);
    BOOST_CHECK_EQUAL(fs.is_rna(), true);


    fs.set_iupec_nucleotide();
    fs.set_rna();
    fs.set_dna();
    fs.set_dna();
    fs.set_iupec_nucleotide();
    fs.set_rna();
    BOOST_CHECK_EQUAL(fs.is_iupec_nucleotide(), false);
    BOOST_CHECK_EQUAL(fs.is_dna(), false);
    BOOST_CHECK_EQUAL(fs.is_rna(), true);


    fs.set_iupec_nucleotide();
    fs.set_rna();
    fs.set_dna();
    fs.set_iupec_nucleotide();
    fs.set_rna();
    fs.set_dna();
    BOOST_CHECK_EQUAL(fs.is_iupec_nucleotide(), false);
    BOOST_CHECK_EQUAL(fs.is_dna(), true);
    BOOST_CHECK_EQUAL(fs.is_rna(), false);


    fs.set_linear();
    BOOST_CHECK_EQUAL(fs.is_linear(), true);
    BOOST_CHECK_EQUAL(fs.is_circular(), false);

    fs.set_circular();
    fs.set_circular();
    fs.set_linear();
    BOOST_CHECK_EQUAL(fs.is_linear(), true);
    BOOST_CHECK_EQUAL(fs.is_circular(), false);

    fs.set_linear();
    fs.set_linear();
    fs.set_circular();
    BOOST_CHECK_EQUAL(fs.is_linear(), false);
    BOOST_CHECK_EQUAL(fs.is_circular(), true);


    fs.set_complete();
    BOOST_CHECK_EQUAL(fs.is_complete(), true);
    BOOST_CHECK_EQUAL(fs.is_incomplete(), false);

    fs.set_incomplete();
    fs.set_incomplete();
    fs.set_complete();
    BOOST_CHECK_EQUAL(fs.is_complete(), true);
    BOOST_CHECK_EQUAL(fs.is_incomplete(), false);

    fs.set_complete();
    fs.set_complete();
    fs.set_incomplete();
    BOOST_CHECK_EQUAL(fs.is_complete(), false);
    BOOST_CHECK_EQUAL(fs.is_incomplete(), true);



    // get characters
    fs.set_incomplete();
    fs.set_linear();
    fs.set_dna();

    std::array<unsigned char, 2> bits = fs.get_bits();
    BOOST_CHECK_EQUAL(bits[0], '\0');
    BOOST_CHECK_EQUAL(bits[1], '\0');


    fs.set_complete();
    fs.set_circular();
    fs.set_iupec_nucleotide();

    bits = fs.get_bits();
    BOOST_CHECK_EQUAL(bits[0], '\x58');// 1011000
    BOOST_CHECK_EQUAL(bits[1], '\0');

}

BOOST_AUTO_TEST_SUITE_END()
