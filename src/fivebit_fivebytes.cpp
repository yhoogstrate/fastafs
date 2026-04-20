#include <cstdint>
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


const char fivebit_fivebytes::fivebit_alphabet[28 + 1] = "ABCDEFGHIJKLMNOPQRSTUVWYZX*-";
const char fivebit_fivebytes::encode_hash[28 + 1][2] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "Y", "Z", "X", "*", "-" };


// @todo, offset needs to be second parameter
void fivebit_fivebytes::set(unsigned char bit_offset, unsigned char amino_acid)
{
    // bit_offset: 0, 1, 2, 3, 4, 5, 6, 7
    this->data_decompressed[bit_offset] = fivebit_fivebytes::fivebit_alphabet[amino_acid];

    switch(bit_offset) {
    case 0:
        // 00000111 11222223 33334444 45555566 66677777
        // -----111
        this->data_compressed[0] = (unsigned char)((this->data_decompressed[0] & ~(128 + 64 + 32 + 16 + 8)) | (amino_acid  << 3));
        break;
    case 1:
        // 00000111 11222223 33334444 45555566 66677777

        // xxx00000
        // --xxx000
        this->data_compressed[0] = (unsigned char)((this->data_compressed[0] & ~(4 + 2 + 1)) | amino_acid >> 2);

        // 00000111 11222223 33334444 45555566 66677777
        //          --222223
        this->data_compressed[1] = (unsigned char)((this->data_compressed[1] & ~(128 + 64)) | amino_acid << 6);
        break;
    case 2:
        // 00000000
        // 0000000-  << 1
        // --00000-  ~(2 + 1) << 6
        this->data_compressed[1] = (unsigned char)((this->data_compressed[1] & ~(32 + 16 + 8 + 4 + 2)) | (amino_acid << 1 & ~(128 + 64)));
        break;
    case 3:
        // 00000111 11222223 33334444 45555566 66677777

        // 000xxxxx
        // -------x
        this->data_compressed[1] = (unsigned char)((this->data_compressed[1] & ~(1)) | amino_acid >> 4);

        // 000xxxxx
        // xxxx----
        this->data_compressed[2] = (unsigned char)((this->data_compressed[2] & ~((8 + 4 + 2 + 1) << 4)) | amino_acid << 4);
        break;
    case 4:
        // 00000111 11222223 33334444 45555566 66677777

        // 000xxxxx
        // -000xxxx
        this->data_compressed[2] = (unsigned char)((this->data_compressed[2] & ~(8 + 4 + 2 + 1)) | amino_acid >> 1);

        //
        this->data_compressed[3] = (unsigned char)((this->data_compressed[3] & ~(128)) | amino_acid << 7);
        break;
    case 5:
        // 00000111 11222223 33334444 45555566 66677777

        // 000xxxxx
        // 0xxxxx--
        this->data_compressed[3] = (unsigned char)((this->data_compressed[3] & ~(64 + 32 + 16 + 8 + 4)) | amino_acid << 2);
        break;
    case 6:
        // 00000111 11222223 33334444 45555566 66677777

        // 000xxxxx
        // ---000xx
        this->data_compressed[3] = (unsigned char)((this->data_compressed[3] & ~(2 + 1)) | amino_acid >> 3);


        // 000xxxxx
        // xxx-----
        this->data_compressed[4] = (unsigned char)((this->data_compressed[4] & ~(128 + 64 + 32)) | amino_acid << 5);

        break;
    case 7:
        // 00000111 11222223 33334444 45555566 66677777

        // 000xxxxx
        // ---xxxxx
        this->data_compressed[4] = (unsigned char)((this->data_compressed[4] & ~(16 + 8 + 4 + 2 + 1)) | (amino_acid & ~(128 + 64 + 32)));
        break;
    }
}


char *fivebit_fivebytes::get(void)
{
    return (char *) this->data_decompressed;
}


