#define BOOST_TEST_MODULE fivebit_fivebytes

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fivebit_fivebytes.hpp"


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_fivebit_fivebytes_conversions)
{
    unsigned char seq_comp[5];

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

    char *seq_decomp = f.get();


    BOOST_CHECK_EQUAL(seq_decomp[0], 'F');
    BOOST_CHECK_EQUAL(seq_decomp[1], 'A');
    BOOST_CHECK_EQUAL(seq_decomp[2], 'S');
    BOOST_CHECK_EQUAL(seq_decomp[3], 'T');
    BOOST_CHECK_EQUAL(seq_decomp[4], 'A');
    BOOST_CHECK_EQUAL(seq_decomp[5], '-');
    BOOST_CHECK_EQUAL(seq_decomp[6], 'F');
    BOOST_CHECK_EQUAL(seq_decomp[7], 'S');


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

    BOOST_CHECK_EQUAL(f2.data_decompressed[0], 'F');
    BOOST_CHECK_EQUAL(f2.data_decompressed[1], 'A');
    BOOST_CHECK_EQUAL(f2.data_decompressed[2], 'S');
    BOOST_CHECK_EQUAL(f2.data_decompressed[3], 'T');
    BOOST_CHECK_EQUAL(f2.data_decompressed[4], 'A');
    BOOST_CHECK_EQUAL(f2.data_decompressed[5], '-');
    BOOST_CHECK_EQUAL(f2.data_decompressed[6], 'F');
    BOOST_CHECK_EQUAL(f2.data_decompressed[7], 'S');
}



BOOST_AUTO_TEST_CASE(test_fivebit_fivebytes__dict_conversions)
{
    char hash[255];
    hash['A'] = 0;
    hash['B'] = 1;
    hash['C'] = 2;
    hash['D'] = 3;
    hash['E'] = 4;
    hash['F'] = 5;
    hash['G'] = 6;
    hash['H'] = 7;
    hash['I'] = 8;
    hash['J'] = 9;
    hash['K'] = 10;
    hash['L'] = 11;
    hash['M'] = 12;
    hash['N'] = 13;
    hash['O'] = 14;
    hash['P'] = 15;
    hash['Q'] = 16;
    hash['R'] = 17;
    hash['S'] = 18;
    hash['T'] = 19;
    hash['U'] = 20;
    hash['V'] = 21;
    hash['W'] = 22;
    hash['Y'] = 23;
    hash['Z'] = 24;
    hash['X'] = 25;
    hash['*'] = 26;
    hash['-'] = 27;


    fivebit_fivebytes f = fivebit_fivebytes();// set_compressed(char *);// string with 5 character - requires unpacking

    std::vector<std::string> dict = {"FASTA-FS", "FRATSAST", "UCTFXJNH", "CGLWQNSI", "*OLS*DEN", "LAGD*PYE", "HFFXWSVZ", "ABBECXVW", "YMGTTOX*", "I-XVFAQX", "KEYVHNER", "IUISR-ZH", "JHCVXJMK", "NNFKGPOW", "WVGTUHYB", "SYGCM-UQ", "-KLOGKUC", "W*SWGLIJ", "*ZKJTHTV", "UWRTTHUM", "XLJTHJEQ", "LRFPJHAR", "ZJAVVMHP", "MDOADYFU", "NCK*CYNZ", "YMQZXEUR", "FBHAEAZS", "ERBIE-NQ", "GIJRSMBZ", "SEAZ*PO-", "PKB*XTLD", "GDFWBYMA", "RDGF-TN-", "P-LUKTWO", "-PXBTMBL", "OOJVYSJ*", "NDMCNXNQ", "OVNK-DVU", "FPIHDXEK", "L*AKZMMG", "AYQIQQXF", "CEWTCTDR", "YQPUSSPI", "ZIRY*WWR", "MJWQAOWO", "OCYFLK-V", "GQOEOZFZ", "*SFXOJX*", "PK-BHBLJ", "QPS*KSVV", "IHDZCVM-", "-GWREDJB", "PEKCDABZ", "ULBUMPNG", "ICYANTRP", "CYQGQHR*", "ZDISLFU-", "DIUBILAR", "VSWWTJAP", "PXYJYBIH", "NA-LNTOS", "FQIUFQRX", "KACQOLYW", "TWXUYV*F", "YMANTHBD", "KLZGRNCO", "BSKQ*GDM", "PFELJUIZ", "Q-WOCMLE", "AXJNEKCQ", "WOLPA*FZ", "DMHVYSFS", "RIFXSLYC", "-CQFIYZE", "R*EZXK-V", "WVPJ*HGJ", "HUDGTUV*", "PUC-GHYT", "M-*ZOJVB", "VD*LXUAU", "OLVEK-ZX", "WIVVTQRE", "-MAPUSWY", "IJHTQSGL", "OHIKEZEE", "G-ICTNTZ", "GMRS-DP-", "ARZEWZFS", "RGTFGQFR", "JKD-ELBY", "A-XDUGGM", "MFLWDXAN", "DGOFAMAI", "O**O-STQ", "-VNKGBJC", "RPMZHJJY", "XDBBUKQK", "CUFICTTW", "AUGSBPPE", "*PYTZKEF", "AHYSKALD", "KQMB*BNN", "*HWNMGYV", "IRWY*WBU", "LJNNZNLO", "PHVFRNWR", "CCCIHRHD", "JBORIXIX", "JT*SZHKP", "ZXURDOQD", "CPSBC-VD", "IHMEOQLB", "KXNMOVEX", "SJPQUBM*", "ZCLXJSIA", "YBHNUNHH", "AYTGKWSO", "IHXJ*TD-", "MVAJKHUH", "QUXLEYWW", "TSROZJIQ", "LIAPGNHF", "RCMXSNLM", "GF*-VRJK", "IKDCTKFY", "VAPADEGP", "GKHLIWHU", "-OH-F-EZ", "*GPJZCNY", "RVUGFGVA", "JLLYKRWZ", "WSOWVDKV", "QPJZLBPH", "BYTSODIJ", "-MCJFBFU", "D*NIFL*L", "IG*CW*ZM", "GZTGQWHC", "CTCTBRDU", "IBNLOPPM", "DTP-HLXN", "YQRZ*GWS", "YFVBQEMM", "MYARXKOF", "SQDKDEQZ", "RKKVOEOL", "XA*YVU-W", "CQZQXTRV", "VP*YCJQM", "JWEF*O-J", "LFUTAQSF", "-EEILVRG", "DKNSYQZ-", "EQLM*BT-", "TGNJDBDS", "AJ-IGQLP", "PPDRKUKA", "WDDKVXXX", "HCHASUOB", "-RCUHJOJ", "BVANOTRB", "TYGLLX*H", "SYBOLOOM", "IIBETFMT", "PDFGC-CE", "*PTPEDLR", "ZZQQESJB", "HMNGEPEK", "UPFIPVQN", "XBWOFKFY", "XDOPNZUI", "JEMVIXKL", "EXQWSNKQ", "FAAIORTR", "BWXEMW-D", "ZVT-NUKK", "YRCACBAE", "ASDZZFXM", "XHHSBSRN", "UYSVOSIT", "DVNEHROM", "*PUCVJIC", "NYUWPZ-I", "Y*-GH-AV", "CBB-FGNQ", "CUCEAWMU", "FZO*WW-S", "G*WJ*SNY", "X*BRUXZM", "AGUMJR*Q", "MYTN*XIZ",
                                     "CZCIIXEL", "BNQPCMTF"// failed in a test?
                                    };

    for(size_t i = 0; i < dict.size(); i++) {
        // set and compress amino acid string
        for(size_t j = 0 ; j < 8; j ++) {
            f.set((unsigned char) j, hash[ dict[i][j] ]) ;
        }

        // decompress
        f.unpack();

        // verify
        for(size_t j = 0 ; j < 8; j ++) {
            BOOST_CHECK_EQUAL_MESSAGE(dict[i][j], f.data_decompressed[j], dict[i] + " => " + std::string(1, f.data_decompressed[0]) + std::string(1, f.data_decompressed[1]) + std::string(1, f.data_decompressed[2]) + std::string(1, f.data_decompressed[3]) + std::string(1, f.data_decompressed[4]) + std::string(1, f.data_decompressed[5]) + std::string(1, f.data_decompressed[6]) + std::string(1, f.data_decompressed[7]) + "   (" + std::to_string(j) + ": " + std::string(1, dict[i][j]) + " => " +  std::string(1, f.data_decompressed[j]) + ")");
        }

        /* printing for debugging purpose
        printf(" => [%u %u %u %u %u] => [%02hhX %02hhX %02hhX %02hhX %02hhX] => %c%c%c%c%c%c%c%c\n",

        f.data_compressed[0],f.data_compressed[1],f.data_compressed[2],f.data_compressed[3],f.data_compressed[4],
        f.data_compressed[0],f.data_compressed[1],f.data_compressed[2],f.data_compressed[3],f.data_compressed[4],

        f.data_decompressed[0],f.data_decompressed[1],f.data_decompressed[2],f.data_decompressed[3],f.data_decompressed[4],f.data_decompressed[5],f.data_decompressed[6],f.data_decompressed[7]
        );
        */
    }
}




