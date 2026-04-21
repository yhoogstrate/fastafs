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
char ENCODE_HASH_TWOBIT_DNA[256][5] = {"TTTT", "TTTC", "TTTA", "TTTG", "TTCT", "TTCC", "TTCA", "TTCG", "TTAT", "TTAC", "TTAA", "TTAG", "TTGT", "TTGC", "TTGA", "TTGG", "TCTT", "TCTC", "TCTA", "TCTG", "TCCT", "TCCC", "TCCA", "TCCG", "TCAT", "TCAC", "TCAA", "TCAG", "TCGT", "TCGC", "TCGA", "TCGG", "TATT", "TATC", "TATA", "TATG", "TACT", "TACC", "TACA", "TACG", "TAAT", "TAAC", "TAAA", "TAAG", "TAGT", "TAGC", "TAGA", "TAGG", "TGTT", "TGTC", "TGTA", "TGTG", "TGCT", "TGCC", "TGCA", "TGCG", "TGAT", "TGAC", "TGAA", "TGAG", "TGGT", "TGGC", "TGGA", "TGGG", "CTTT", "CTTC", "CTTA", "CTTG", "CTCT", "CTCC", "CTCA", "CTCG", "CTAT", "CTAC", "CTAA", "CTAG", "CTGT", "CTGC", "CTGA", "CTGG", "CCTT", "CCTC", "CCTA", "CCTG", "CCCT", "CCCC", "CCCA", "CCCG", "CCAT", "CCAC", "CCAA", "CCAG", "CCGT", "CCGC", "CCGA", "CCGG", "CATT", "CATC", "CATA", "CATG", "CACT", "CACC", "CACA", "CACG", "CAAT", "CAAC", "CAAA", "CAAG", "CAGT", "CAGC", "CAGA", "CAGG", "CGTT", "CGTC", "CGTA", "CGTG", "CGCT", "CGCC", "CGCA", "CGCG", "CGAT", "CGAC", "CGAA", "CGAG", "CGGT", "CGGC", "CGGA", "CGGG", "ATTT", "ATTC", "ATTA", "ATTG", "ATCT", "ATCC", "ATCA", "ATCG", "ATAT", "ATAC", "ATAA", "ATAG", "ATGT", "ATGC", "ATGA", "ATGG", "ACTT", "ACTC", "ACTA", "ACTG", "ACCT", "ACCC", "ACCA", "ACCG", "ACAT", "ACAC", "ACAA", "ACAG", "ACGT", "ACGC", "ACGA", "ACGG", "AATT", "AATC", "AATA", "AATG", "AACT", "AACC", "AACA", "AACG", "AAAT", "AAAC", "AAAA", "AAAG", "AAGT", "AAGC", "AAGA", "AAGG", "AGTT", "AGTC", "AGTA", "AGTG", "AGCT", "AGCC", "AGCA", "AGCG", "AGAT", "AGAC", "AGAA", "AGAG", "AGGT", "AGGC", "AGGA", "AGGG", "GTTT", "GTTC", "GTTA", "GTTG", "GTCT", "GTCC", "GTCA", "GTCG", "GTAT", "GTAC", "GTAA", "GTAG", "GTGT", "GTGC", "GTGA", "GTGG", "GCTT", "GCTC", "GCTA", "GCTG", "GCCT", "GCCC", "GCCA", "GCCG", "GCAT", "GCAC", "GCAA", "GCAG", "GCGT", "GCGC", "GCGA", "GCGG", "GATT", "GATC", "GATA", "GATG", "GACT", "GACC", "GACA", "GACG", "GAAT", "GAAC", "GAAA", "GAAG", "GAGT", "GAGC", "GAGA", "GAGG", "GGTT", "GGTC", "GGTA", "GGTG", "GGCT", "GGCC", "GGCA", "GGCG", "GGAT", "GGAC", "GGAA", "GGAG", "GGGT", "GGGC", "GGGA", "GGGG"};
char ENCODE_HASH_TWOBIT_RNA[256][5] = {"UUUU", "UUUC", "UUUA", "UUUG", "UUCU", "UUCC", "UUCA", "UUCG", "UUAU", "UUAC", "UUAA", "UUAG", "UUGU", "UUGC", "UUGA", "UUGG", "UCUU", "UCUC", "UCUA", "UCUG", "UCCU", "UCCC", "UCCA", "UCCG", "UCAU", "UCAC", "UCAA", "UCAG", "UCGU", "UCGC", "UCGA", "UCGG", "UAUU", "UAUC", "UAUA", "UAUG", "UACU", "UACC", "UACA", "UACG", "UAAU", "UAAC", "UAAA", "UAAG", "UAGU", "UAGC", "UAGA", "UAGG", "UGUU", "UGUC", "UGUA", "UGUG", "UGCU", "UGCC", "UGCA", "UGCG", "UGAU", "UGAC", "UGAA", "UGAG", "UGGU", "UGGC", "UGGA", "UGGG", "CUUU", "CUUC", "CUUA", "CUUG", "CUCU", "CUCC", "CUCA", "CUCG", "CUAU", "CUAC", "CUAA", "CUAG", "CUGU", "CUGC", "CUGA", "CUGG", "CCUU", "CCUC", "CCUA", "CCUG", "CCCU", "CCCC", "CCCA", "CCCG", "CCAU", "CCAC", "CCAA", "CCAG", "CCGU", "CCGC", "CCGA", "CCGG", "CAUU", "CAUC", "CAUA", "CAUG", "CACU", "CACC", "CACA", "CACG", "CAAU", "CAAC", "CAAA", "CAAG", "CAGU", "CAGC", "CAGA", "CAGG", "CGUU", "CGUC", "CGUA", "CGUG", "CGCU", "CGCC", "CGCA", "CGCG", "CGAU", "CGAC", "CGAA", "CGAG", "CGGU", "CGGC", "CGGA", "CGGG", "AUUU", "AUUC", "AUUA", "AUUG", "AUCU", "AUCC", "AUCA", "AUCG", "AUAU", "AUAC", "AUAA", "AUAG", "AUGU", "AUGC", "AUGA", "AUGG", "ACUU", "ACUC", "ACUA", "ACUG", "ACCU", "ACCC", "ACCA", "ACCG", "ACAU", "ACAC", "ACAA", "ACAG", "ACGU", "ACGC", "ACGA", "ACGG", "AAUU", "AAUC", "AAUA", "AAUG", "AACU", "AACC", "AACA", "AACG", "AAAU", "AAAC", "AAAA", "AAAG", "AAGU", "AAGC", "AAGA", "AAGG", "AGUU", "AGUC", "AGUA", "AGUG", "AGCU", "AGCC", "AGCA", "AGCG", "AGAU", "AGAC", "AGAA", "AGAG", "AGGU", "AGGC", "AGGA", "AGGG", "GUUU", "GUUC", "GUUA", "GUUG", "GUCU", "GUCC", "GUCA", "GUCG", "GUAU", "GUAC", "GUAA", "GUAG", "GUGU", "GUGC", "GUGA", "GUGG", "GCUU", "GCUC", "GCUA", "GCUG", "GCCU", "GCCC", "GCCA", "GCCG", "GCAU", "GCAC", "GCAA", "GCAG", "GCGU", "GCGC", "GCGA", "GCGG", "GAUU", "GAUC", "GAUA", "GAUG", "GACU", "GACC", "GACA", "GACG", "GAAU", "GAAC", "GAAA", "GAAG", "GAGU", "GAGC", "GAGA", "GAGG", "GGUU", "GGUC", "GGUA", "GGUG", "GGCU", "GGCC", "GGCA", "GGCG", "GGAU", "GGAC", "GGAA", "GGAG", "GGGU", "GGGC", "GGGA", "GGGG"};

