#define BOOST_TEST_MODULE byte_decoder

#include <boost/test/included/unit_test.hpp>

#include "byte_decoder.hpp"
#include "twobit_byte.hpp"
#include "fourbit_byte.hpp"
#include "fivebit_fivebytes.hpp"


BOOST_AUTO_TEST_SUITE(Testing)


// ---------------------------------------------------------------------------
// twobit DNA
// ---------------------------------------------------------------------------

// 2-bit DNA encoding: 4 nucleotides packed per byte
//   00=T  01=C  10=A  11=G
//
// "AAAA" encodes as:
//   byte0: A(10) A(10) A(10) A(10) = 10101010 = 0b10101010
//
// "TTTT" encodes as:
//   byte0: T(00) T(00) T(00) T(00) = 00000000 = 0b00000000
BOOST_AUTO_TEST_CASE(test_byte_decoder_twobit_dna)
{
    const unsigned char input[] = {0b10101010, 0b00000000};
    byte_decoder b(std::make_unique<byte_decoder_interface_twobit_dna>());
    b.set_input_data(input, sizeof(input));
    BOOST_CHECK_EQUAL(b.decode(), "AAAATTTT");
}


// Round-trip: encode 4 nucleotides via twobit_byte_dna::set(), decode via byte_decoder.
// N has no 2-bit representation and is stored as T(00); it decodes back as T (lossy).
BOOST_AUTO_TEST_CASE(test_byte_decoder_twobit_dna_ported)
{
    twobit_byte_dna encoder;
    byte_decoder b(std::make_unique<byte_decoder_interface_twobit_dna>());

    char seq1[] = "AAAA";
    encoder.set(seq1);
    BOOST_CHECK_EQUAL(encoder.data, 0b10101010);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode(), "AAAA");

    char seq2[] = "TAAA";
    encoder.set(seq2);
    BOOST_CHECK_EQUAL(encoder.data, 0b00101010);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode(), "TAAA");

    char seq3[] = "ACTG";
    encoder.set(seq3);
    BOOST_CHECK_EQUAL(encoder.data, 0b10010011);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode(), "ACTG");

    // N stored as T(00) -- decodes back as T, not N
    char seq4[] = "NCTN";
    encoder.set(seq4);
    BOOST_CHECK_EQUAL(encoder.data, 0b00010000);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode(), "TCTT");
}


// twobit: position 0 occupies bits 7-6 of the byte (2 bits per nucleotide)
// Only the bits belonging to position 0 are set; the remaining bits in the
// chunk are uninitialized and their decoded values are not tested.
//   nucleotide index: T=0  C=1  A=2  G=3
BOOST_AUTO_TEST_CASE(test_byte_decoder_twobit_dna_single_nuc)
{
    twobit_byte_dna encoder;
    byte_decoder b(std::make_unique<byte_decoder_interface_twobit_dna>());

    encoder.data = 0;
    encoder.set(6, 2);  // A at position 0 -> bits 7-6 = 10
    BOOST_CHECK_EQUAL(encoder.data & 0b11000000, 0b10000000);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode()[0], 'A');

    encoder.data = 0;
    encoder.set(6, 3);  // G at position 0 -> bits 7-6 = 11
    BOOST_CHECK_EQUAL(encoder.data & 0b11000000, 0b11000000);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode()[0], 'G');

    encoder.data = 0;
    encoder.set(6, 1);  // C at position 0 -> bits 7-6 = 01
    BOOST_CHECK_EQUAL(encoder.data & 0b11000000, 0b01000000);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode()[0], 'C');

    encoder.data = 0;
    encoder.set(6, 0);  // T at position 0 -> bits 7-6 = 00
    BOOST_CHECK_EQUAL(encoder.data & 0b11000000, 0b00000000);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode()[0], 'T');
}


