#include <iostream>
#include <fstream>

#include "config.hpp"

#include "fourbit_byte.hpp"


/*
 alphabet = ACGTURYKMSWBDHVN

binary:		IUPEC

00000000	AA
00000001	AC
00000010	AG
...
00000010	NH
11111110	NV
11111111	NN
 */

const char fourbit_byte::fourbit_alhpabet[17] = "ACGTURYKMSWBDHVN";
const char fourbit_byte::fourbit_hash[256][3] = {"AA", "AC", "AG", "AT", "AU", "AR", "AY", "AK", "AM", "AS", "AW", "AB", "AD", "AH", "AV", "AN", "CA", "CC", "CG", "CT", "CU", "CR", "CY", "CK", "CM", "CS", "CW", "CB", "CD", "CH", "CV", "CN", "GA", "GC", "GG", "GT", "GU", "GR", "GY", "GK", "GM", "GS", "GW", "GB", "GD", "GH", "GV", "GN", "TA", "TC", "TG", "TT", "TU", "TR", "TY", "TK", "TM", "TS", "TW", "TB", "TD", "TH", "TV", "TN", "UA", "UC", "UG", "UT", "UU", "UR", "UY", "UK", "UM", "US", "UW", "UB", "UD", "UH", "UV", "UN", "RA", "RC", "RG", "RT", "RU", "RR", "RY", "RK", "RM", "RS", "RW", "RB", "RD", "RH", "RV", "RN", "YA", "YC", "YG", "YT", "YU", "YR", "YY", "YK", "YM", "YS", "YW", "YB", "YD", "YH", "YV", "YN", "KA", "KC", "KG", "KT", "KU", "KR", "KY", "KK", "KM", "KS", "KW", "KB", "KD", "KH", "KV", "KN", "MA", "MC", "MG", "MT", "MU", "MR", "MY", "MK", "MM", "MS", "MW", "MB", "MD", "MH", "MV", "MN", "SA", "SC", "SG", "ST", "SU", "SR", "SY", "SK", "SM", "SS", "SW", "SB", "SD", "SH", "SV", "SN", "WA", "WC", "WG", "WT", "WU", "WR", "WY", "WK", "WM", "WS", "WW", "WB", "WD", "WH", "WV", "WN", "BA", "BC", "BG", "BT", "BU", "BR", "BY", "BK", "BM", "BS", "BW", "BB", "BD", "BH", "BV", "BN", "DA", "DC", "DG", "DT", "DU", "DR", "DY", "DK", "DM", "DS", "DW", "DB", "DD", "DH", "DV", "DN", "HA", "HC", "HG", "HT", "HU", "HR", "HY", "HK", "HM", "HS", "HW", "HB", "HD", "HH", "HV", "HN", "VA", "VC", "VG", "VT", "VU", "VR", "VY", "VK", "VM", "VS", "VW", "VB", "VD", "VH", "VV", "VN", "NA", "NC", "NG", "NT", "NU", "NR", "NY", "NK", "NM", "NS", "NW", "NB", "ND", "NH", "NV", "NN"};


/*
 input \t output
 0        6
 1        4
 2        2
 3        0

 4        6
 5        4
 6        2
 7        0

not sure what the quickest way is - this way all calculations are done as ints, not as chars


 */
unsigned char fourbit_byte::iterator_to_offset(uint32_t iterator)
{
    return (unsigned char)((3 - (iterator % 4)) * 2);
}

