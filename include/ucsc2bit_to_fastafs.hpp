
#include <stdio.h>
#include <string.h>

#include <vector>
#include <openssl/sha.h>

#include "config.hpp"
#include "utils.hpp"

#include "fastafs.hpp"
#include "twobit_byte.hpp"



struct ucsc2bit_seq_header {
    unsigned char name_size;
    char *name;
    uint32_t offset;// in file, in bytes

    uint32_t dna_size;

    uint32_t n_blocks;
    std::vector<uint32_t> n_block_starts;
    std::vector<uint32_t> n_block_sizes;

    uint32_t m_blocks;
    std::vector<uint32_t> m_block_starts;
    std::vector<uint32_t> m_block_sizes;
};

struct ucsc2bit_seq_header_conversion_data {
    // the followin should be member of a conversion struct, because they're not related to the original 2bit format:
    SHA_CTX ctx;
    unsigned char sha1_digest[SHA_DIGEST_LENGTH];

    uint32_t N;// number of N nucleotides (n - N = total 2bit compressed nucleotides)

    off_t header_position; // file positions where sha1 and offsets are stored
    off_t file_offset_dna_in_ucsc2bit; // file positions where sequence data blocks start
    
    ucsc2bit_seq_header_conversion_data():N(0) { }
};


// think of having this as a off_t returning function, returning the written bytes
void ucsc2bit_to_fastafs(std::string, std::string);