twobit_byte::twobit_byte(char (&encode_hash_arg)[256][5]): encode_hash(encode_hash_arg) {}

twobit_byte_dna::twobit_byte_dna(): twobit_byte(ENCODE_HASH_TWOBIT_DNA) {}

twobit_byte_rna::twobit_byte_rna(): twobit_byte(ENCODE_HASH_TWOBIT_RNA) {}


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
}


// input char "AACCCTTGG"
// N's are treated as 0, for some weird reason
// this function seems specific for UCSC 2 bit format?! - if so, denote it like that
void twobit_byte::set(char* buffer)
{
    static constexpr std::array<unsigned char, 4> bit_offsets = {6, 4, 2, 0};
    for(unsigned char i = 0; i < 4; i++) {
        switch(buffer[i]) {
        case 't':
        case 'T':
        case 'u':
        case 'U':
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
 * @brief Decode into a provided buffer, returning the number of nucleotides written.
 * @param length Number of nucleotides to decode (1-4). In DEBUG mode, values outside this range throw.
 * @param output Buffer to write decoded nucleotides to. Caller is responsible for ensuring it has at least 'length' bytes.
 * @return Number of nucleotides written (same as length).
**/
size_t twobit_byte::get(unsigned char length, char *output)
{
#if DEBUG
    if(length < 1 || length > 4) {
        throw std::invalid_argument("twobit_byte::get(length, output) -> length must be 1-4, got " + std::to_string(length) + "\n");
    }
#endif //DEBUG

    memcpy(output, twobit_byte::encode_hash[this->data], length);
    return length;
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


void twobit_byte::next(chunked_reader &r)
{
    r.read(&this->data, 1);
}