/**
 * @brief Decode into a provided buffer, returning the number of symbols written.
 * @param length Number of symbols to decode (1-7). For all 8 symbols, use get() instead.
 *        In DEBUG mode, values outside this range (including 8) throw.
 * @param output Buffer to write decoded symbols to. Caller is responsible for ensuring it has at least 'length' bytes.
 * @return Number of symbols written (same as length).
**/
size_t fivebit_fivebytes::get(unsigned char length, char *output)
{
#if DEBUG
    if(length < 1 || length > 7) {
        throw std::invalid_argument("fivebit_fivebytes::get(length, output) -> length must be 1-7, got " + std::to_string(length) + "; use get() for all 8 symbols\n");
    }
#endif //DEBUG

    memcpy(output, this->data_decompressed, length);
    return length;
}




// Encode 8 amino acids from a char buffer
void fivebit_fivebytes::set(char *buffer)
{
    for(unsigned char i = 0; i < 8; i++) {
        // Find index of character in alphabet
        unsigned char idx = 0;
        for(unsigned char j = 0; j < 28; j++) {
            if(fivebit_alphabet[j] == buffer[i]) {
                idx = j;
                break;
            }
        }
        this->set(i, idx);
    }
}



// @todo, offset needs to be second parameter
void fivebit_fivebytes::set_compressed(unsigned char (&compressed_data)[5])
{
    this->data_compressed[0] = compressed_data[0];
    this->data_compressed[1] = compressed_data[1];
    this->data_compressed[2] = compressed_data[2];
    this->data_compressed[3] = compressed_data[3];
    this->data_compressed[4] = compressed_data[4];

    this->unpack();
}


void fivebit_fivebytes::unpack()
{
    //  00000111 11222223 33334444 45555566 66677777
    //                                      66677777
    //                                      ---77777
    this->data_decompressed[7] = (unsigned char)(this->data_compressed[4] & ~(128 + 64 + 32));

    //  00000111 11222223 33334444 45555566 66677777
    //                             45555566 66677777
    //                             -----455 55566666
    //                             -----455 ---66666
    //                                      ---66666
    this->data_decompressed[6] = (unsigned char)(((this->data_compressed[3] << 8) | (this->data_compressed[4])) >> 5);
    this->data_decompressed[6] = (unsigned char)(this->data_decompressed[6] & ~(128 + 64 + 32));

    //  00000111 11222223 33334444 45555566 66677777
    //                             45555566
    //                             --455555
    //                             ---55555
    this->data_decompressed[5] = (unsigned char)(this->data_compressed[3] >> 2);
    this->data_decompressed[5] = (unsigned char)(this->data_decompressed[5] & ~(128 + 64 + 32));// only bit 6 should be set to 0

    //  00000111 11222223 33334444 45555566 66677777
    //                    33334444 45555566
    //                    -------3 33344444
    //                             33344444
    //                             ---44444
    this->data_decompressed[4] = (unsigned char)(((this->data_compressed[2] << 8) | (this->data_compressed[3])) >> 7);
    this->data_decompressed[4] = (unsigned char)(this->data_decompressed[4] & ~(128 + 64 + 32));

    //  00000111 11222223 33334444 45555566 66677777
    //  11222223 33334444       bit shift << 8 + normal
    //  ----1122 22233333       bit shift >> 4
    //           22233333       convert to u-char
    //           ---33333       set zero's
    this->data_decompressed[3] = (unsigned char)(((this->data_compressed[1] << 8) | (this->data_compressed[2])) >> 4);
    this->data_decompressed[3] = (unsigned char)(this->data_decompressed[3] & ~(128 + 64 + 32));

    //  00101000 00100101 00110000 01101100 10110010
    //  00000111 11222223 33334444 45555566 66677777
    //           11222223
    //           -1122222
    //           ---22222
    this->data_decompressed[2] = (unsigned char)(this->data_compressed[1] >> 1);   // shifts of unsigned types always zero-fill :)
    this->data_decompressed[2] = (unsigned char)(this->data_decompressed[2] & ~(128 + 64 + 32)); // i think only bit 6 and 7 need to be set because of the shift above

    //  00000111 11222223 33334444 45555566 66677777
    //  00000111 11222223
    //  ------00 00011111
    //           00011111
    //           ---11111
    this->data_decompressed[1] = (unsigned char)(((this->data_compressed[0] << 8) | (this->data_compressed[1])) >> 6);
    this->data_decompressed[1] = (unsigned char)(this->data_decompressed[1] & ~(128 + 64 + 32));

    //  00000111 11222223 33334444 45555566 66677777
    //  00000111
    //  ---00000
    this->data_decompressed[0] = (unsigned char)(this->data_compressed[0] >> 3);   // shifts of unsigned types always zero-fill :)


    // decode
    for(unsigned char i = 0; i < 8; i++) {
        this->data_decompressed[i] = fivebit_fivebytes::fivebit_alphabet[this->data_decompressed[i]];
    }
}



