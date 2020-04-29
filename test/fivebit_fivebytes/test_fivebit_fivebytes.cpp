#define BOOST_TEST_MODULE fivebit_fivebytes

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fivebit_fivebytes.hpp"


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_fivebit_fivebytes_conversions)
{
    unsigned char seq_comp[5];
    unsigned char seq_decomp[8];
    //static const char fivebit_fivebytes::fivebit_alhpabet[28 + 1] = "ABCDEFGHIJKLMNOPQRSTUVWYZX*-";

    // set to:
    // ABCDEFGHIJKLMNOPQRSTUVWYZX*-

    //  F      A      S      T      A      -      F      S
    // [05   ][00   ][18   ][19   ][00   ][27   ][05   ][18   ]
    //  00101  00000  10010  10011  00000  11011  00101  10010
    //  00101000 00100101 00110000 01101100 10110010
    //  40       37       48       108      178

    //  00000  11111  22222  33333  44444  55555  66666  77777
    //  00000111 11222223 33334444 45555566 66677777

    seq_comp[0] = 40;
    seq_comp[1] = 37;
    seq_comp[2] = 48;
    seq_comp[3] = 108;
    seq_comp[4] = 178;


    //  00101000 00100101 00110000 01101100 10110010
    //  00000111 11222223 33334444 45555566 66677777
    //                                      66677777
    //                                      ---77777
    seq_decomp[7] = (unsigned char)(seq_comp[4] & ~((4 + 2 + 1) << 5));


    // slightly slower implementation
    //        seq_decomp[5] = (unsigned char)(seq_comp[3] << 3);
    //        seq_decomp[5] = (unsigned char)(seq_decomp[5] & ~((4 + 2 + 1) << 5));
    //        seq_decomp[6] = (unsigned char)(seq_comp[4] >> 5);
    //        seq_decomp[6] = seq_decomp[5] | seq_decomp[6];


    // method 2 = ~2.46s
    //  00101000 00100101 00110000 01101100 10110010
    //  00000111 11222223 33334444 45555566 66677777
    //                             45555566 66677777
    //                             -----455 55566666
    //                             -----455 ---66666
    //                                      ---66666
    seq_decomp[6] = (unsigned char)(((seq_comp[3] << 8) | (seq_comp[4])) >> 5);
    seq_decomp[6] = (unsigned char)(seq_decomp[6] & ~((4 + 2 + 1) << 5));


    //  00101000 00100101 00110000 01101100 10110010
    //  00000111 11222223 33334444 45555566 66677777
    //                             45555566
    //                             --455555
    //                             ---55555
    seq_decomp[5] = (unsigned char)(seq_comp[3] >> 2);
    seq_decomp[5] = (unsigned char)(seq_decomp[5] & ~((4 + 2 + 1) << 5));// only bit 6 should be set to 0


    //  00101000 00100101 00110000 01101100 10110010
    //  00000111 11222223 33334444 45555566 66677777
    //                    33334444 45555566
    //                    -------3 33344444
    //                             33344444
    //                             ---44444
    seq_decomp[4] = (unsigned char)(((seq_comp[2] << 8) | (seq_comp[3])) >> 7);
    seq_decomp[4] = (unsigned char)(seq_decomp[4] & ~((4 + 2 + 1) << 5));
    

    //  00101000 00100101 00110000 01101100 10110010
    //  00000111 11222223 33334444 45555566 66677777
    //  11222223 33334444       bit shift << 8 + normal
    //  ----1122 22233333       bit shift >> 4
    //           22233333       convert to u-char
    //           ---33333       set zero's
    seq_decomp[3] = (unsigned char)(((seq_comp[1] << 8) | (seq_comp[2])) >> 4);
    seq_decomp[3] = (unsigned char)(seq_decomp[3] & ~((4 + 2 + 1) << 5));


    //  00101000 00100101 00110000 01101100 10110010
    //  00000111 11222223 33334444 45555566 66677777
    //           11222223
    //           -1122222
    //           ---22222
    seq_decomp[2] = (unsigned char)(seq_comp[1] >> 1);   // shifts of unsigned types always zero-fill :)
    seq_decomp[2] = (unsigned char)(seq_decomp[2] & ~((4 + 2 + 1) << 5)); // i think only bit 6 and 7 need to be set because of the shift above


    //  00101000 00100101 00110000 01101100 10110010
    //  00000111 11222223 33334444 45555566 66677777
    //  00000111 11222223
    //  ------00 00011111
    //           00011111
    //           ---11111
    seq_decomp[1] = (unsigned char)(((seq_comp[0] << 8) | (seq_comp[1])) >> 6);
    seq_decomp[1] = (unsigned char)(seq_decomp[1] & ~((4 + 2 + 1) << 5));


    //  00101000 00100101 00110000 01101100 10110010
    //  00000111
    //  ---00000
    seq_decomp[0] = (unsigned char)(seq_comp[0] >> 3);   // shifts of unsigned types always zero-fill :)



    printf("0: [%i][%u]\t[%c]\n", seq_decomp[0], seq_decomp[0], fivebit_fivebytes::fivebit_alhpabet[seq_decomp[0]]);
    printf("1: [%i][%u]\t[%c]\n", seq_decomp[1], seq_decomp[1], fivebit_fivebytes::fivebit_alhpabet[seq_decomp[1]]);
    printf("2: [%i][%u]\t[%c]\n", seq_decomp[2], seq_decomp[2], fivebit_fivebytes::fivebit_alhpabet[seq_decomp[2]]);
    printf("3: [%i][%u]\t[%c]\n", seq_decomp[3], seq_decomp[3], fivebit_fivebytes::fivebit_alhpabet[seq_decomp[3]]);
    printf("4: [%i][%u]\t[%c]\n", seq_decomp[4], seq_decomp[4], fivebit_fivebytes::fivebit_alhpabet[seq_decomp[4]]);
    printf("5: [%i][%u]\t[%c]\n", seq_decomp[5], seq_decomp[5], fivebit_fivebytes::fivebit_alhpabet[seq_decomp[5]]);
    printf("6: [%i][%u]\t[%c]\n", seq_decomp[6], seq_decomp[6], fivebit_fivebytes::fivebit_alhpabet[seq_decomp[6]]);
    printf("7: [%i][%u]\t[%c]\n", seq_decomp[6], seq_decomp[7], fivebit_fivebytes::fivebit_alhpabet[seq_decomp[7]]);

    BOOST_CHECK_EQUAL(seq_decomp[0], 05);
    BOOST_CHECK_EQUAL(seq_decomp[1], 00);
    BOOST_CHECK_EQUAL(seq_decomp[2], 18);
    BOOST_CHECK_EQUAL(seq_decomp[3], 19);
    BOOST_CHECK_EQUAL(seq_decomp[4], 00);
    BOOST_CHECK_EQUAL(seq_decomp[5], 27);
    BOOST_CHECK_EQUAL(seq_decomp[6], 05);
    BOOST_CHECK_EQUAL(seq_decomp[7], 18);
    
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alhpabet[seq_decomp[0]], 'F');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alhpabet[seq_decomp[1]], 'A');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alhpabet[seq_decomp[2]], 'S');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alhpabet[seq_decomp[3]], 'T');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alhpabet[seq_decomp[4]], 'A');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alhpabet[seq_decomp[5]], '-');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alhpabet[seq_decomp[6]], 'F');
    BOOST_CHECK_EQUAL(fivebit_fivebytes::fivebit_alhpabet[seq_decomp[7]], 'S');
}





BOOST_AUTO_TEST_SUITE_END()
