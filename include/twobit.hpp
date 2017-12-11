
#include <vector>

#include "fasta_to_fastafs_seq.hpp"

class twobit
{
    std::string *filename = nullptr;

    std::vector<fasta_to_fastafs_seq *> data;

public:
    twobit(std::string *fname);
    ~twobit();
    int cache(void);
    void print();
    void write(std::string);
    unsigned int get_index_size();
    unsigned int get_sequence_offset(unsigned int);
};
