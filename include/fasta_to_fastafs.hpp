
#include <vector>

#include "config.hpp"

#include "twobit_byte.hpp"

class fasta_to_fastafs_seq
{
    private:
    twobit_byte *twobit_data;

    bool previous_was_N;

public:
    std::vector<unsigned char> twobits;

    fasta_to_fastafs_seq(void);
    ~fasta_to_fastafs_seq(void);

    std::string name;
    unsigned int n;// effective size in nt
    unsigned int N;// effective size of unnknown nulceotides (N's) in nt
    std::vector<unsigned int> n_starts;
    std::vector<unsigned int> n_ends;


    size_t size(void);// size of compressed data, may be longer than 4*this->n


    void add_N();
    void add_nucleotide(unsigned char);
    void add_twobit(twobit_byte &);
    void flush_reading();
    void close_reading();

    void print(void);
};



class fasta_to_fastafs
{
    std::string *filename = nullptr;

    std::vector<fasta_to_fastafs_seq *> data;

public:
    fasta_to_fastafs(std::string *fname);
    ~fasta_to_fastafs();
    int cache(void);
    void print();
    void write(std::string);
    unsigned int get_index_size();
    unsigned int get_sequence_offset(unsigned int);
};
