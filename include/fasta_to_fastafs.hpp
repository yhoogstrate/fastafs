
#include <vector>

#include <openssl/sha.h>

#include "config.hpp"
#include "utils.hpp"

#include "fastafs.hpp"
#include "twobit_byte.hpp"



class fasta_seq_header_conversion_data
{
public:
    void add_ACTG(unsigned char, std::ofstream &);//Adds a T or a U
    void add_N();
    void finish_sequence(std::ofstream &);

    off_t file_offset_in_fasta; // file positions where sequence data blocks start
    std::string name;

    uint32_t N;// number of N (unknown) nucleotides (n - N = total 2bit compressed nucleotides)
    uint32_t n_actg;// number of non-N nucleotides (any [ACTGU])

    bool previous_was_N;


    fasta_seq_header_conversion_data(off_t fof, std::string name):
        file_offset_in_fasta(fof),
        name(name),
        N(0),
        n_actg(0),
        previous_was_N(false)
    {
        SHA1_Init(&this->ctx);
    }


    // all below are undefined at initialization
    uint32_t padding;

    // the followin should be member of a conversion struct, because they're not related to the original 2bit format:
    SHA_CTX ctx;
    unsigned char sha1_digest[SHA_DIGEST_LENGTH];

    uint32_t n_blocks;
    std::vector<uint32_t> n_block_starts;
    std::vector<uint32_t> n_block_ends;

    uint32_t m_blocks;
    std::vector<uint32_t> m_block_starts;
    std::vector<uint32_t> m_block_ends;


    twobit_byte twobit_data;
};


size_t fasta_to_fastafs(const std::string, const std::string);