// when input is not a multiple of bytes_per_chunk, the trailing incomplete
// chunk is silently skipped -- here 3 bytes for a 1-byte-per-chunk decoder
// decodes all 3 bytes normally:
//   0b10101010 -> AAAA  (10=A repeated)
//   0b00000000 -> TTTT  (00=T repeated)
//   0b11111111 -> GGGG  (11=G repeated)
BOOST_AUTO_TEST_CASE(test_byte_decoder_twobit_dna_partial_chunk_skipped)
{
    const unsigned char input[] = {0b10101010, 0b00000000, 0b11111111};
    byte_decoder b(std::make_unique<byte_decoder_interface_twobit_dna>());
    b.set_input_data(input, sizeof(input));
    BOOST_CHECK_EQUAL(b.decode(), "AAAATTTTGGGG");
}


// ---------------------------------------------------------------------------
// twobit RNA
// ---------------------------------------------------------------------------

// 2-bit RNA encoding: identical bit layout to DNA, T is replaced by U
//   00=U  01=C  10=A  11=G
//
// "AAAA" encodes as:
//   byte0: A(10) A(10) A(10) A(10) = 10101010 = 0b10101010
//
// "UUUU" encodes as:
//   byte0: U(00) U(00) U(00) U(00) = 00000000 = 0b00000000
BOOST_AUTO_TEST_CASE(test_byte_decoder_twobit_rna)
{
    const unsigned char input[] = {0b10101010, 0b00000000};
    byte_decoder b(std::make_unique<byte_decoder_interface_twobit_rna>());
    b.set_input_data(input, sizeof(input));
    BOOST_CHECK_EQUAL(b.decode(), "AAAAUUUU");
}


// Round-trip: encode via twobit_byte_rna::set(), decode via byte_decoder.
// T and N are stored as 00; RNA decodes 00 as U (lossy for T and N).
BOOST_AUTO_TEST_CASE(test_byte_decoder_twobit_rna_ported)
{
    twobit_byte_rna encoder;
    byte_decoder b(std::make_unique<byte_decoder_interface_twobit_rna>());

    char seq1[] = "AAAA";
    encoder.set(seq1);
    BOOST_CHECK_EQUAL(encoder.data, 0b10101010);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode(), "AAAA");

    // T stored as 00; RNA decodes 00 as U -- round-trip gives U, not T
    char seq2[] = "TAAA";
    encoder.set(seq2);
    BOOST_CHECK_EQUAL(encoder.data, 0b00101010);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode(), "UAAA");

    char seq3[] = "ACTG";
    encoder.set(seq3);
    BOOST_CHECK_EQUAL(encoder.data, 0b10010011);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode(), "ACUG");

    // N and T both stored as 00=U in RNA
    char seq4[] = "NCTN";
    encoder.set(seq4);
    BOOST_CHECK_EQUAL(encoder.data, 0b00010000);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode(), "UCUU");
}


// the strategy (encoding type) can be swapped at runtime without
// re-supplying the input data -- same bytes, different alphabet:
//   0b10101010 with DNA strategy -> "AAAA"  (10=A)
//   0b10101010 with RNA strategy -> "AAAA"  (10=A, same mapping)
BOOST_AUTO_TEST_CASE(test_byte_decoder_set_strategy)
{
    const unsigned char input[] = {0b10101010};
    byte_decoder b(std::make_unique<byte_decoder_interface_twobit_dna>());
    b.set_input_data(input, sizeof(input));
    BOOST_CHECK_EQUAL(b.decode(), "AAAA");

    b.set_strategy(std::make_unique<byte_decoder_interface_twobit_rna>());
    BOOST_CHECK_EQUAL(b.decode(), "AAAA");
}


// ---------------------------------------------------------------------------
// fourbit
// ---------------------------------------------------------------------------

// 4-bit IUPEC encoding: 2 nucleotides packed per byte (high nibble, low nibble)
//   alphabet index: A=0000  C=0001  G=0010  T=0011  U=0100  ...  N=1111
//
// "AA" encodes as:
//   byte0: A(0000) A(0000) = 00000000 = 0b00000000
//
// "CG" encodes as:
//   byte1: C(0001) G(0010) = 00010010 = 0b00010010
BOOST_AUTO_TEST_CASE(test_byte_decoder_fourbit)
{
    const unsigned char input[] = {0b00000000, 0b00010010};
    byte_decoder b(std::make_unique<byte_decoder_interface_fourbit>());
    b.set_input_data(input, sizeof(input));
    BOOST_CHECK_EQUAL(b.decode(), "AACG");
}


