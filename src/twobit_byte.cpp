#include <iostream>
#include <fstream>

#include "config.hpp"

#include "twobit_byte.hpp"



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
unsigned char twobit_byte::iterator_to_offset(uint32_t iterator)
{
    return (unsigned char)((3 - (iterator % 4)) * 2);
}

// @todo, offset needs to be second parameter
void twobit_byte::set(unsigned char bit_offset, unsigned char nucleotide)
{
    // bit_offset must be: {0, 2, 4 or 6};
    // nucleotides must be:
    // => T - 00, C - 01, A - 10, G - 11
    // => T - 00, C -  1, A -  2, G -  3
#if DEBUG
    if(bit_offset != 0 and bit_offset != 2 and bit_offset != 4 and bit_offset != 6) {
        throw std::invalid_argument("twobit_byte(bit_offset, ..) must be 0, 2, 4 or 6\n");
    }
#endif //DEBUG
    //set   bit(s): INPUT |= 1 << N;
    //unset bit(s): INPUT &= ~(1 << N);
    switch(nucleotide) {
    case 0://NUCLEOTIDE_T (00)
        //      ??????00
        //          11?? ~(3 << bit_offset)
        // data ????????
        this->data = (unsigned char)(this->data & ~((2 + 1) << bit_offset));
        break;
    case 1://NUCLEOTIDE_C (01)
        this->data = (unsigned char)(this->data & ~((2) << bit_offset));
        this->data = (unsigned char)(this->data | ((1) << bit_offset));
        break;
    case 2://NUCLEOTIDE_A (10)
        this->data = (unsigned char)(this->data & ~((1) << bit_offset));
        this->data = (unsigned char)(this->data | ((2) << bit_offset));
        break;
    case 3://NUCLEOTIDE_G (11)
        this->data = (unsigned char)(this->data | ((2 + 1) << bit_offset));
        break;
#if DEBUG
    default:
        throw std::invalid_argument("twobit_byte::set(pos, nucleotide) invalid value\n");
        break;
#endif //DEBUG
    }
};


// input char "AACCCTTGG"
// N's are treated as 0, for some weird reason
// this function seems specific for UCSC 2 bit format?! - if so, denote it like that
void twobit_byte::set(char* buffer)
{
    const std::array< unsigned char, 4> bit_offsets = {6, 4, 2, 0};
    for(unsigned char i = 0; i < 4; i++) {
        switch(buffer[i]) {
        case 't':
        case 'T':
        case 'n':
        case 'N':
            this->set(bit_offsets[i], 0);
            break;
        case 'c':
        case 'C':
            this->set(bit_offsets[i], 1);
            break;
        case 'a':
        case 'A':
            this->set(bit_offsets[i], 2);
            break;
        case 'g':
        case 'G':
            this->set(bit_offsets[i], 3);
            break;
#if DEBUG
        default:
            throw std::invalid_argument("twobit_byte::set(char *) invalid value\n");
            break;
#endif //DEBUG
        }
    }
}

/**
 * @brief fully decodes a twobit byte, not referencing to a hash but allocating a new char*,
 * slower than twobit_byte::get(void) but capable of determining very ends
**/
char *twobit_byte::get(unsigned char length)
{
#if DEBUG
    if(length > 4) {
        throw std::invalid_argument("twobit_byte::get(unsigned char length) -> out of bound: " + std::to_string(length) + "\n");
    }
#endif //DEBUG

    char *seq = new char[length + 1];

    for(unsigned char i = 0; i < length; i++) { // length = 4: i = 0, 1, 2, 3
        seq[i] = twobit_byte::encode_hash[this->data][i];
    }
    seq[length] = '\0';

    return seq;
}



char *twobit_byte::get()
{
    return twobit_byte::encode_hash[this->data];
}


/*
 * To calculate file offset
 *
 * example:
 *
 * >Seq
 * [ACTG][ACTG] has offset of 2 (or 3)?
 *
 * >Seq
 * [ACTG][ACTG][AC] has offset of 2 (or 3)?
 * */
off_t twobit_byte::nucleotides_to_compressed_fileoffset(size_t n_nucleotides)
{
    return (off_t) n_nucleotides / twobit_byte::nucleotides_per_byte;
}

off_t twobit_byte::nucleotides_to_compressed_offset(size_t n_nucleotides)
{
    return  twobit_byte::nucleotides_to_compressed_fileoffset(n_nucleotides + twobit_byte::nucleotides_per_byte - 1);
}




// needs to be separate function because not encodings read byte-per-byte
void twobit_byte::next(chunked_reader &r)
{
    this->data = r.read();
}