// @todo, offset needs to be second parameter
void fourbit_byte::set(unsigned char bit_offset, unsigned char nucleotide)
{
    // bit_offset must be: {0, or 4}; -> location in bits
    // nucleotides must be:
    // => T - 00, C - 01, A - 10, G - 11
    // => T - 00, C -  1, A -  2, G -  3
#if DEBUG
    if(bit_offset != 0 and bit_offset != 4) {
        throw std::invalid_argument("fourbit_byte(bit_offset, ..) must be 0 or 4\n");
    }
#endif //DEBUG
    //set   bit(s): INPUT |= 1 << N;
    //unset bit(s): INPUT &= ~(1 << N);

    switch(nucleotide) {
    case 0:// A (0000)
        this->data = (unsigned char)(this->data & ~( (8+4+2+1) << bit_offset)); // set zero's
        break;
    case 1:// C (0001)
        this->data = (unsigned char)(this->data & ~( (8+4+2  ) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (      1) << bit_offset)); // set one's
        break;
    case 2:// G (0010)
        this->data = (unsigned char)(this->data & ~( (8+4  +1) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (    2  ) << bit_offset)); // set one's
        break;
    case 3:// T (0011)
        this->data = (unsigned char)(this->data & ~( (8+4    ) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (    2+1) << bit_offset)); // set one's
        break;
	case 4:// U (0100)
        this->data = (unsigned char)(this->data & ~( (8  +2+1) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (  4    ) << bit_offset)); // set one's
		break;
	case 5:// R (0101)
        this->data = (unsigned char)(this->data & ~( (8  +2  ) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (  4  +1) << bit_offset)); // set one's
		break;
	case 6:// Y (0110)
        this->data = (unsigned char)(this->data & ~( (8    +1) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (  4+2  ) << bit_offset)); // set one's
		break;
	case 7:// K (0111)
        this->data = (unsigned char)(this->data & ~( (8      ) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (  4+2+1) << bit_offset)); // set one's
		break;
	case 8:// M (1000)
        this->data = (unsigned char)(this->data & ~( (  4+2+1) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (8      ) << bit_offset)); // set one's
		break;
	case 9:// S (1001)
        this->data = (unsigned char)(this->data & ~( (  4+2  ) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (8    +1) << bit_offset)); // set one's
		break;
	case 10:// W (1010)
        this->data = (unsigned char)(this->data & ~( (  4  +1) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (8  +2  ) << bit_offset)); // set one's
		break;
	case 11:// B (1011)
        this->data = (unsigned char)(this->data & ~( (  4    ) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (8  +2+1) << bit_offset)); // set one's
		break;
	case 12:// D (1100)
        this->data = (unsigned char)(this->data & ~( (    2+1) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (8+4    ) << bit_offset)); // set one's
		break;
	case 13:// H (1101)
        this->data = (unsigned char)(this->data & ~( (    2  ) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (8+4  +1) << bit_offset)); // set one's
		break;
	case 14:// V (1110)
        this->data = (unsigned char)(this->data & ~( (     +1) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data |  ( (8+4+2  ) << bit_offset)); // set one's
		break;
	case 15:// N (1111)
        this->data = (unsigned char)(this->data |  ( (8+4+2+1) << bit_offset)); // set one's
		break;

#if DEBUG
    default:
        throw std::invalid_argument("fourbit_byte::set(pos, nucleotide) invalid value\n");
        break;
#endif //DEBUG
    }
};


// input char "AACCCTTGG"
// N's are treated as 0, for some weird reason
void fourbit_byte::set(char* buffer)
{
    const std::array< unsigned char, 4> bit_offsets = {4, 0};
    for(unsigned char i = 0; i < 2; i++) {
        switch(buffer[i]) {

			case 'A':// A (0000)
			case 'a':
				this->set(bit_offsets[i], 0);
				break;
			case 'C':// C (0001)
			case 'c':
				this->set(bit_offsets[i], 1);
				break;
			case 'G':// G (0010)
			case 'g':
				this->set(bit_offsets[i], 2);
				break;
			case 'T':// T (0011)
			case 't':
				this->set(bit_offsets[i], 3);
				break;
			case 'U':// U (0100)
			case 'u':
				this->set(bit_offsets[i], 4);
				break;
			case 'R':// R (0101)
			case 'r':
				this->set(bit_offsets[i], 5);
				break;
			case 'Y':// Y (0110)
			case 'y':
				this->set(bit_offsets[i], 6);
				break;
			case 'K':// K (0111)
			case 'k':
				this->set(bit_offsets[i], 7);
				break;
			case 'M':// M (1000)
			case 'm':
				this->set(bit_offsets[i], 8);
				break;
			case 'S':// S (1001)
			case 's':
				this->set(bit_offsets[i], 9);
				break;
			case 'W':// W (1010)
			case 'w':
				this->set(bit_offsets[i], 10);
				break;
			case 'B':// B (1011)
			case 'b':
				this->set(bit_offsets[i], 11);
				break;
			case 'D':// D (1100)
			case 'd':
				this->set(bit_offsets[i], 12);
				break;
			case 'H':// H (1101)
			case 'h':
				this->set(bit_offsets[i], 13);
				break;
			case 'V':// V (1110)
			case 'v':
				this->set(bit_offsets[i], 14);
				break;
			case 'N':// N (1111)
			case 'n':
				this->set(bit_offsets[i], 15);
				break;

#if DEBUG
			default:
				throw std::invalid_argument("fourbit_byte::set(char *) invalid value\n");
				break;
#endif //DEBUG
        }
    }
}

/**
 * @brief fully decodes a fourbit byte, not referencing to a hash but allocating a new char*,
 * slower than fourbit_byte::get(void) but capable of determining very ends
**/
char *fourbit_byte::get(unsigned char length)
{
#if DEBUG
	if(length > 2) {
		throw std::invalid_argument("four_byte::get(unsigned char length) -> out of bound: " + std::to_string(length)+ "\n");
	}
#endif //DEBUG

    char *seq = new char[length + 1];
    for(unsigned char i = 0; i < length; i++) { // length = 4: i = 0, 1, 2, 3
        seq[i] = fourbit_byte::fourbit_hash[this->data][i];
    }
    seq[length] = '\0';
    return seq;
}



const char *fourbit_byte::get()
{
    return fourbit_byte::fourbit_hash[this->data];
}
