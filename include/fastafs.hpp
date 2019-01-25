
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
    std::vector<unsigned int> n_starts;
    std::vector<unsigned int> n_ends;
    unsigned char sha1_digest[SHA_DIGEST_LENGTH];//this is the binary encoded sha1 hash, not the ascii decoded
    // masked not -yet- needed||implemented

    fastafs_seq();

    void view_fasta(unsigned int, std::ifstream *);
    int view_fasta_chunk(unsigned int, char *, off_t, size_t, std::ifstream *);
    unsigned int fasta_filesize(unsigned int padding);

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
    int view_fasta_chunk(unsigned int, char *, size_t, off_t );
    int view_faidx_chunk(unsigned int, char *, size_t, off_t );

    std::string get_faidx(unsigned int);
    unsigned int fasta_filesize(unsigned int);

    void info();
};

#endif