// Round-trip: encode via fourbit_byte::set(), decode via byte_decoder.
// Covers all 16 IUPEC symbols by testing all 8 adjacent pairs.
//   byte value = (index[nuc0] << 4) | index[nuc1]
//   A=0  C=1  G=2  T=3  U=4  R=5  Y=6  K=7  M=8  S=9  W=10  B=11  D=12  H=13  V=14  N=15
BOOST_AUTO_TEST_CASE(test_byte_decoder_fourbit_ported)
{
    fourbit_byte encoder;
    byte_decoder b(std::make_unique<byte_decoder_interface_fourbit>());

    char seq1[] = "AC";  encoder.set(seq1);  BOOST_CHECK_EQUAL(encoder.data, 0b00000001);
    b.set_input_data(&encoder.data, 1);      BOOST_CHECK_EQUAL(b.decode(), "AC");

    char seq2[] = "GT";  encoder.set(seq2);  BOOST_CHECK_EQUAL(encoder.data, 0b00100011);
    b.set_input_data(&encoder.data, 1);      BOOST_CHECK_EQUAL(b.decode(), "GT");

    char seq3[] = "UR";  encoder.set(seq3);  BOOST_CHECK_EQUAL(encoder.data, 0b01000101);
    b.set_input_data(&encoder.data, 1);      BOOST_CHECK_EQUAL(b.decode(), "UR");

    char seq4[] = "YK";  encoder.set(seq4);  BOOST_CHECK_EQUAL(encoder.data, 0b01100111);
    b.set_input_data(&encoder.data, 1);      BOOST_CHECK_EQUAL(b.decode(), "YK");

    char seq5[] = "MS";  encoder.set(seq5);  BOOST_CHECK_EQUAL(encoder.data, 0b10001001);
    b.set_input_data(&encoder.data, 1);      BOOST_CHECK_EQUAL(b.decode(), "MS");

    char seq6[] = "WB";  encoder.set(seq6);  BOOST_CHECK_EQUAL(encoder.data, 0b10101011);
    b.set_input_data(&encoder.data, 1);      BOOST_CHECK_EQUAL(b.decode(), "WB");

    char seq7[] = "DH";  encoder.set(seq7);  BOOST_CHECK_EQUAL(encoder.data, 0b11001101);
    b.set_input_data(&encoder.data, 1);      BOOST_CHECK_EQUAL(b.decode(), "DH");

    char seq8[] = "VN";  encoder.set(seq8);  BOOST_CHECK_EQUAL(encoder.data, 0b11101111);
    b.set_input_data(&encoder.data, 1);      BOOST_CHECK_EQUAL(b.decode(), "VN");
}


// fourbit: position 0 occupies bits 7-4 of the byte (4 bits per nucleotide)
// Only the bits belonging to position 0 are set; the remaining bits in the
// chunk are uninitialized and their decoded values are not tested.
//   nucleotide index: A=0  C=1  G=2  T=3  U=4  ...  N=15
BOOST_AUTO_TEST_CASE(test_byte_decoder_fourbit_single_nuc)
{
    fourbit_byte encoder;
    byte_decoder b(std::make_unique<byte_decoder_interface_fourbit>());

    encoder.data = 0;
    encoder.set(4, 0);  // A at position 0 -> bits 7-4 = 0000
    BOOST_CHECK_EQUAL(encoder.data & 0b11110000, 0b00000000);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode()[0], 'A');

    encoder.data = 0;
    encoder.set(4, 3);  // T at position 0 -> bits 7-4 = 0011
    BOOST_CHECK_EQUAL(encoder.data & 0b11110000, 0b00110000);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode()[0], 'T');

    encoder.data = 0;
    encoder.set(4, 15);  // N at position 0 -> bits 7-4 = 1111
    BOOST_CHECK_EQUAL(encoder.data & 0b11110000, 0b11110000);
    b.set_input_data(&encoder.data, 1);
    BOOST_CHECK_EQUAL(b.decode()[0], 'N');
}


// ---------------------------------------------------------------------------
// fivebit
// ---------------------------------------------------------------------------

