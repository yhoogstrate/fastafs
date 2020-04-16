

#ifndef FASTAFS_HPP
#define FASTAFS_HPP



#include <vector>

#include <openssl/sha.h>
#include <openssl/md5.h>

#include "utils.hpp"

#include "sequence_region.hpp"
#include "flags.hpp"
#include "chunked_reader.hpp"


struct ffs2f_init_seq {
    // fasta seq size
    // fasta seq newlines/padding lines
    const uint32_t padding;// padding used for this sequence, cannot be 0
    const uint32_t total_sequence_containing_lines;// calculate total number of full nucleotide lines: (this->n + padding - 1) / padding

    std::vector<uint32_t> n_starts;// file position based
    std::vector<uint32_t> n_ends;// file position based

    std::vector<uint32_t> m_starts;// file position based
    std::vector<uint32_t> m_ends;// file position based

    ffs2f_init_seq(const uint32_t padding, size_t n_blocks, size_t m_blocks, const uint32_t n_lines):
        padding(padding),
        total_sequence_containing_lines(n_lines),
        n_starts(n_blocks), n_ends(n_blocks),
        m_starts(m_blocks), m_ends(m_blocks)
    {}
};

struct ffs2f_init {
    const uint32_t padding_arg;// padding argument, 0 means no padding takes place and all nucleotides are written to one single line
    std::vector<ffs2f_init_seq *> sequences;

    ffs2f_init(size_t size, uint32_t padding_arg): padding_arg(padding_arg), sequences(size) {}

    ~ffs2f_init(void)
    {
        for(size_t i = 0; i < sequences.size(); i++) {
            delete sequences[i];
        }
    }
};


class fastafs_seq
{
public:
    ffs2f_init_seq* init_ffs2f_seq(uint32_t, bool);

    std::string name;//may not exceed 255 chars in current datatype
    uint32_t data_position;// file offset to start reading sequence data
    uint32_t n;// number nucleotides
    std::vector<uint32_t> n_starts;// start positions (nucleotide positions; 0-based)
    std::vector<uint32_t> n_ends;// end positions (nucleotide positions; 0-based)
    fastafs_sequence_flags flags;

    std::vector<uint32_t> m_starts;// start positions (nucleotide positions; 0-based)
    std::vector<uint32_t> m_ends;// end positions (nucleotide positions; 0-based)

    // those red from fastafs file
    unsigned char sha1_digest[SHA_DIGEST_LENGTH];//this is the binary encoded sha1 hash, not the ascii decoded
    unsigned char md5_digest[MD5_DIGEST_LENGTH];//this is the binary encoded md5 hash, not the ascii decoded

    fastafs_seq();

    uint32_t fasta_filesize(uint32_t padding);
    void view_fasta(ffs2f_init_seq*, chunked_reader &fh);

    size_t view_sequence_region_size(ffs2f_init_seq*, sequence_region*, std::ifstream *);
    uint32_t view_sequence_region(ffs2f_init_seq*, sequence_region*, char *, size_t, off_t, chunked_reader &);
    uint32_t view_fasta_chunk(ffs2f_init_seq*, char *, size_t, off_t, chunked_reader &);
    template <class T> uint32_t view_fasta_chunk_generalized(ffs2f_init_seq*, char *, size_t, off_t, chunked_reader &);

    std::string sha1(ffs2f_init_seq*, chunked_reader &);// sha1 works 'fine' but is, like md5, sensitive to length extension hacks and should actually not be used for identifiers.
    std::string md5(ffs2f_init_seq*, chunked_reader &);// md5 works 'fine' but is, like sha1, sensitive to length extension hacks and should actually not be used for identifiers.

    uint32_t n_bits();

    static uint32_t n_padding(uint32_t, uint32_t, uint32_t);
    bool get_n_offset(uint32_t, uint32_t *);
};


/*
struct fastafs_metadata {
    std::string name;
    std::string uid;
};
*/


class fastafs
{

public:
    ffs2f_init* init_ffs2f(uint32_t, bool);

    explicit fastafs(std::string);
    ~fastafs();

    std::string name;
    std::string filename;
    std::vector<fastafs_seq*> data;
    uint32_t crc32f;// crc32 as found in fastafs file

    fastafs_flags flags;

    uint32_t n();// number nucleotdies

    std::string basename();

    void load(std::string);
    void view_fasta(ffs2f_init*);

    size_t view_sequence_region_size(ffs2f_init*, const char *); // read stuff like "chr1:123-456" into the buffer
    uint32_t view_sequence_region(ffs2f_init*, const char *, char*, size_t, off_t); // read stuff like "chr1:123-456" into the buffer
    uint32_t view_fasta_chunk(ffs2f_init*, char*, size_t, off_t, chunked_reader &);
    uint32_t view_fasta_chunk(ffs2f_init*, char*, size_t, off_t);
    uint32_t view_faidx_chunk(uint32_t, char *, size_t, off_t);
    uint32_t view_ucsc2bit_chunk(char *, size_t, off_t);
    size_t   view_dict_chunk(char *, size_t, off_t);

    uint32_t get_crc32(void);// returns a 'new' crc32, estimated on file contents
    size_t fastafs_filesize(void);
    size_t fasta_filesize(uint32_t);
    size_t ucsc2bit_filesize(void);
    size_t dict_filesize(void);

    std::string get_faidx(uint32_t);//@todo get rid of this, make it full chunked

    int info(bool);
    bool check_file_integrity(bool);
    bool check_sequence_integrity(bool);
};



#endif
