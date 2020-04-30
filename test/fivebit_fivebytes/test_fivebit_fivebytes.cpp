#define BOOST_TEST_MODULE fivebit_fivebytes

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fivebit_fivebytes.hpp"


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_fivebit_fivebytes_conversions)
{
    unsigned char seq_comp[5];
/*
    //  F      A      S      T      A      -      F      S
    // [05   ][00   ][18   ][19   ][00   ][27   ][05   ][18   ]
    //  00101  00000  10010  10011  00000  11011  00101  10010
    //  00101000 00100101 00110000 01101100 10110010
    //  FFFFFAAA AASSSSST TTTTAAAA A-----FF FFFSSSSS
    //  40       37       48       108      178

    seq_comp[0] = 40;
    seq_comp[1] = 37;
    seq_comp[2] = 48;
    seq_comp[3] = 108;
    seq_comp[4] = 178;

    fivebit_fivebytes f = fivebit_fivebytes();// set_compressed(char *);// string with 5 character - requires unpacking
    f.set_compressed(seq_comp);
    
    unsigned char *seq_decomp = f.get();
    

    BOOST_CHECK_EQUAL(seq_decomp[0], 05);
    BOOST_CHECK_EQUAL(seq_decomp[1], 00);
    BOOST_CHECK_EQUAL(seq_decomp[2], 18);
    BOOST_CHECK_EQUAL(seq_decomp[3], 19);
    BOOST_CHECK_EQUAL(seq_decomp[4], 00);
    BOOST_CHECK_EQUAL(seq_decomp[5], 27);
    BOOST_CHECK_EQUAL(seq_decomp[6], 05);
    BOOST_CHECK_EQUAL(seq_decomp[7], 18);
    
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alphabet[seq_decomp[0]], 'F');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alphabet[seq_decomp[1]], 'A');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alphabet[seq_decomp[2]], 'S');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alphabet[seq_decomp[3]], 'T');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alphabet[seq_decomp[4]], 'A');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alphabet[seq_decomp[5]], '-');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alphabet[seq_decomp[6]], 'F');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alphabet[seq_decomp[7]], 'S');
*/


    fivebit_fivebytes f2 = fivebit_fivebytes();// set_compressed(char *);// string with 5 character - requires unpacking

    f2.data_compressed[0] = 1;
    f2.data_compressed[1] = 3;
    f2.data_compressed[2] = 3;
    f2.data_compressed[3] = 7;
    f2.data_compressed[4] = 255;

    f2.set(0, 05);// F
    f2.set(1, 00);// A
    f2.set(2, 18);// S
    f2.set(3, 19);// T
    f2.set(4, 00);// A
    f2.set(5, 27);// -
    f2.set(6, 05);// F
    f2.set(7, 18);// S

    //  F      A      S      T      A      -      F      S
    // [05   ][00   ][18   ][19   ][00   ][27   ][05   ][18   ]
    //  00101  00000  10010  10011  00000  11011  00101  10010
    //  00101000 00100101 00110000 01101100 10110010
    //  FFFFFAAA AASSSSST TTTTAAAA A-----FF FFFSSSSS
    //  40       37       48       108      178


    BOOST_CHECK_EQUAL(f2.data_compressed[0], 40);
    BOOST_CHECK_EQUAL(f2.data_compressed[1], 37);
    BOOST_CHECK_EQUAL(f2.data_compressed[2], 48);
    BOOST_CHECK_EQUAL(f2.data_compressed[3], 108);
    BOOST_CHECK_EQUAL(f2.data_compressed[4], 178);

    // reset to make sure it needs to be unpacked properly
    f2.data_decompressed[0] = '?';
    f2.data_decompressed[1] = '?';
    f2.data_decompressed[2] = '?';
    f2.data_decompressed[3] = '?';
    f2.data_decompressed[4] = '?';
    f2.data_decompressed[5] = '?';
    f2.data_decompressed[6] = '?';
    f2.data_decompressed[7] = '?';


    f2.unpack();

    BOOST_CHECK_EQUAL(f2.data_decompressed[0], 05);
    BOOST_CHECK_EQUAL(f2.data_decompressed[1], 00);
    BOOST_CHECK_EQUAL(f2.data_decompressed[2], 18);
    BOOST_CHECK_EQUAL(f2.data_decompressed[3], 19);
    BOOST_CHECK_EQUAL(f2.data_decompressed[4], 00);
    BOOST_CHECK_EQUAL(f2.data_decompressed[5], 27);
    BOOST_CHECK_EQUAL(f2.data_decompressed[6], 05);
    BOOST_CHECK_EQUAL(f2.data_decompressed[7], 18);

    BOOST_CHECK_EQUAL( fivebit_fivebytes::fivebit_alphabet[f2.data_decompressed[0]], 'F');
    BOOST_CHECK_EQUAL( fivebit_fivebytes::fivebit_alphabet[f2.data_decompressed[1]], 'A');
    BOOST_CHECK_EQUAL( fivebit_fivebytes::fivebit_alphabet[f2.data_decompressed[2]], 'S');
    BOOST_CHECK_EQUAL( fivebit_fivebytes::fivebit_alphabet[f2.data_decompressed[3]], 'T');
    BOOST_CHECK_EQUAL( fivebit_fivebytes::fivebit_alphabet[f2.data_decompressed[4]], 'A');
    BOOST_CHECK_EQUAL( fivebit_fivebytes::fivebit_alphabet[f2.data_decompressed[5]], '-');
    BOOST_CHECK_EQUAL( fivebit_fivebytes::fivebit_alphabet[f2.data_decompressed[6]], 'F');
    BOOST_CHECK_EQUAL( fivebit_fivebytes::fivebit_alphabet[f2.data_decompressed[7]], 'S');
}





BOOST_AUTO_TEST_SUITE_END()
