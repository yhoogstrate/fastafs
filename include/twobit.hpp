
#include <vector>

#include "twobit_seq.hpp"

class twobit
{
    std::string *filename = nullptr;

    std::vector<twobit_seq *> data;

public:
    twobit(std::string *fname);
    ~twobit();
    int cache(void);
    void print();
    void write(std::string);
    unsigned int get_index_size();
    unsigned int get_sequence_offset(unsigned int);
};
