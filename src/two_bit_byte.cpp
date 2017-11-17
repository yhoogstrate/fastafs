#include <iostream>
#include <fstream>

#include "config.hpp"

#include "two_bit_byte.hpp"

const char two_bit_byte::hash_table[(256*4) + 1] = "TTTTTTTCTTTATTTGTTCTTTCCTTCATTCGTTATTTACTTAATTAGTTGTTTGCTTGATTGGTCTTTCTCTCTATCTGTCCTTCCCTCCATCCGTCATTCACTCAATCAGTCGTTCGCTCGATCGGTATTTATCTATATATGTACTTACCTACATACGTAATTAACTAAATAAGTAGTTAGCTAGATAGGTGTTTGTCTGTATGTGTGCTTGCCTGCATGCGTGATTGACTGAATGAGTGGTTGGCTGGATGGGCTTTCTTCCTTACTTGCTCTCTCCCTCACTCGCTATCTACCTAACTAGCTGTCTGCCTGACTGGCCTTCCTCCCTACCTGCCCTCCCCCCCACCCGCCATCCACCCAACCAGCCGTCCGCCCGACCGGCATTCATCCATACATGCACTCACCCACACACGCAATCAACCAAACAAGCAGTCAGCCAGACAGGCGTTCGTCCGTACGTGCGCTCGCCCGCACGCGCGATCGACCGAACGAGCGGTCGGCCGGACGGGATTTATTCATTAATTGATCTATCCATCAATCGATATATACATAAATAGATGTATGCATGAATGGACTTACTCACTAACTGACCTACCCACCAACCGACATACACACAAACAGACGTACGCACGAACGGAATTAATCAATAAATGAACTAACCAACAAACGAAATAAACAAAAAAAGAAGTAAGCAAGAAAGGAGTTAGTCAGTAAGTGAGCTAGCCAGCAAGCGAGATAGACAGAAAGAGAGGTAGGCAGGAAGGGGTTTGTTCGTTAGTTGGTCTGTCCGTCAGTCGGTATGTACGTAAGTAGGTGTGTGCGTGAGTGGGCTTGCTCGCTAGCTGGCCTGCCCGCCAGCCGGCATGCACGCAAGCAGGCGTGCGCGCGAGCGGGATTGATCGATAGATGGACTGACCGACAGACGGAATGAACGAAAGAAGGAGTGAGCGAGAGAGGGGTTGGTCGGTAGGTGGGCTGGCCGGCAGGCGGGATGGACGGAAGGAGGGGTGGGCGGGAGGGG" ;
const char two_bit_byte::inverse_hash[256][5] = {"TTTT", "TTTC", "TTTA", "TTTG", "TTCT", "TTCC", "TTCA", "TTCG", "TTAT", "TTAC", "TTAA", "TTAG", "TTGT", "TTGC", "TTGA", "TTGG", "TCTT", "TCTC", "TCTA", "TCTG", "TCCT", "TCCC", "TCCA", "TCCG", "TCAT", "TCAC", "TCAA", "TCAG", "TCGT", "TCGC", "TCGA", "TCGG", "TATT", "TATC", "TATA", "TATG", "TACT", "TACC", "TACA", "TACG", "TAAT", "TAAC", "TAAA", "TAAG", "TAGT", "TAGC", "TAGA", "TAGG", "TGTT", "TGTC", "TGTA", "TGTG", "TGCT", "TGCC", "TGCA", "TGCG", "TGAT", "TGAC", "TGAA", "TGAG", "TGGT", "TGGC", "TGGA", "TGGG", "CTTT", "CTTC", "CTTA", "CTTG", "CTCT", "CTCC", "CTCA", "CTCG", "CTAT", "CTAC", "CTAA", "CTAG", "CTGT", "CTGC", "CTGA", "CTGG", "CCTT", "CCTC", "CCTA", "CCTG", "CCCT", "CCCC", "CCCA", "CCCG", "CCAT", "CCAC", "CCAA", "CCAG", "CCGT", "CCGC", "CCGA", "CCGG", "CATT", "CATC", "CATA", "CATG", "CACT", "CACC", "CACA", "CACG", "CAAT", "CAAC", "CAAA", "CAAG", "CAGT", "CAGC", "CAGA", "CAGG", "CGTT", "CGTC", "CGTA", "CGTG", "CGCT", "CGCC", "CGCA", "CGCG", "CGAT", "CGAC", "CGAA", "CGAG", "CGGT", "CGGC", "CGGA", "CGGG", "ATTT", "ATTC", "ATTA", "ATTG", "ATCT", "ATCC", "ATCA", "ATCG", "ATAT", "ATAC", "ATAA", "ATAG", "ATGT", "ATGC", "ATGA", "ATGG", "ACTT", "ACTC", "ACTA", "ACTG", "ACCT", "ACCC", "ACCA", "ACCG", "ACAT", "ACAC", "ACAA", "ACAG", "ACGT", "ACGC", "ACGA", "ACGG", "AATT", "AATC", "AATA", "AATG", "AACT", "AACC", "AACA", "AACG", "AAAT", "AAAC", "AAAA", "AAAG", "AAGT", "AAGC", "AAGA", "AAGG", "AGTT", "AGTC", "AGTA", "AGTG", "AGCT", "AGCC", "AGCA", "AGCG", "AGAT", "AGAC", "AGAA", "AGAG", "AGGT", "AGGC", "AGGA", "AGGG", "GTTT", "GTTC", "GTTA", "GTTG", "GTCT", "GTCC", "GTCA", "GTCG", "GTAT", "GTAC", "GTAA", "GTAG", "GTGT", "GTGC", "GTGA", "GTGG", "GCTT", "GCTC", "GCTA", "GCTG", "GCCT", "GCCC", "GCCA", "GCCG", "GCAT", "GCAC", "GCAA", "GCAG", "GCGT", "GCGC", "GCGA", "GCGG", "GATT", "GATC", "GATA", "GATG", "GACT", "GACC", "GACA", "GACG", "GAAT", "GAAC", "GAAA", "GAAG", "GAGT", "GAGC", "GAGA", "GAGG", "GGTT", "GGTC", "GGTA", "GGTG", "GGCT", "GGCC", "GGCA", "GGCG", "GGAT", "GGAC", "GGAA", "GGAG", "GGGT", "GGGC", "GGGA", "GGGG"};

void two_bit_byte::set(unsigned char bit_offset, unsigned char nucleotide)
{
    // bit_offset must be: {0, 2, 4 or 6};
    // nucleotides must be:
    // => T - 00, C - 01, A - 10, G - 11
    // => T - 00, C -  1, A -  2, G -  3

    printf("{%i}\n", bit_offset);

#if DEBUG
    if(bit_offset != 0 and bit_offset != 2 and bit_offset != 4 and bit_offset != 6) {
        throw std::invalid_argument("two_bit_byte(bit_offset, ..) must be 0, 2, 4 or 6\n");
    }
#endif //DEBUG

    this->data |= (unsigned char) (nucleotide << bit_offset);
};

void two_bit_byte::print()
{
    // leuk, testen!
    char c1 = two_bit_byte::hash_table[this->data * 4];
    char c2 = two_bit_byte::hash_table[(this->data * 4) + 1] ;
    char c3 = two_bit_byte::hash_table[(this->data * 4) + 2] ;
    char c4 = two_bit_byte::hash_table[(this->data * 4) + 3] ;
    printf("[%i] -> %c%c%c%c\n", this->data, c1,c2,c3,c4);

}

const char *two_bit_byte::get() {
    return two_bit_byte::inverse_hash[this->data];
}