// static functions - not bound to class instance
unsigned char fivebit_fivebytes::iterator_to_offset(unsigned int iterator)
{
    return (unsigned char)(iterator % 8);
}


/*
    it is not necessary to write the whole 5 byes if only 3 contain compressin information
    00000111 11222223 33334444 45555566 66677777
    // 00000111 11222223 33334444 45555566 66677777
    //     1     2    3     4     5    6     7
*/
unsigned char fivebit_fivebytes::decompressed_to_compressed_bytes(unsigned char decompressed_bytes)
{
    return (unsigned char)(((decompressed_bytes + 3) * 5 / 8) - 1);
}






/*
 * To calculate file offset
 *
 * example:
 *
 * >Seq
 * [ABCDEFGH][ABCDEFGH] has offset of 2
 *
 * >Seq
 * [ABCDEFGH][ABCDEFGH][A] has offset of 2?
  *
 * >Seq
 * [ABCDEFGH][ABCDEFGH][ACCCAAC] has offset of 2?
 * */
off_t fivebit_fivebytes::nucleotides_to_compressed_fileoffset(size_t n_amino_acids)
{
    off_t out = n_amino_acids / (off_t) fivebit_fivebytes::nucleotides_per_chunk;

    out = out * fivebit_fivebytes::bytes_per_chunk;

    return out;
}

/*
 * To calculate file offset
 *
 * example:
 *
 * >Seq
 * [ABCDEFGH][ABCDEFGH] has offset of 10
 *
 * >Seq
 * [ABCDEFGH][ABCDEFGH][A] has offset of 11?
 *
 */
off_t fivebit_fivebytes::nucleotides_to_compressed_offset(size_t n_amino_acids)
{
    return  fivebit_fivebytes::nucleotides_to_compressed_fileoffset(n_amino_acids)
            + fivebit_fivebytes::decompressed_to_compressed_bytes(n_amino_acids % fivebit_fivebytes::nucleotides_per_chunk);
}







void fivebit_fivebytes::next(chunked_reader &r)
{
    r.read(this->data_compressed, fivebit_fivebytes::bytes_per_chunk);
    this->unpack();
}


void fivebit_fivebytes::decode(const unsigned char *input, char *output)
{
    // Pack 5 bytes into a 40-bit integer, most-significant byte first.
    // Layout: 00000111 11222223 33334444 45555566 66677777
    //         symbol 0 starts at bit 39, each symbol is 5 bits wide.
    const uint64_t bits = ((uint64_t)input[0] << 32)
                        | ((uint64_t)input[1] << 24)
                        | ((uint64_t)input[2] << 16)
                        | ((uint64_t)input[3] <<  8)
                        |  (uint64_t)input[4];

    // Extract each 5-bit symbol by shifting to bit 0 and masking with 0b00011111.
    // The mask is needed because uint64_t is 64 bits wide — after shifting, bits above
    // position 4 may contain residue from neighbouring symbols. 0b00011111 zeroes bits
    // 7,6,5 so the index stays within the valid alphabet range (0–27).
    output[0] = fivebit_alphabet[(bits >> 35) & 0b00011111];
    output[1] = fivebit_alphabet[(bits >> 30) & 0b00011111];
    output[2] = fivebit_alphabet[(bits >> 25) & 0b00011111];
    output[3] = fivebit_alphabet[(bits >> 20) & 0b00011111];
    output[4] = fivebit_alphabet[(bits >> 15) & 0b00011111];
    output[5] = fivebit_alphabet[(bits >> 10) & 0b00011111];
    output[6] = fivebit_alphabet[(bits >>  5) & 0b00011111];
    output[7] = fivebit_alphabet[(bits >>  0) & 0b00011111];
}
