
#ifndef TWOBIT_BYTE_HPP
#define TWOBIT_BYTE_HPP

#include <array>
#include "config.hpp"

#include "chunked_reader.hpp"
#include "xbit_byte_encoder.hpp"


class twobit_byte : xbit_byte_encoder
{
private: // things only needed by the compression [encoding, not decoding]

public:
    static const char xbit_byte_encoder::n_fill_unmasked = 'N';
    static const char xbit_byte_encoder::n_fill_masked = 'n';
    static const unsigned char xbit_byte_encoder::bits_per_nucleotide = 2;

    char (&encode_hash)[256][5];
    twobit_byte(char (&encode_hash_arg)[256][5]): encode_hash(encode_hash_arg) {};

    
    static const char nucleotides_per_byte = 8 / twobit_byte::bits_per_nucleotide ; // this is about compressed data
    static const char nucleotides_per_chunk = 8 / twobit_byte::bits_per_nucleotide ; // this is about decompressed chunks

    unsigned char data; // go private
    void set(unsigned char, unsigned char);
    void set(char*);// string met 4 bytes set
    char *get(void);
    char *get(unsigned char);

    static unsigned char iterator_to_offset(unsigned int);

    static off_t nucleotides_to_compressed_fileoffset(size_t); // file offset waarna gelezen kan worden
    static off_t nucleotides_to_compressed_offset(size_t);// aantal bytes nodig om zoveel data weg te schrijven

    void next(chunked_reader &);  // update the compressed data
};




