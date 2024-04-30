
#include <stdio.h>
#include <string.h>

#include <vector>
#include <openssl/evp.h>

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

    ucsc2bit_seq_header():
        name_size(0), name(nullptr), n_blocks(0) { }
};

struct ucsc2bit_seq_header_conversion_data {
    // the followin should be member of a conversion struct, because they're not related to the original 2bit format:
    EVP_MD_CTX *mdctx;
    //MD5_CTX ctx; // needs to be initialized in a constructor, if possible
    unsigned char md5_digest[MD5_DIGEST_LENGTH];

    uint32_t N;// number of N (unknown) nucleotides (n - N = total 2bit compressed nucleotides)

    off_t file_offset_dna_in_ucsc2bit; // file positions where sequence data blocks start

    ucsc2bit_seq_header_conversion_data(): N(0)
    {
        this->mdctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(this->mdctx, EVP_md5(), NULL);
        
        //MD5_Init(&this->ctx);
    }
};


// think of having this as a off_t returning function, returning the written bytes
size_t ucsc2bit_to_fastafs(std::string, std::string);



