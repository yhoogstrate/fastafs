
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



size_t f2fs(const std::string, const std::string);

