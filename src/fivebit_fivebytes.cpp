#include <iostream>
#include <fstream>

#include "config.hpp"

#include "fivebit_fivebytes.hpp"

/*
alphabet = ABCDEFGHIJKLMNOPQRSTUVWYZX*-

Gaps can be efficiently included

In five bytes we can store nine 5bits:

[00000111]
[11222223]
[33334444]
[46666677]
[77788888]

idx: https://stackoverflow.com/questions/11509415/character-array-as-a-value-in-c-map

A = 00000 (0)
B = 00001 (1)
C = 00010 (2)
...
Z = 11000 (24)
X = 11001 (25)
* = 11010 (26)
- = 11011 (27)
*/


const char fivebit_byte::fivebit_alhpabet[28] = "ABCDEFGHIJKLMNOPQRSTUVWYZX*-";
//const char fourbit_byte::encode_hash[256][3] = {"AA", "AC", "AG", "AT", "AU", "AR", "AY", "AK", "AM", "AS", "AW", "AB", "AD", "AH", "AV", "AN", "CA", "CC", "CG", "CT", "CU", "CR", "CY", "CK", "CM", "CS", "CW", "CB", "CD", "CH", "CV", "CN", "GA", "GC", "GG", "GT", "GU", "GR", "GY", "GK", "GM", "GS", "GW", "GB", "GD", "GH", "GV", "GN", "TA", "TC", "TG", "TT", "TU", "TR", "TY", "TK", "TM", "TS", "TW", "TB", "TD", "TH", "TV", "TN", "UA", "UC", "UG", "UT", "UU", "UR", "UY", "UK", "UM", "US", "UW", "UB", "UD", "UH", "UV", "UN", "RA", "RC", "RG", "RT", "RU", "RR", "RY", "RK", "RM", "RS", "RW", "RB", "RD", "RH", "RV", "RN", "YA", "YC", "YG", "YT", "YU", "YR", "YY", "YK", "YM", "YS", "YW", "YB", "YD", "YH", "YV", "YN", "KA", "KC", "KG", "KT", "KU", "KR", "KY", "KK", "KM", "KS", "KW", "KB", "KD", "KH", "KV", "KN", "MA", "MC", "MG", "MT", "MU", "MR", "MY", "MK", "MM", "MS", "MW", "MB", "MD", "MH", "MV", "MN", "SA", "SC", "SG", "ST", "SU", "SR", "SY", "SK", "SM", "SS", "SW", "SB", "SD", "SH", "SV", "SN", "WA", "WC", "WG", "WT", "WU", "WR", "WY", "WK", "WM", "WS", "WW", "WB", "WD", "WH", "WV", "WN", "BA", "BC", "BG", "BT", "BU", "BR", "BY", "BK", "BM", "BS", "BW", "BB", "BD", "BH", "BV", "BN", "DA", "DC", "DG", "DT", "DU", "DR", "DY", "DK", "DM", "DS", "DW", "DB", "DD", "DH", "DV", "DN", "HA", "HC", "HG", "HT", "HU", "HR", "HY", "HK", "HM", "HS", "HW", "HB", "HD", "HH", "HV", "HN", "VA", "VC", "VG", "VT", "VU", "VR", "VY", "VK", "VM", "VS", "VW", "VB", "VD", "VH", "VV", "VN", "NA", "NC", "NG", "NT", "NU", "NR", "NY", "NK", "NM", "NS", "NW", "NB", "ND", "NH", "NV", "NN"};


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
        this->data = (unsigned char)(this->data & ~((8 + 4 + 2 + 1) << bit_offset)); // set zero's
        break;
    case 1:// C (0001)
        this->data = (unsigned char)(this->data & ~((8 + 4 + 2) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data | ((1) << bit_offset));         // set one's
        break;
    case 2:// G (0010)
        this->data = (unsigned char)(this->data & ~((8 + 4  + 1) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data | ((2) << bit_offset));         // set one's
        break;
    case 3:// T (0011)
        this->data = (unsigned char)(this->data & ~((8 + 4) << bit_offset));    // set zero's
        this->data = (unsigned char)(this->data | ((2 + 1) << bit_offset));     // set one's
        break;
    case 4:// U (0100)
        this->data = (unsigned char)(this->data & ~((8  + 2 + 1) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data | ((4) << bit_offset));         // set one's
        break;
    case 5:// R (0101)
        this->data = (unsigned char)(this->data & ~((8  + 2) << bit_offset));   // set zero's
        this->data = (unsigned char)(this->data | ((4  + 1) << bit_offset));    // set one's
        break;
    case 6:// Y (0110)
        this->data = (unsigned char)(this->data & ~((8    + 1) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data | ((4 + 2) << bit_offset));     // set one's
        break;
    case 7:// K (0111)
        this->data = (unsigned char)(this->data & ~((8) << bit_offset));        // set zero's
        this->data = (unsigned char)(this->data | ((4 + 2 + 1) << bit_offset)); // set one's
        break;
    case 8:// M (1000)
        this->data = (unsigned char)(this->data & ~((4 + 2 + 1) << bit_offset)); // set zero's
        this->data = (unsigned char)(this->data | ((8) << bit_offset));         // set one's
        break;
    case 9:// S (1001)
        this->data = (unsigned char)(this->data & ~((4 + 2) << bit_offset));    // set zero's
        this->data = (unsigned char)(this->data | ((8    + 1) << bit_offset));  // set one's
        break;
    case 10:// W (1010)
        this->data = (unsigned char)(this->data & ~((4  + 1) << bit_offset));   // set zero's
        this->data = (unsigned char)(this->data | ((8  + 2) << bit_offset));    // set one's
        break;
    case 11:// B (1011)
        this->data = (unsigned char)(this->data & ~((4) << bit_offset));        // set zero's
        this->data = (unsigned char)(this->data | ((8  + 2 + 1) << bit_offset)); // set one's
        break;
    case 12:// D (1100)
        this->data = (unsigned char)(this->data & ~((2 + 1) << bit_offset));    // set zero's
        this->data = (unsigned char)(this->data | ((8 + 4) << bit_offset));     // set one's
        break;
    case 13:// H (1101)
        this->data = (unsigned char)(this->data & ~((2) << bit_offset));        // set zero's
        this->data = (unsigned char)(this->data | ((8 + 4  + 1) << bit_offset)); // set one's
        break;
    case 14:// V (1110)
        this->data = (unsigned char)(this->data & ~((+1) << bit_offset));       // set zero's
        this->data = (unsigned char)(this->data | ((8 + 4 + 2) << bit_offset)); // set one's
        break;
    case 15:// N (1111)
        this->data = (unsigned char)(this->data | ((8 + 4 + 2 + 1) << bit_offset)); // set one's
        break;

#if DEBUG
    default:
        throw std::invalid_argument("fourbit_byte::set(pos, nucleotide) invalid value\n");
        break;
#endif //DEBUG
    }
};


