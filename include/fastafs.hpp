
#include <vector>

#include <openssl/sha.h>

#include "utils.hpp"

#ifndef FASTAFS_HPP
#define FASTAFS_HPP

class fastafs_seq
{
public:
    std::string name;//may not exceed 255 chars in current datatype
    unsigned int data_position;
    unsigned int n;// number nucleotides
    std::vector<unsigned int> n_starts;// start positions (0-based)
    std::vector<unsigned int> n_ends;// end positions (1-based)
    unsigned char sha1_digest[SHA_DIGEST_LENGTH];//this is the binary encoded sha1 hash, not the ascii decoded
    // masked not -yet- needed||implemented

    fastafs_seq();

    unsigned int fasta_filesize(unsigned int padding);
    void view_fasta(unsigned int, std::ifstream *);
    unsigned int view_fasta_chunk(unsigned int, char *, off_t, size_t, std::ifstream *);//@todo order of off_t and size_t needs to be identical to view chunk in fastafs::

    std::string sha1(std::ifstream *);

    unsigned int n_twobits();

    static unsigned int n_padding(unsigned int, unsigned int, unsigned int);
    bool get_n_offset(unsigned int, unsigned int *);
};



struct fastafs_metadata {
    std::string name;
    std::string uid;
};


class fastafs
{

public:
    fastafs(std::string);
    ~fastafs();

    std::string name;
    std::string filename;
    std::vector<fastafs_seq *> data;
    unsigned int n();

    std::string basename();

    void load(std::string);
    void view_fasta(unsigned int);
    unsigned int view_fasta_chunk(unsigned int, char *, size_t, off_t );
    unsigned int view_faidx_chunk(unsigned int, char *, size_t, off_t );
    
    unsigned int view_ucsc2bit_chunk(char *, size_t, off_t);
    unsigned int ucsc2bit_filesize(void);

    std::string get_faidx(unsigned int);
    unsigned int fasta_filesize(unsigned int);

    int info(bool);
    int check_integrity();
};

#endif
