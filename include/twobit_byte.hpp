
#ifndef TWOBIT_BYTE_HPP
#define TWOBIT_BYTE_HPP

#include <array>
#include <cstdint>
#include <cstring>
#include "config.hpp"

#include "chunked_reader.hpp"


// Genereer een uint32_t decode-tabel voor twobit op compilatietijd.
// Elke entry bevat 4 opeenvolgende nucleotiden gepakt als little-endian uint32_t,
// zodat memcpy(output, &val, 4) ze in de juiste volgorde schrijft op x86-64.
// t_or_u = 'T' voor DNA, 'U' voor RNA.
constexpr std::array<uint32_t, 256> make_twobit_decode_table(char t_or_u)
{
    std::array<uint32_t, 256> table = {};
    const uint32_t nuc[4] = {
        static_cast<uint32_t>(t_or_u),
        static_cast<uint32_t>('C'),
        static_cast<uint32_t>('A'),
        static_cast<uint32_t>('G')
    };
    for(uint32_t i = 0; i < 256u; i++) {
        table[i] = nuc[(i >> 6u) & 3u]
                 | (nuc[(i >> 4u) & 3u] << 8u)
                 | (nuc[(i >> 2u) & 3u] << 16u)
                 | (nuc[i & 3u]         << 24u);
    }
    return table;
}

inline constexpr auto DECODE_TWOBIT_DNA_U32 = make_twobit_decode_table('T');
inline constexpr auto DECODE_TWOBIT_RNA_U32 = make_twobit_decode_table('U');


class twobit_byte
{
public:
    static const char n_fill_unmasked = 'N';
    static const char n_fill_masked = 'n';
    static const unsigned char bits_per_nucleotide = 2;

    unsigned char data;

    char (&encode_hash)[256][5];
    twobit_byte(char (&encode_hash_arg)[256][5]);

    static const char nucleotides_per_byte = 8 / twobit_byte::bits_per_nucleotide;
    static const char nucleotides_per_chunk = 8 / twobit_byte::bits_per_nucleotide;

    void set(unsigned char, unsigned char);
    void set(char*);
    char *get(void);
    size_t get(unsigned char length, char *output);

    static unsigned char iterator_to_offset(unsigned int);

    static off_t nucleotides_to_compressed_fileoffset(size_t);
    static off_t nucleotides_to_compressed_offset(size_t);

    void next(chunked_reader &);
};




extern char ENCODE_HASH_TWOBIT_DNA[256][5];
extern char ENCODE_HASH_TWOBIT_RNA[256][5];



class twobit_byte_dna : public twobit_byte
{
public:
    twobit_byte_dna();
};



class twobit_byte_rna : public twobit_byte
{
public:
    twobit_byte_rna();
};






#endif