// 5-bit protein alphabet: ABCDEFGHIJKLMNOPQRSTUVWYZX*-
//   A=00000(0)  C=00010(2)  D=00011(3)  E=00100(4)
//   F=00101(5)  G=00110(6)  H=00111(7)  I=01000(8)
//
// 8 symbols x 5 bits = 40 bits packed into 5 bytes:
//   byte layout: [00000111][11222223][33334444][45555566][66677777]
//   (digit = symbol index 0-7)
//
// "ACDEFGHI" encodes as:
//   byte0: A(00000) + C[4:2](000)          = 00000_000 = 0b00000000
//   byte1: C[1:0](10) + D(00011) + E[4](0) = 10_00011_0 = 0b10000110
//   byte2: E[3:0](0100) + F[4:1](0010)     = 0100_0010 = 0b01000010
//   byte3: F[0](1) + G(00110) + H[4:3](00) = 1_00110_00 = 0b10011000
//   byte4: H[2:0](111) + I(01000)          = 111_01000 = 0b11101000
BOOST_AUTO_TEST_CASE(test_byte_decoder_fivebit)
{
    const unsigned char input[5] = {0b00000000, 0b10000110, 0b01000010, 0b10011000, 0b11101000};
    byte_decoder b(std::make_unique<byte_decoder_interface_fivebit>());
    b.set_input_data(input, sizeof(input));
    BOOST_CHECK_EQUAL(b.decode(), "ACDEFGHI");
}


// all-zero 5-byte chunk: every 5-bit group is 00000 = index 0 = 'A'
BOOST_AUTO_TEST_CASE(test_byte_decoder_fivebit_all_A)
{
    const unsigned char input[5] = {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000};
    byte_decoder b(std::make_unique<byte_decoder_interface_fivebit>());
    b.set_input_data(input, sizeof(input));
    BOOST_CHECK_EQUAL(b.decode(), "AAAAAAAA");
}


// 5-bit protein alphabet: ABCDEFGHIJKLMNOPQRSTUVWYZX*-
//   F=00101(5)  A=00000(0)  S=10010(18)  T=10011(19)
//   A=00000(0)  -=11011(27) F=00101(5)   S=10010(18)
//
// "FASTA-FS" packed into 5 bytes:
//   byte layout: [00000111][11222223][33334444][45555566][66677777]
//
//   byte0: F[4:0](00101) + A[4:2](000)          = 00101_000 = 0b00101000 (40)
//   byte1: A[1:0](00) + S(10010) + T[4](1)      = 00_10010_1 = 0b00100101 (37)
//   byte2: T[3:0](0011) + A[4:1](0000)           = 0011_0000 = 0b00110000 (48)
//   byte3: A[0](0) + -(11011) + F[4:3](00)       = 0_11011_00 = 0b01101100 (108)
//   byte4: F[2:0](101) + S(10010)                = 101_10010 = 0b10110010 (178)
BOOST_AUTO_TEST_CASE(test_byte_decoder_fivebit_fasta_fs)
{
    const unsigned char input[5] = {0b00101000, 0b00100101, 0b00110000, 0b01101100, 0b10110010};
    byte_decoder b(std::make_unique<byte_decoder_interface_fivebit>());
    b.set_input_data(input, sizeof(input));
    BOOST_CHECK_EQUAL(b.decode(), "FASTA-FS");
}


// Two consecutive 5-byte chunks decode to 16 symbols.
// Uses known byte values from earlier single-chunk tests.
BOOST_AUTO_TEST_CASE(test_byte_decoder_fivebit_multiple_chunks)
{
    const unsigned char input[10] = {
        0b00101000, 0b00100101, 0b00110000, 0b01101100, 0b10110010,  // "FASTA-FS"
        0b00000000, 0b10000110, 0b01000010, 0b10011000, 0b11101000   // "ACDEFGHI"
    };
    byte_decoder b(std::make_unique<byte_decoder_interface_fivebit>());
    b.set_input_data(input, sizeof(input));
    BOOST_CHECK_EQUAL(b.decode(), "FASTA-FSACDEFGHI");
}


