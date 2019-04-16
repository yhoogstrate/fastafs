
#include <vector>

#include <openssl/sha.h>

#include "config.hpp"
#include "utils.hpp"

#include "fastafs.hpp"
#include "twobit_byte.hpp"

class fasta_to_fastafs_seq : public fastafs_seq
{
private:
    twobit_byte twobit_data;

    bool previous_was_N;

public:
    std::vector<unsigned char> twobits;

    unsigned int N;// effective size of unnknown nulceotides (N's) in nt

    fasta_to_fastafs_seq(void);

    size_t size(void);// size of compressed data, may be longer than 4*this->n

    void add_N();
    void add_nucleotide(unsigned char);
    void add_twobit(twobit_byte &);
    void flush_reading();
    void close_reading();

    SHA_CTX ctx;
    //unsigned char sha1_digest[SHA_DIGEST_LENGTH];
};



class fasta_to_fastafs
{
    std::string name;
    std::string filename;

    std::vector<fasta_to_fastafs_seq *> data;

public:
    fasta_to_fastafs(std::string, std::string);
    ~fasta_to_fastafs();
    int cache(void);
    void write(std::string);
    unsigned int get_index_size();
};




class fasta_seq_header_conversion_data {
    public:
        off_t file_offset_in_fasta; // file positions where sequence data blocks start
        std::string name;
        
        uint32_t N;// number of N (unknown) nucleotides (n - N = total 2bit compressed nucleotides)

        bool previous_was_N;
        int twobit_offset; // ranging from 0 to 3


        fasta_seq_header_conversion_data(off_t fof, std::string name):
            file_offset_in_fasta(fof),
            name(name),
            N(0),
            previous_was_N(false)
            { }


        // all below are undefined at initialization
        uint32_t padding;

        // the followin should be member of a conversion struct, because they're not related to the original 2bit format:
        SHA_CTX ctx;
        unsigned char sha1_digest[SHA_DIGEST_LENGTH];

        uint32_t n_blocks;
        std::vector<uint32_t> n_block_starts;
        std::vector<uint32_t> n_block_sizes;

        uint32_t m_blocks;
        std::vector<uint32_t> m_block_starts;
        std::vector<uint32_t> m_block_sizes;


        twobit_byte twobit_data;
};


size_t f2fs(const std::string, const std::string);