static char ENCODE_HASH_TWOBIT_DNA[256][5] = {"TTTT", "TTTC", "TTTA", "TTTG", "TTCT", "TTCC", "TTCA", "TTCG", "TTAT", "TTAC", "TTAA", "TTAG", "TTGT", "TTGC", "TTGA", "TTGG", "TCTT", "TCTC", "TCTA", "TCTG", "TCCT", "TCCC", "TCCA", "TCCG", "TCAT", "TCAC", "TCAA", "TCAG", "TCGT", "TCGC", "TCGA", "TCGG", "TATT", "TATC", "TATA", "TATG", "TACT", "TACC", "TACA", "TACG", "TAAT", "TAAC", "TAAA", "TAAG", "TAGT", "TAGC", "TAGA", "TAGG", "TGTT", "TGTC", "TGTA", "TGTG", "TGCT", "TGCC", "TGCA", "TGCG", "TGAT", "TGAC", "TGAA", "TGAG", "TGGT", "TGGC", "TGGA", "TGGG", "CTTT", "CTTC", "CTTA", "CTTG", "CTCT", "CTCC", "CTCA", "CTCG", "CTAT", "CTAC", "CTAA", "CTAG", "CTGT", "CTGC", "CTGA", "CTGG", "CCTT", "CCTC", "CCTA", "CCTG", "CCCT", "CCCC", "CCCA", "CCCG", "CCAT", "CCAC", "CCAA", "CCAG", "CCGT", "CCGC", "CCGA", "CCGG", "CATT", "CATC", "CATA", "CATG", "CACT", "CACC", "CACA", "CACG", "CAAT", "CAAC", "CAAA", "CAAG", "CAGT", "CAGC", "CAGA", "CAGG", "CGTT", "CGTC", "CGTA", "CGTG", "CGCT", "CGCC", "CGCA", "CGCG", "CGAT", "CGAC", "CGAA", "CGAG", "CGGT", "CGGC", "CGGA", "CGGG", "ATTT", "ATTC", "ATTA", "ATTG", "ATCT", "ATCC", "ATCA", "ATCG", "ATAT", "ATAC", "ATAA", "ATAG", "ATGT", "ATGC", "ATGA", "ATGG", "ACTT", "ACTC", "ACTA", "ACTG", "ACCT", "ACCC", "ACCA", "ACCG", "ACAT", "ACAC", "ACAA", "ACAG", "ACGT", "ACGC", "ACGA", "ACGG", "AATT", "AATC", "AATA", "AATG", "AACT", "AACC", "AACA", "AACG", "AAAT", "AAAC", "AAAA", "AAAG", "AAGT", "AAGC", "AAGA", "AAGG", "AGTT", "AGTC", "AGTA", "AGTG", "AGCT", "AGCC", "AGCA", "AGCG", "AGAT", "AGAC", "AGAA", "AGAG", "AGGT", "AGGC", "AGGA", "AGGG", "GTTT", "GTTC", "GTTA", "GTTG", "GTCT", "GTCC", "GTCA", "GTCG", "GTAT", "GTAC", "GTAA", "GTAG", "GTGT", "GTGC", "GTGA", "GTGG", "GCTT", "GCTC", "GCTA", "GCTG", "GCCT", "GCCC", "GCCA", "GCCG", "GCAT", "GCAC", "GCAA", "GCAG", "GCGT", "GCGC", "GCGA", "GCGG", "GATT", "GATC", "GATA", "GATG", "GACT", "GACC", "GACA", "GACG", "GAAT", "GAAC", "GAAA", "GAAG", "GAGT", "GAGC", "GAGA", "GAGG", "GGTT", "GGTC", "GGTA", "GGTG", "GGCT", "GGCC", "GGCA", "GGCG", "GGAT", "GGAC", "GGAA", "GGAG", "GGGT", "GGGC", "GGGA", "GGGG"};
static char ENCODE_HASH_TWOBIT_RNA[256][5] = {"UUUU", "UUUC", "UUUA", "UUUG", "UUCU", "UUCC", "UUCA", "UUCG", "UUAU", "UUAC", "UUAA", "UUAG", "UUGU", "UUGC", "UUGA", "UUGG", "UCUU", "UCUC", "UCUA", "UCUG", "UCCU", "UCCC", "UCCA", "UCCG", "UCAU", "UCAC", "UCAA", "UCAG", "UCGU", "UCGC", "UCGA", "UCGG", "UAUU", "UAUC", "UAUA", "UAUG", "UACU", "UACC", "UACA", "UACG", "UAAU", "UAAC", "UAAA", "UAAG", "UAGU", "UAGC", "UAGA", "UAGG", "UGUU", "UGUC", "UGUA", "UGUG", "UGCU", "UGCC", "UGCA", "UGCG", "UGAU", "UGAC", "UGAA", "UGAG", "UGGU", "UGGC", "UGGA", "UGGG", "CUUU", "CUUC", "CUUA", "CUUG", "CUCU", "CUCC", "CUCA", "CUCG", "CUAU", "CUAC", "CUAA", "CUAG", "CUGU", "CUGC", "CUGA", "CUGG", "CCUU", "CCUC", "CCUA", "CCUG", "CCCU", "CCCC", "CCCA", "CCCG", "CCAU", "CCAC", "CCAA", "CCAG", "CCGU", "CCGC", "CCGA", "CCGG", "CAUU", "CAUC", "CAUA", "CAUG", "CACU", "CACC", "CACA", "CACG", "CAAU", "CAAC", "CAAA", "CAAG", "CAGU", "CAGC", "CAGA", "CAGG", "CGUU", "CGUC", "CGUA", "CGUG", "CGCU", "CGCC", "CGCA", "CGCG", "CGAU", "CGAC", "CGAA", "CGAG", "CGGU", "CGGC", "CGGA", "CGGG", "AUUU", "AUUC", "AUUA", "AUUG", "AUCU", "AUCC", "AUCA", "AUCG", "AUAU", "AUAC", "AUAA", "AUAG", "AUGU", "AUGC", "AUGA", "AUGG", "ACUU", "ACUC", "ACUA", "ACUG", "ACCU", "ACCC", "ACCA", "ACCG", "ACAU", "ACAC", "ACAA", "ACAG", "ACGU", "ACGC", "ACGA", "ACGG", "AAUU", "AAUC", "AAUA", "AAUG", "AACU", "AACC", "AACA", "AACG", "AAAU", "AAAC", "AAAA", "AAAG", "AAGU", "AAGC", "AAGA", "AAGG", "AGUU", "AGUC", "AGUA", "AGUG", "AGCU", "AGCC", "AGCA", "AGCG", "AGAU", "AGAC", "AGAA", "AGAG", "AGGU", "AGGC", "AGGA", "AGGG", "GUUU", "GUUC", "GUUA", "GUUG", "GUCU", "GUCC", "GUCA", "GUCG", "GUAU", "GUAC", "GUAA", "GUAG", "GUGU", "GUGC", "GUGA", "GUGG", "GCUU", "GCUC", "GCUA", "GCUG", "GCCU", "GCCC", "GCCA", "GCCG", "GCAU", "GCAC", "GCAA", "GCAG", "GCGU", "GCGC", "GCGA", "GCGG", "GAUU", "GAUC", "GAUA", "GAUG", "GACU", "GACC", "GACA", "GACG", "GAAU", "GAAC", "GAAA", "GAAG", "GAGU", "GAGC", "GAGA", "GAGG", "GGUU", "GGUC", "GGUA", "GGUG", "GGCU", "GGCC", "GGCA", "GGCG", "GGAU", "GGAC", "GGAA", "GGAG", "GGGU", "GGGC", "GGGA", "GGGG"};



class twobit_byte_dna : public twobit_byte
{
public:
    twobit_byte_dna(): twobit_byte(ENCODE_HASH_TWOBIT_DNA) {    }
};



class twobit_byte_rna : public twobit_byte
{
public:
    twobit_byte_rna(): twobit_byte(ENCODE_HASH_TWOBIT_RNA) {    }
};






#endif