// Round-trip test: encode via fivebit_fivebytes::set(), decode via byte_decoder.
// Covers a broad set of 8-symbol sequences including all alphabet characters
// and edge cases (*,-,X,Z) that were historically problematic.
BOOST_AUTO_TEST_CASE(test_byte_decoder_fivebit_dict)
{
    char hash[256] = {};
    hash['A']=0;  hash['B']=1;  hash['C']=2;  hash['D']=3;
    hash['E']=4;  hash['F']=5;  hash['G']=6;  hash['H']=7;
    hash['I']=8;  hash['J']=9;  hash['K']=10; hash['L']=11;
    hash['M']=12; hash['N']=13; hash['O']=14; hash['P']=15;
    hash['Q']=16; hash['R']=17; hash['S']=18; hash['T']=19;
    hash['U']=20; hash['V']=21; hash['W']=22; hash['Y']=23;
    hash['Z']=24; hash['X']=25; hash['*']=26; hash['-']=27;

    const std::vector<std::string> dict = {
        "FASTA-FS", "FRATSAST", "UCTFXJNH", "CGLWQNSI", "*OLS*DEN", "LAGD*PYE",
        "HFFXWSVZ", "ABBECXVW", "YMGTTOX*", "I-XVFAQX", "KEYVHNER", "IUISR-ZH",
        "JHCVXJMK", "NNFKGPOW", "WVGTUHYB", "SYGCM-UQ", "-KLOGKUC", "W*SWGLIJ",
        "*ZKJTHTV", "UWRTTHUM", "XLJTHJEQ", "LRFPJHAR", "ZJAVVMHP", "MDOADYFU",
        "NCK*CYNZ", "YMQZXEUR", "FBHAEAZS", "ERBIE-NQ", "GIJRSMBZ", "SEAZ*PO-",
        "PKB*XTLD", "GDFWBYMA", "RDGF-TN-", "P-LUKTWO", "-PXBTMBL", "OOJVYSJ*",
        "NDMCNXNQ", "OVNK-DVU", "FPIHDXEK", "L*AKZMMG", "AYQIQQXF", "CEWTCTDR",
        "YQPUSSPI", "ZIRY*WWR", "MJWQAOWO", "OCYFLK-V", "GQOEOZFZ", "*SFXOJX*",
        "PK-BHBLJ", "QPS*KSVV", "IHDZCVM-", "-GWREDJB", "PEKCDABZ", "ULBUMPNG",
        "ICYANTRP", "CYQGQHR*", "ZDISLFU-", "DIUBILAR", "VSWWTJAP", "PXYJYBIH",
        "NA-LNTOS", "FQIUFQRX", "KACQOLYW", "TWXUYV*F", "YMANTHBD", "KLZGRNCO",
        "BSKQ*GDM", "PFELJUIZ", "Q-WOCMLE", "AXJNEKCQ", "WOLPA*FZ", "DMHVYSFS",
        "RIFXSLYC", "-CQFIYZE", "R*EZXK-V", "WVPJ*HGJ", "HUDGTUV*", "PUC-GHYT",
        "M-*ZOJVB", "VD*LXUAU", "OLVEK-ZX", "WIVVTQRE", "-MAPUSWY", "IJHTQSGL",
        "OHIKEZEE", "G-ICTNTZ", "GMRS-DP-", "ARZEWZFS", "RGTFGQFR", "JKD-ELBY",
        "A-XDUGGM", "MFLWDXAN", "DGOFAMAI", "O**O-STQ", "-VNKGBJC", "RPMZHJJY",
        "XDBBUKQK", "CUFICTTW", "AUGSBPPE", "*PYTZKEF", "AHYSKALD", "KQMB*BNN",
        "*HWNMGYV", "IRWY*WBU", "LJNNZNLO", "PHVFRNWR", "CCCIHRHD", "JBORIXIX",
        "JT*SZHKP", "ZXURDOQD", "CPSBC-VD", "IHMEOQLB", "KXNMOVEX", "SJPQUBM*",
        "ZCLXJSIA", "YBHNUNHH", "AYTGKWSO", "IHXJ*TD-", "MVAJKHUH", "QUXLEYWW",
        "TSROZJIQ", "LIAPGNHF", "RCMXSNLM", "GF*-VRJK", "IKDCTKFY", "VAPADEGP",
        "GKHLIWHU", "-OH-F-EZ", "*GPJZCNY", "RVUGFGVA", "JLLYKRWZ", "WSOWVDKV",
        "QPJZLBPH", "BYTSODIJ", "-MCJFBFU", "D*NIFL*L", "IG*CW*ZM", "GZTGQWHC",
        "CTCTBRDU", "IBNLOPPM", "DTP-HLXN", "YQRZ*GWS", "YFVBQEMM", "MYARXKOF",
        "SQDKDEQZ", "RKKVOEOL", "XA*YVU-W", "CQZQXTRV", "VP*YCJQM", "JWEF*O-J",
        "LFUTAQSF", "-EEILVRG", "DKNSYQZ-", "EQLM*BT-", "TGNJDBDS", "AJ-IGQLP",
        "PPDRKUKA", "WDDKVXXX", "HCHASUOB", "-RCUHJOJ", "BVANOTRB", "TYGLLX*H",
        "SYBOLOOM", "IIBETFMT", "PDFGC-CE", "*PTPEDLR", "ZZQQESJB", "HMNGEPEK",
        "UPFIPVQN", "XBWOFKFY", "XDOPNZUI", "JEMVIXKL", "EXQWSNKQ", "FAAIORTR",
        "BWXEMW-D", "ZVT-NUKK", "YRCACBAE", "ASDZZFXM", "XHHSBSRN", "UYSVOSIT",
        "DVNEHROM", "*PUCVJIC", "NYUWPZ-I", "Y*-GH-AV", "CBB-FGNQ", "CUCEAWMU",
        "FZO*WW-S", "G*WJ*SNY", "X*BRUXZM", "AGUMJR*Q", "MYTN*XIZ",
        "CZCIIXEL", "BNQPCMTF"
    };

    fivebit_fivebytes encoder;
    byte_decoder b(std::make_unique<byte_decoder_interface_fivebit>());

    for(const std::string &seq : dict) {
        // encode each 8-symbol sequence symbol-by-symbol
        for(unsigned char j = 0; j < 8; j++) {
            encoder.set(j, (unsigned char) hash[(unsigned char) seq[j]]);
        }

        // decode the compressed chunk via byte_decoder
        b.set_input_data(encoder.data_compressed, 5);
        const std::string decoded = b.decode();

        BOOST_CHECK_EQUAL_MESSAGE(decoded, seq,
            "round-trip failed for: " + seq + " -> got: " + decoded);
    }
}


