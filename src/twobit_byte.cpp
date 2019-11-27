#include <iostream>
#include <fstream>

#include "config.hpp"

#include "twobit_byte.hpp"

const char twobit_byte::twobit_hash[256][5] = {"TTTT", "TTTC", "TTTA", "TTTG", "TTCT", "TTCC", "TTCA", "TTCG", "TTAT", "TTAC", "TTAA", "TTAG", "TTGT", "TTGC", "TTGA", "TTGG", "TCTT", "TCTC", "TCTA", "TCTG", "TCCT", "TCCC", "TCCA", "TCCG", "TCAT", "TCAC", "TCAA", "TCAG", "TCGT", "TCGC", "TCGA", "TCGG", "TATT", "TATC", "TATA", "TATG", "TACT", "TACC", "TACA", "TACG", "TAAT", "TAAC", "TAAA", "TAAG", "TAGT", "TAGC", "TAGA", "TAGG", "TGTT", "TGTC", "TGTA", "TGTG", "TGCT", "TGCC", "TGCA", "TGCG", "TGAT", "TGAC", "TGAA", "TGAG", "TGGT", "TGGC", "TGGA", "TGGG", "CTTT", "CTTC", "CTTA", "CTTG", "CTCT", "CTCC", "CTCA", "CTCG", "CTAT", "CTAC", "CTAA", "CTAG", "CTGT", "CTGC", "CTGA", "CTGG", "CCTT", "CCTC", "CCTA", "CCTG", "CCCT", "CCCC", "CCCA", "CCCG", "CCAT", "CCAC", "CCAA", "CCAG", "CCGT", "CCGC", "CCGA", "CCGG", "CATT", "CATC", "CATA", "CATG", "CACT", "CACC", "CACA", "CACG", "CAAT", "CAAC", "CAAA", "CAAG", "CAGT", "CAGC", "CAGA", "CAGG", "CGTT", "CGTC", "CGTA", "CGTG", "CGCT", "CGCC", "CGCA", "CGCG", "CGAT", "CGAC", "CGAA", "CGAG", "CGGT", "CGGC", "CGGA", "CGGG", "ATTT", "ATTC", "ATTA", "ATTG", "ATCT", "ATCC", "ATCA", "ATCG", "ATAT", "ATAC", "ATAA", "ATAG", "ATGT", "ATGC", "ATGA", "ATGG", "ACTT", "ACTC", "ACTA", "ACTG", "ACCT", "ACCC", "ACCA", "ACCG", "ACAT", "ACAC", "ACAA", "ACAG", "ACGT", "ACGC", "ACGA", "ACGG", "AATT", "AATC", "AATA", "AATG", "AACT", "AACC", "AACA", "AACG", "AAAT", "AAAC", "AAAA", "AAAG", "AAGT", "AAGC", "AAGA", "AAGG", "AGTT", "AGTC", "AGTA", "AGTG", "AGCT", "AGCC", "AGCA", "AGCG", "AGAT", "AGAC", "AGAA", "AGAG", "AGGT", "AGGC", "AGGA", "AGGG", "GTTT", "GTTC", "GTTA", "GTTG", "GTCT", "GTCC", "GTCA", "GTCG", "GTAT", "GTAC", "GTAA", "GTAG", "GTGT", "GTGC", "GTGA", "GTGG", "GCTT", "GCTC", "GCTA", "GCTG", "GCCT", "GCCC", "GCCA", "GCCG", "GCAT", "GCAC", "GCAA", "GCAG", "GCGT", "GCGC", "GCGA", "GCGG", "GATT", "GATC", "GATA", "GATG", "GACT", "GACC", "GACA", "GACG", "GAAT", "GAAC", "GAAA", "GAAG", "GAGT", "GAGC", "GAGA", "GAGG", "GGTT", "GGTC", "GGTA", "GGTG", "GGCT", "GGCC", "GGCA", "GGCG", "GGAT", "GGAC", "GGAA", "GGAG", "GGGT", "GGGC", "GGGA", "GGGG"};


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
        this->data = (unsigned char)(this->data & ~( (2+1) << bit_offset));
        break;
    case 1://NUCLEOTIDE_C (01)
        this->data = (unsigned char)(this->data & ~( (2  ) << bit_offset));
        this->data = (unsigned char)(this->data |  ( (  1) << bit_offset));
        break;
    case 2://NUCLEOTIDE_A (10)
        this->data = (unsigned char)(this->data & ~( (  1) << bit_offset));
        this->data = (unsigned char)(this->data |  ( (2  ) << bit_offset));
        break;
    case 3://NUCLEOTIDE_G (11)
        this->data = (unsigned char)(this->data |  ( (2+1) << bit_offset));
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
        seq[i] = twobit_byte::twobit_hash[this->data][i];
    }
    seq[length] = '\0';

    return seq;
}



const char *twobit_byte::get()
{
    return twobit_byte::twobit_hash[this->data];
}
