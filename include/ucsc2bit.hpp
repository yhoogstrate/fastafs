
#include <vector>


#include "utils.hpp"

#ifndef UCSC2BIT_HPP
#define UCSC2BIT_HPP




class ucsc2bit_seq
{
public:
    explicit ucsc2bit_seq();

    std::string name;//may not exceed 255 chars in current datatype
    uint32_t data_position;// file offset to start reading sequence data
    uint32_t sequence_data_position;// file offset to start reading sequence data

    uint32_t n;// number nucleotides [ACTG + N]

    std::vector<uint32_t> n_starts;// start positions (nucleotide positions; 0-based)
    std::vector<uint32_t> n_ends;// end positions (nucleotide positions; 0-based)

    std::vector<uint32_t> m_starts;// start positions (nucleotide positions; 0-based)
    std::vector<uint32_t> m_ends;// end positions (nucleotide positions; 0-based)

    uint32_t fasta_filesize(uint32_t);
    uint32_t n_padding(uint32_t, uint32_t, uint32_t);

    uint32_t view_fasta_chunk(uint32_t, char*, size_t, off_t, std::ifstream*);
};



class ucsc2bit
{
public:

    explicit ucsc2bit(std::string);
    ~ucsc2bit();

    std::string name;// needed as basename for mounting
    std::string filename;

    void load(std::string);

    std::vector<ucsc2bit_seq*> data;


    uint32_t n();
    size_t fasta_filesize(uint32_t);


    uint32_t view_fasta_chunk(uint32_t, char*, size_t, off_t);

    /*
        void view_fasta(ffs2f_init*);
        std::string get_faidx(uint32_t);//@todo get rid of this, make it full chunked
        */
};



#endif
