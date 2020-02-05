
#include <vector>

#include <openssl/md5.h>

#include "config.hpp"
#include "utils.hpp"

#include "fastafs.hpp"
#include "twobit_byte.hpp"



class fasta_seq_header_twobit_conversion_data
{
public:
    void add_ACTG(unsigned char, std::ofstream &);//Adds a T or a U
    void add_N();
    void finish_sequence(std::ofstream &);

    off_t file_offset_in_fasta; // file positions in FASTA file where sequence data blocks starts [ACTG]
    off_t file_offset_in_fastafs; // file positions in FASTAFS file where sequence data blocks starts [2bit/4bit]
    std::string name;

    uint32_t N;// number of N (unknown) nucleotides (n - N = total 2bit compressed nucleotides)
    uint32_t n_actg;// number of non-N nucleotides (any [ACTGU])

    bool previous_was_N;

    char current_dict;

    fasta_seq_header_twobit_conversion_data(off_t fof_fasta, off_t fof_fastafs, const std::string &name):
        file_offset_in_fasta(fof_fasta),
        file_offset_in_fastafs(fof_fastafs),
        name(name),
        N(0),
        n_actg(0),
        previous_was_N(false),
        in_m_block(false),
        current_dict(DICT_TWOBIT)
    {
        MD5_Init(&this->ctx);
    }


    // all below are undefined at initialization
    uint32_t padding;

    // the followin should be member of a conversion struct, because they're not related to the original 2bit format:
    MD5_CTX ctx;
    unsigned char md5_digest[MD5_DIGEST_LENGTH];

    std::vector<uint32_t> n_block_starts;
    std::vector<uint32_t> n_block_ends;

    std::vector<uint32_t> m_block_starts;
    std::vector<uint32_t> m_block_ends;
    bool in_m_block;

    twobit_byte twobit_data;
};


size_t fasta_to_twobit_fastafs(const std::string &, const std::string &);