BOOST_AUTO_TEST_CASE(test_bytes_rounding)
{
    BOOST_CHECK_EQUAL(fivebit_fivebytes::decompressed_to_compressed_bytes(0), 0); // 8 % 8 = 0
    BOOST_CHECK_EQUAL(fivebit_fivebytes::decompressed_to_compressed_bytes(1), 1);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::decompressed_to_compressed_bytes(2), 2);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::decompressed_to_compressed_bytes(3), 2);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::decompressed_to_compressed_bytes(4), 3);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::decompressed_to_compressed_bytes(5), 4);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::decompressed_to_compressed_bytes(6), 4);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::decompressed_to_compressed_bytes(7), 5);

    // 12345678 12345678 12345
    // 11111 22222 3333
    BOOST_CHECK_EQUAL(fivebit_fivebytes::nucleotides_to_compressed_offset(8), 5);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::nucleotides_to_compressed_offset(16), 10);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::nucleotides_to_compressed_offset(24), 15);

    BOOST_CHECK_EQUAL(fivebit_fivebytes::nucleotides_to_compressed_offset(21), 14);


    BOOST_CHECK_EQUAL(fivebit_fivebytes::nucleotides_to_compressed_offset(24 + 1), 15 + 1);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::nucleotides_to_compressed_offset(24 + 2), 15 + 2);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::nucleotides_to_compressed_offset(24 + 3), 15 + 2);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::nucleotides_to_compressed_offset(24 + 4), 15 + 3);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::nucleotides_to_compressed_offset(24 + 5), 15 + 4);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::nucleotides_to_compressed_offset(24 + 6), 15 + 4);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::nucleotides_to_compressed_offset(24 + 7), 15 + 5);
    BOOST_CHECK_EQUAL(fivebit_fivebytes::nucleotides_to_compressed_offset(24 + 8), 15 + 5); // full reset, back to /8
}



BOOST_AUTO_TEST_SUITE_END()
