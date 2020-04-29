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
    // set
};


unsigned char *fivebit_fivebytes::get(void) {
    return this->data_decompressed;
}




// @todo, offset needs to be second parameter
void fivebit_fivebytes::set_compressed(unsigned char (&compressed_data)[5]) {
    this->data_compressed[0] = compressed_data[0];
    this->data_compressed[1] = compressed_data[1];
    this->data_compressed[2] = compressed_data[2];
    this->data_compressed[3] = compressed_data[3];
    this->data_compressed[4] = compressed_data[4];

    this->unpack();
}


void fivebit_fivebytes::unpack() {
    //  00000111 11222223 33334444 45555566 66677777
    //                                      66677777
    //                                      ---77777
    this->data_decompressed[7] = (unsigned char)(this->data_compressed[4] & ~((4 + 2 + 1) << 5));

    //  00000111 11222223 33334444 45555566 66677777
    //                             45555566 66677777
    //                             -----455 55566666
    //                             -----455 ---66666
    //                                      ---66666
    this->data_decompressed[6] = (unsigned char)(((this->data_compressed[3] << 8) | (this->data_compressed[4])) >> 5);
    this->data_decompressed[6] = (unsigned char)(this->data_decompressed[6] & ~((4 + 2 + 1) << 5));

    //  00000111 11222223 33334444 45555566 66677777
    //                             45555566
    //                             --455555
    //                             ---55555
    this->data_decompressed[5] = (unsigned char)(this->data_compressed[3] >> 2);
    this->data_decompressed[5] = (unsigned char)(this->data_decompressed[5] & ~((4 + 2 + 1) << 5));// only bit 6 should be set to 0

    //  00000111 11222223 33334444 45555566 66677777
    //                    33334444 45555566
    //                    -------3 33344444
    //                             33344444
    //                             ---44444
    this->data_decompressed[4] = (unsigned char)(((this->data_compressed[2] << 8) | (this->data_compressed[3])) >> 7);
    this->data_decompressed[4] = (unsigned char)(this->data_decompressed[4] & ~((4 + 2 + 1) << 5));
    
    //  00000111 11222223 33334444 45555566 66677777
    //  11222223 33334444       bit shift << 8 + normal
    //  ----1122 22233333       bit shift >> 4
    //           22233333       convert to u-char
    //           ---33333       set zero's
    this->data_decompressed[3] = (unsigned char)(((this->data_compressed[1] << 8) | (this->data_compressed[2])) >> 4);
    this->data_decompressed[3] = (unsigned char)(this->data_decompressed[3] & ~((4 + 2 + 1) << 5));

    //  00101000 00100101 00110000 01101100 10110010
    //  00000111 11222223 33334444 45555566 66677777
    //           11222223
    //           -1122222
    //           ---22222
    this->data_decompressed[2] = (unsigned char)(this->data_compressed[1] >> 1);   // shifts of unsigned types always zero-fill :)
    this->data_decompressed[2] = (unsigned char)(this->data_decompressed[2] & ~((4 + 2 + 1) << 5)); // i think only bit 6 and 7 need to be set because of the shift above

    //  00000111 11222223 33334444 45555566 66677777
    //  00000111 11222223
    //  ------00 00011111
    //           00011111
    //           ---11111
    this->data_decompressed[1] = (unsigned char)(((this->data_compressed[0] << 8) | (this->data_compressed[1])) >> 6);
    this->data_decompressed[1] = (unsigned char)(this->data_decompressed[1] & ~((4 + 2 + 1) << 5));

    //  00000111 11222223 33334444 45555566 66677777
    //  00000111
    //  ---00000
    this->data_decompressed[0] = (unsigned char)(this->data_compressed[0] >> 3);   // shifts of unsigned types always zero-fill :)
}
