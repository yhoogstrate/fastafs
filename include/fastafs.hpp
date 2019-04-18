
#include <vector>

#include <openssl/sha.h>

#include "utils.hpp"

#ifndef FASTAFS_HPP
#define FASTAFS_HPP


struct ffs2f_init_seq {
    // fasta seq size
    // fasta seq newlines/padding lines
    const uint32_t padding;// padding used for this sequence, cannot be 0
    const uint32_t total_sequence_containing_lines;// calculate total number of full nucleotide lines: (this->n + padding - 1) / padding

    std::vector<uint32_t> n_starts;
    std::vector<uint32_t> n_ends;

    ffs2f_init_seq(const uint32_t padding, size_t size, const uint32_t n_lines): padding(padding), total_sequence_containing_lines(n_lines), n_starts(size), n_ends(size) {}
};

struct ffs2f_init {
    const uint32_t padding_arg;// padding argument, 0 means no padding takes place and all nucleotides are written to one single line
    std::vector<ffs2f_init_seq *> sequences;

    ffs2f_init(size_t size, uint32_t padding_arg): padding_arg(padding_arg), sequences(size) {}
};


class fastafs_seq
{
public:
    ffs2f_init_seq* init_ffs2f_seq(uint32_t padding);

    std::string name;//may not exceed 255 chars in current datatype
    uint32_t data_position;// file offset to start reading sequence data
    uint32_t n;// number nucleotides
    std::vector<uint32_t> n_starts;// start positions 0-based)
    std::vector<uint32_t> n_ends;// end positions (is 0-based, must become 1-based)
    uint16_t flag;

    std::vector<std::pair<uint32_t, uint32_t>> m_blocks;// @ todo check if unsiged int[2] is not more efficient / less bloated

    unsigned char sha1_digest[SHA_DIGEST_LENGTH];//this is the binary encoded sha1 hash, not the ascii decoded
    // masked not -yet- needed||implemented

    fastafs_seq();

    uint32_t fasta_filesize(uint32_t padding);
    void view_fasta(ffs2f_init_seq*, std::ifstream *);
    //uint32_t view_fasta_chunk_cached( char *, off_t, size_t, std::ifstream *);//@todo order of off_t and size_t needs to be identical to view chunk in fastafs::
    uint32_t view_fasta_chunk(uint32_t, char *, off_t, size_t, std::ifstream *);//@todo order of off_t and size_t needs to be identical to view chunk in fastafs::
    uint32_t view_fasta_chunk_cached(ffs2f_init_seq*, char *, size_t, off_t, std::ifstream *);

    std::string sha1(std::ifstream *);

    uint32_t n_twobits();

    static uint32_t n_padding(uint32_t, uint32_t, uint32_t);
    bool get_n_offset(uint32_t, uint32_t *);
};



struct fastafs_metadata {
    std::string name;
    std::string uid;
};


class fastafs
{

public:
    ffs2f_init* init_ffs2f(uint32_t padding);

    fastafs(std::string);
    ~fastafs();

    std::string name;
    std::string filename;
    std::vector<fastafs_seq *> data;
    uint16_t flag;

    uint32_t n();

    std::string basename();

    void load(std::string);
    void view_fasta(ffs2f_init*);
    uint32_t view_fasta_chunk_cached(ffs2f_init*, char*, size_t, off_t);
    uint32_t view_fasta_chunk(uint32_t, char *, size_t, off_t);
    uint32_t view_faidx_chunk(uint32_t, char *, size_t, off_t);

    uint32_t view_ucsc2bit_chunk(char *, size_t, off_t);
    off_t ucsc2bit_filesize(void);

    std::string get_faidx(uint32_t);
    uint32_t fasta_filesize(uint32_t);

    int info(bool);
    int check_integrity();
};


// Constants:
using namespace std::literals;
static const std::string UCSC2BIT_MAGIC = "\x43\x27\x41\x1a"s;
static const std::string UCSC2BIT_VERSION = "\x00\x00\x00\x00"s;

static const std::string FASTAFS_MAGIC = "\x0F\x0A\x46\x53"s;
static const std::string FASTAFS_VERSION = "\x00\x00\x00\x00"s;

static const int READ_BUFFER_SIZE = 4096;

#endif
