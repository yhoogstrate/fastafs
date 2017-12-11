
#include <vector>


class twobit_seq_header {
    public:
        std::string name;//may not exceed 255 chars in current datatype
        unsigned int data_position;
        unsigned int n;// number nucleotides
        unsigned int N_regions;// number N regions
        unsigned int n_actg;
        
        // considered actual data:
        std::vector<unsigned int> n_starts;
        std::vector<unsigned int> n_ends;
        
        
        // masked not -yet- needed
        
        void view(unsigned int, std::ifstream *);
};


class twobit_header
{

public:
    std::string *filename = nullptr;
    std::vector<twobit_seq_header *> data;

    void load(std::string *); // loads 
    //void write_2bit_header(fstream);
    //unsigned int get_index_size();
    //unsigned int get_sequence_offset(unsigned int);
    void view(unsigned int);
};
