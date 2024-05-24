
#include <vector>

#include <openssl/evp.h>

#include "config.hpp"
#include "utils.hpp"

#include "fastafs.hpp"
#include "twobit_byte.hpp"
#include "fourbit_byte.hpp"
#include "fivebit_fivebytes.hpp"




class fasta_to_fastafs_seq
{
public:

    void add_unknown();

    void finish_sequence(std::ofstream &);

    void twobit_add(unsigned char, std::ofstream &);
    void twobit_finish_sequence(std::ofstream &);

    void fourbit_add(unsigned char, std::ofstream &);
    void fourbit_finish_sequence(std::ofstream &);

    void fivebit_add(unsigned char, std::ofstream &);
    void fivebit_finish_sequence(std::ofstream &);

    off_t file_offset_in_fasta; // file positions in FASTA file where sequence data blocks starts [ACTG]
    off_t file_offset_in_fastafs; // file positions in FASTAFS file where sequence data blocks starts [2bit/4bit]
    std::string name;

    uint32_t N;// number of N (unknown) nucleotides (n - N = total 2bit compressed nucleotides)
    uint32_t n_actg;// number of non-N nucleotides (any [ACTGU])

    size_t N_bytes_used();// total number of bytes needed to store N's
    size_t twobit_bytes_used();// total number of bytes needed to store xBits

    bool previous_was_N;




    // all below are undefined at initialization
    uint32_t padding;

    // the followin should be member of a conversion struct, because they're not related to the original 2bit format:
    EVP_MD_CTX *mdctx;
    unsigned char md5_digest[MD5_DIGEST_LENGTH];

    std::vector<uint32_t> n_block_starts;
    std::vector<uint32_t> n_block_ends;

    std::vector<uint32_t> m_block_starts;
    std::vector<uint32_t> m_block_ends;
    bool in_m_block;

    char current_dict;


    bool has_T;
    bool has_U;


    twobit_byte twobit_data;
    fourbit_byte fourbit_data;
    fivebit_fivebytes fivebit_data;


    fasta_to_fastafs_seq(off_t fof_fasta, off_t fof_fastafs, const std::string &name):
        file_offset_in_fasta(fof_fasta),
        file_offset_in_fastafs(fof_fastafs),
        name(name),
        N(0),
        n_actg(0),
        previous_was_N(false),
        in_m_block(false),
        current_dict(DICT_TWOBIT),
        has_T(false),
        has_U(false),
        twobit_data(ENCODE_HASH_TWOBIT_DNA) // not relevant for encoding, only for decoding
    {
        if(name.size() > 255) {
            fprintf(stderr, "[fasta_to_fastafs::init] sequence name truncated to 255 charaters: %s\n", name.c_str());
            this->name = this->name.substr(0, 255);
        }

        this->mdctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(this->mdctx, EVP_md5(), NULL);
    }

    void flush();
};


size_t fasta_to_fastafs(const std::string &, const std::string &, bool auto_recompress_to_fourbit);

