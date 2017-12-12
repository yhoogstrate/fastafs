
#include <vector>

#include "utils.hpp"


class fastafs_seq
{
public:
    std::string name;//may not exceed 255 chars in current datatype
    unsigned int data_position;
    unsigned int n;// number nucleotides
    std::vector<unsigned int> n_starts;
    std::vector<unsigned int> n_ends;

    // masked not -yet- needed

    fastafs_seq();

    void view(unsigned int, std::ifstream *);
    std::string sha1(std::ifstream *);
    unsigned int n_twobits();
};



struct fastafs_metadata {
    std::string name;
    std::string uid;
};


class fastafs
{

public:
    ~fastafs();
    
    std::string filename;
    std::vector<fastafs_seq *> data;
    unsigned int n();

    std::string basename();

    void load(std::string); // loads
    //void write_2bit_header(fstream);
    //unsigned int get_index_size();
    //unsigned int get_sequence_offset(unsigned int);
    void view(unsigned int);
    void info();
    
    void mount();
};
