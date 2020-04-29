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


const char fivebit_fivebytes::fivebit_alhpabet[28 + 1] = "ABCDEFGHIJKLMNOPQRSTUVWYZX*-";
const char fivebit_fivebytes::encode_hash[28 + 1][2] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "Y", "Z", "X", "*", "-" };


// @todo, offset needs to be second parameter
void fivebit_fivebytes::set(unsigned char bit_offset, unsigned char nucleotide)
{
    // bit_offset must be: {0, or 4}; -> location in bits
    // nucleotides must be:
    // => T - 00, C - 01, A - 10, G - 11
    // => T - 00, C -  1, A -  2, G -  3
#if DEBUG
    if(bit_offset != 0 and bit_offset != 4) {
        throw std::invalid_argument("fivebit_fivebytes(bit_offset, ..) must be 0 or 4\n");
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
        throw std::invalid_argument("fivebit_fivebytes::set(pos, nucleotide) invalid value\n");
        break;
#endif //DEBUG
    }
};


