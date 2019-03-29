
#include <vector>

#include <openssl/sha.h>

#include "utils.hpp"

#ifndef FASTAFS_HPP
#define FASTAFS_HPP


struct ffs2f_init_seq {
    // fasta seq size
    // fasta seq newlines/padding lines
    const unsigned int total_sequence_containing_lines;// calculate total number of full nucleotide lines: (this->n + padding - 1) / padding

    std::vector<unsigned int> n_starts;
    std::vector<unsigned int> n_ends;

    ffs2f_init_seq(size_t size, const unsigned int n_lines): total_sequence_containing_lines(n_lines), n_starts(size), n_ends(size) {}
};

struct ffs2f_init {
    unsigned int padding;
    std::vector<ffs2f_init_seq *> sequences;

    ffs2f_init(size_t size, unsigned int padding): padding(padding), sequences(size) {}
};


class fastafs_seq
{
public:
    ffs2f_init_seq* init_ffs2f_seq(unsigned int padding);

    std::string name;//may not exceed 255 chars in current datatype
    unsigned int data_position;// file offset to start reading sequence data
    unsigned int n;// number nucleotides
    std::vector<unsigned int> n_starts;// start positions 0-based)
    std::vector<unsigned int> n_ends;// end positions (is 0-based, must become 1-based)

    std::vector<std::pair<unsigned int, unsigned int>> m_blocks;// @ todo check if unsiged int[2] is not more efficient / less bloated

    unsigned char sha1_digest[SHA_DIGEST_LENGTH];//this is the binary encoded sha1 hash, not the ascii decoded
    // masked not -yet- needed||implemented

    fastafs_seq();

    unsigned int fasta_filesize(unsigned int padding);
    void view_fasta(unsigned int, std::ifstream *);
    //unsigned int view_fasta_chunk_cached( char *, off_t, size_t, std::ifstream *);//@todo order of off_t and size_t needs to be identical to view chunk in fastafs::
    unsigned int view_fasta_chunk(unsigned int, char *, off_t, size_t, std::ifstream *);//@todo order of off_t and size_t needs to be identical to view chunk in fastafs::
    unsigned int view_fasta_chunk_cached(unsigned int, char *, off_t, size_t, std::ifstream *, ffs2f_init_seq*);

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
    ffs2f_init* init_ffs2f(unsigned int padding);

    fastafs(std::string);
    ~fastafs();

    std::string name;
    std::string filename;
    std::vector<fastafs_seq *> data;
    unsigned int n();

    std::string basename();

    void load(std::string);
    void view_fasta(unsigned int);
    unsigned int view_fasta_chunk_cached(ffs2f_init*, char *, size_t, off_t);
    unsigned int view_fasta_chunk(unsigned int, char *, size_t, off_t);
    unsigned int view_faidx_chunk(unsigned int, char *, size_t, off_t);

    unsigned int view_ucsc2bit_chunk(char *, size_t, off_t);
    off_t ucsc2bit_filesize(void);

    std::string get_faidx(unsigned int);
    unsigned int fasta_filesize(unsigned int);

    int info(bool);
    int check_integrity();
};


// Constants:
using namespace std::literals;
static const std::string UCSC2BIT_MAGIC = "\x43\x27\x41\x1a"s;
static const std::string UCSC2BIT_VERSION = "\x00\x00\x00\x00"s;

static const std::string FASTAFS_MAGIC = "\x0F\x0A\x46x53"s;
static const std::string FASTAFS_VERSION = "\x00\x00\x00\x00"s;

static const int READ_BUFFER_SIZE = 4096;

#endif