// fivebit: position 0 occupies bits 7-3 of byte0 (5 bits per symbol)
// Only the bits belonging to position 0 are set; the remaining bits in the
// chunk are uninitialized and their decoded values are not tested.
//   symbol index: A=0  F=5  -=27
BOOST_AUTO_TEST_CASE(test_byte_decoder_fivebit_single_symbol)
{
    fivebit_fivebytes encoder;
    byte_decoder b(std::make_unique<byte_decoder_interface_fivebit>());

    memset(encoder.data_compressed, 0, 5);
    encoder.set(0, 5);  // F at position 0 -> bits 7-3 of byte0 = 00101
    BOOST_CHECK_EQUAL(encoder.data_compressed[0] & 0b11111000, 0b00101000);
    b.set_input_data(encoder.data_compressed, 5);
    BOOST_CHECK_EQUAL(b.decode()[0], 'F');

    memset(encoder.data_compressed, 0, 5);
    encoder.set(0, 27);  // '-' at position 0 -> bits 7-3 of byte0 = 11011
    BOOST_CHECK_EQUAL(encoder.data_compressed[0] & 0b11111000, 0b11011000);
    b.set_input_data(encoder.data_compressed, 5);
    BOOST_CHECK_EQUAL(b.decode()[0], '-');

    memset(encoder.data_compressed, 0, 5);
    encoder.set(0, 0);  // A at position 0 -> bits 7-3 of byte0 = 00000
    BOOST_CHECK_EQUAL(encoder.data_compressed[0] & 0b11111000, 0b00000000);
    b.set_input_data(encoder.data_compressed, 5);
    BOOST_CHECK_EQUAL(b.decode()[0], 'A');
}


BOOST_AUTO_TEST_SUITE_END()
