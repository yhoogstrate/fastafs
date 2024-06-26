#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
//#include <filesystem>

#include <openssl/evp.h>
#include <openssl/err.h>

// SSL requests to ENA
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <zlib.h> // crc32


#include "config.hpp"

#include "twobit_byte.hpp"
#include "fourbit_byte.hpp"
#include "fivebit_fivebytes.hpp"
#include "fastafs.hpp"
#include "utils.hpp"



static const std::string dict_sq = "@SQ\tSN:";
static const std::string dict_ln = "\tLN:";
static const std::string dict_m5 = "\tM5:";
static const std::string dict_ur = "\tUR:fastafs:///";


fastafs_seq::fastafs_seq(): n(0)
{
}

/*
 * @todo create class fasta(fastafs ~ padding)
 * all fasta properties need to be put outside of the fastafs class
 */
uint32_t fastafs_seq::fasta_filesize(uint32_t padding)
{
#if DEBUG
    if(padding == 0) {
        throw std::invalid_argument("[fastafs_seq::fasta_filesize] Padding is set to 0, should have been set to this->n elsewhere.\n");
    }
#endif
    //               >                   chr                 \n  ACTG NNN  /number of newlines corresponding to ACTG NNN lines

    return 1 + (uint32_t) this->name.size() + 1 + this->n + (this->n + (padding - 1)) / padding;
}



void fastafs_seq::view_fasta(ffs2f_init_seq* cache, chunked_reader &fh)
{
    char buffer[READ_BUFFER_SIZE];// = new char [READ_BUFFER_SIZE];
    uint32_t offset = 0;

    //@todo figure out if a do {} while() loop isn't more in place here?
    uint32_t written = this->view_fasta_chunk(cache, buffer, READ_BUFFER_SIZE, offset, fh);
    while(written > 0) {
        std::cout << std::string(buffer, written);
        offset += written;

        written = this->view_fasta_chunk(cache, buffer, READ_BUFFER_SIZE, offset, fh);
    }
}



ffs2f_init_seq* fastafs_seq::init_ffs2f_seq(const uint32_t padding_arg, bool allow_masking)
{
    uint32_t padding = padding_arg;
    if(padding_arg == 0) { // for writing all sequences to one single line after the header
        if(n == 0) {
            throw std::runtime_error("Empty sequence glitch\n");
        }
        padding = this->n;
    }

    // this can go into the constructor
    const uint32_t total_sequence_containing_lines = (this->n + padding - 1) / padding;// calculate total number of full nucleotide lines
    ffs2f_init_seq* data;

    if(allow_masking) {
        data = new ffs2f_init_seq(padding, this->n_starts.size() + 1,  this->m_starts.size() + 1, total_sequence_containing_lines, this->fasta_filesize(padding));
    } else {
        data = new ffs2f_init_seq(padding, this->n_starts.size() + 1,  1, total_sequence_containing_lines, this->fasta_filesize(padding));
    }

    uint32_t fasta_header_size = (uint32_t) this->name.size() + 2;
    unsigned int max_val = fasta_header_size + this->n + total_sequence_containing_lines + 1;
    size_t block_size;

    // n blocks are stored in the fastafs object per nucleotide position, but as fasta file they need to be calculated as file position
    for(size_t i = 0; i < this->n_starts.size(); i++) {
        data->n_starts[i] = fasta_header_size + this->n_starts[i] + (this->n_starts[i] / padding);
        data->n_ends[i] = fasta_header_size + this->n_ends[i] + (this->n_ends[i] / padding);
    }

    block_size = data->n_starts.size();
    data->n_starts[block_size - 1]  = max_val;
    data->n_ends[block_size - 1] = max_val;

    // m blocks
    if(allow_masking) {
        for(size_t i = 0; i < this->m_starts.size(); i++) {
            data->m_starts[i] = fasta_header_size + this->m_starts[i] + (this->m_starts[i] / padding);
            data->m_ends[i] = fasta_header_size + this->m_ends[i] + (this->m_ends[i] / padding);
        }
    }

    block_size = data->m_starts.size();
    data->m_starts[block_size - 1] = max_val;
    data->m_ends[block_size - 1] = max_val;

    return data;
}



// @todo templating like stuff
uint32_t fastafs_seq::view_fasta_chunk(
    ffs2f_init_seq* cache,
    char *buffer,

    size_t buffer_size,
    off_t start_pos_in_fasta,

    chunked_reader &fh)
{
    uint32_t written_iter;
    uint32_t written = 0;

    do {
        if(this->flags.is_twobit()) {
            if(this->flags.is_dna()) {
                written_iter = this->view_fasta_chunk_generalized<twobit_byte_dna>(cache, buffer + written, buffer_size - written, start_pos_in_fasta + written, fh);
            } else {
                written_iter = this->view_fasta_chunk_generalized<twobit_byte_rna>(cache, buffer + written, buffer_size - written, start_pos_in_fasta + written, fh);
            }
        } else if(this->flags.is_fourbit()) {
            written_iter = this->view_fasta_chunk_generalized<fourbit_byte>(cache, buffer + written, buffer_size - written, start_pos_in_fasta + written, fh);
        } else {
            written_iter = this->view_fasta_chunk_generalized<fivebit_fivebytes>(cache, buffer + written, buffer_size - written, start_pos_in_fasta + written, fh);
        }
        written += written_iter;
    } while((written_iter > 0) and (written < buffer_size));

    return written;
}



/*
 * fastafs_seq::view_fasta_chunk -
 *
 * @padding = number of spaces?
 * @char buffer =
 * @start_pos_in_fasta =
 * @len_to_copy =
 * @fh = filestream to fastafs file
 * @cache = pre calculated values from fastafs_seq::init_ffs2f_seq
 *
 * returns
 *
 * @todo see if this can be a std::ifstream or some kind of stream type of object?
*/
template <class T> inline uint32_t fastafs_seq::view_fasta_chunk_generalized(
    ffs2f_init_seq* cache,
    char *buffer,

    size_t buffer_size,
    off_t start_pos_in_fasta,

    chunked_reader &fh)
{
#if DEBUG
    if(cache == nullptr) {
        throw std::runtime_error("Empty cache was provided\n");
    }
#endif //DEBUG

    buffer_size = std::min((size_t) READ_BUFFER_SIZE, buffer_size);


    T t = T();// nice way of having this templated object on stack :)
    uint32_t written = 0;


    if(written >= buffer_size) {
        // requesting a buffer of size=0, should throw an exception?
        return written;
    }

    uint32_t pos = (uint32_t) start_pos_in_fasta;


    size_t pos_limit = this->name.size() + 2;
    if(pos < pos_limit) {
        const std::string header = ">" + this->name + "\n";

        const uint32_t tocopy = (uint32_t) std::min(pos_limit - pos, buffer_size); // size to be copied
        const uint32_t copied = (uint32_t) header.copy(buffer, tocopy, pos); // effective size of copied data

        written += (uint32_t) copied;

        if(written >= buffer_size) {
            return written;
        }

        pos += (uint32_t) copied;
    }

    const uint32_t offset_from_sequence_line = (uint32_t)(pos - pos_limit);
    size_t n_block = cache->n_starts.size();
    size_t m_block = cache->m_starts.size();
    uint32_t newlines_passed = offset_from_sequence_line / (cache->padding + 1);// number of newlines passed (within the sequence part)
    const uint32_t nucleotide_pos = offset_from_sequence_line - newlines_passed;// requested nucleotide in file

    // calculate file position for next twobit
    // when we are in an OPEN n block, we need to go to the first non-N base after, and place the file pointer there
    uint32_t n_passed = 0;
    this->get_n_offset(nucleotide_pos, &n_passed);
    uint32_t compressed_nucleotide_offset = nucleotide_pos - n_passed; // number of nucleotides [NACT / compressed] behind us
    fh.seek((uint32_t) this->data_position + 4 + T::nucleotides_to_compressed_fileoffset(compressed_nucleotide_offset));
    unsigned char bit_offset = compressed_nucleotide_offset % T::nucleotides_per_chunk;// twobit -> 4, fourbit: -> 2

    /*
     0  0  0  0  1  1  1  1 << desired offset from starting point
     A  C  T  G  A  C  T  G
    *

    handigste is om file pointer naar de byte ervoor te zetten
    vervolgens wanneer bit_offset gelijk is aan nul, lees je de volgende byte
    * nooit out of bound

    */
//    const char *chunk = t.encode_hash[0];// init
//    unsigned char bit_offset = (nucleotide_pos - n_passed) % t.nucleotides_per_byte;// twobit -> 4, fourbit: -> 2

    char *chunk = (char *) t.encode_hash[1];// init

    if(bit_offset != 0) {
        t.next(fh);
        chunk = t.get();
    }
    while(n_block > 0 and pos <= cache->n_ends[n_block - 1]) { // iterate back
        n_block--;
    }
    while(m_block > 0 and pos <= cache->m_ends[m_block - 1]) { // iterate back
        m_block--;
    }

    uint32_t cur_n_end = cache->n_ends[n_block];
    uint32_t cur_n_start = cache->n_starts[n_block];

    uint32_t cur_m_end = cache->m_ends[m_block];
    uint32_t cur_m_start = cache->m_starts[m_block];


    // write sequence
    pos_limit += newlines_passed * (cache->padding + 1);// passed sequence-containg lines
    while(newlines_passed < cache->total_sequence_containing_lines) { // only 'complete' lines that are guarenteed 'padding' number of nucleotides long [ this loop starts at one to be unsigned-safe ]
        pos_limit += std::min(cache->padding, this->n - (newlines_passed * cache->padding));// only last line needs to be smaller ~ calculate from the beginning of newlines_passed

        // write nucleotides
        while(pos < pos_limit) {// while next sequence-containing-line is open
            if(pos >= cur_n_start) {
                if(pos >= cur_m_start) { // IN an m block; lower-case
                    buffer[written++] = T::n_fill_masked;
                } else {
                    buffer[written++] = T::n_fill_unmasked;
                }
            } else {

                if(bit_offset % T::nucleotides_per_chunk == 0) {
                    t.next(fh);
                    chunk = t.get();
                }

                if(pos >= cur_m_start) { // IN an m block; lower-case
                    buffer[written++] = (unsigned char)(chunk[bit_offset] + 32);
                } else {
                    buffer[written++] = chunk[bit_offset];
                }

                bit_offset = (unsigned char)(bit_offset + 1) % T::nucleotides_per_chunk;
            }

            if(pos == cur_n_end) {
                //if(pos == cache->n_ends[n_block]) {
                n_block++;
                cur_n_end = cache->n_ends[n_block];
                cur_n_start = cache->n_starts[n_block];
            }
            if(pos == cur_m_end) {
                //if(pos == cache->m_ends[m_block]) {
                m_block++;
                cur_m_end = cache->m_ends[m_block];
                cur_m_start = cache->m_starts[m_block];
            }

            pos++;

            if(written >= buffer_size) {
                //fh->clear();
                //delete[] from_file_buffer;
                return written;
            }
        }

        // write newline
        pos_limit += 1;
        if(pos < pos_limit) {
            buffer[written++] = '\n';
            pos++;

            if(written >= buffer_size) {
                //fh->clear();
                //delete[] from_file_buffer;
                return written;
            }
        }

        newlines_passed++;
    }

    //fh->clear();
    //delete[] from_file_buffer;
    return written;
}




size_t fastafs_seq::view_sequence_region_size(sequence_region* sr)
{
#if DEBUG
    if(sr == nullptr) {
        throw std::invalid_argument("fastafs_seq::view_sequence_region - error 02\n");
    }
#endif


    size_t total_requested_size;
    if(sr->has_defined_end()) {
        total_requested_size = std::min((size_t) this->n, (size_t) sr->get_end_position() + 1);
    } else {
        total_requested_size = this->n;
    }

    total_requested_size -= sr->get_start_position();
    return total_requested_size;
}

uint32_t fastafs_seq::view_sequence_region(ffs2f_init_seq* cache,
        sequence_region* sr, char *buffer, size_t size, off_t offset,
        chunked_reader &fh)
{
#if DEBUG
    if(cache == nullptr) {
        throw std::invalid_argument("fastafs_seq::view_sequence_region - error 01\n");
    }

    if(sr == nullptr) {
        throw std::invalid_argument("fastafs_seq::view_sequence_region - error 02\n");
    }

    if(size == 0) { // requestedsize must be larger than 0
        throw std::invalid_argument("fastafs_seq::view_sequence_region - error 03\n");
    }

#endif

    uint32_t written = 0;

    size_t total_requested_size;
    if(sr->has_defined_end()) {
        total_requested_size = std::min((size_t) this->n, (size_t) sr->get_end_position() + 1);
    } else {
        total_requested_size = this->n;
    }

    total_requested_size -= sr->get_start_position();
    total_requested_size -= offset;
    total_requested_size = std::min(size, total_requested_size);

    written = (uint32_t) this->view_fasta_chunk(
                  cache, // ffs2f_init_seq* cache,
                  buffer, // char *buffer
                  (size_t) total_requested_size, // size_t buffer_size,
                  (off_t) 2 + this->name.size() + sr->get_start_position() + offset, // offset is for chunked reading
                  fh
              );

    return written;
}



/*
CRAM specification:

M5 (sequence MD5 checksum) field of @SQ sequence record in the BAM header is required and UR (URI
for the sequence fasta optionally gzipped file) field is strongly advised. The rule for calculating MD5 is
 - to remove any non-base symbols (like \n, sequence name or length and spaces) and
 - upper case the rest.

meaning: md5s([ACTGN]+)

// ww -l = 149998
*
* need = 74999


chunk size 1:
fastafs check short  1.49s user 2.79s system 99% cpu 4.282 total
fastafs check short  1.53s user 2.73s system 99% cpu 4.269 total

chunk size 1024:
??
*/
std::string fastafs_seq::sha1(ffs2f_init_seq* cache, chunked_reader &fh)
{
#if DEBUG
    if(cache == nullptr) {
        throw std::invalid_argument("Using an empty cache is impossible\n");
    }
#endif

    const size_t header_offset = this->name.size() + 2;

    //const size_t fasta_size = header_offset + this->n; // not size effectively, as terminating newline is skipped..., but length to be read
    const unsigned long chunksize = 1024 * 1024; // seems to not matter that much
    char chunk[chunksize + 2];
    chunk[chunksize] = '\0';



    EVP_MD_CTX *mdctx;
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL);


    //SHA_CTX ctx;
    //SHA1_Init(&ctx);

    //fh->clear();

    // "(a/b)*b + a%b shall equal a"
    // full iterations = this->n / chunk_size; do this number of iterations looped
    unsigned long n_iterations = (unsigned long) this->n / chunksize;
    signed int remaining_bytes = this->n % chunksize;

    // half iteration remainder = this->n % chunk_size; if this number > 0; do it too
    for(uint32_t i = 0; i < n_iterations; i++) {
        this->view_fasta_chunk(cache, chunk,
                               chunksize,
                               header_offset + (i * chunksize),
                               fh);

        //SHA1_Update(&ctx, chunk, chunksize);
        EVP_DigestUpdate(mdctx, chunk, chunksize); // new

    }

    if(remaining_bytes > 0) {
        this->view_fasta_chunk(cache, chunk, remaining_bytes, header_offset + (n_iterations * chunksize), fh);

        //SHA1_Update(&ctx, chunk, remaining_bytes);
        EVP_DigestUpdate(mdctx, chunk, remaining_bytes); // new

        //chunk[remaining_bytes] = '\0';
    }


    unsigned int sha1_digest_len = EVP_MD_size(EVP_md5());
    unsigned char cur_sha1_digest[sha1_digest_len];
    EVP_DigestFinal_ex(mdctx, cur_sha1_digest, &sha1_digest_len);
    EVP_MD_CTX_free(mdctx);

    //SHA1_Final(cur_sha1_digest, &ctx);
    //fh->clear(); // because gseek was done before

    char sha1_hash[41];
    sha1_digest_to_hash(cur_sha1_digest, sha1_hash);

    return std::string(sha1_hash);
}



std::string fastafs_seq::md5(ffs2f_init_seq* cache, chunked_reader &fh)
{
#if DEBUG
    if(cache == nullptr) {
        throw std::invalid_argument("Using an empty cache is impossible\n");
    }
#endif

    const size_t header_offset = this->name.size() + 2;

    //const size_t fasta_size = header_offset + this->n; // not size effectively, as terminating newline is skipped..., but length to be read
    const unsigned long chunksize = 1024 * 1024; // seems to not matter that much
    char chunk[chunksize + 2];
    chunk[chunksize] = '\0';

    // https://stackoverflow.com/questions/69806220/advice-needed-for-migration-of-low-level-openssl-api-to-high-level-openssl-apis
    EVP_MD_CTX *mdctx;
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);


    //fh->clear();

    // "(a/b)*b + a%b shall equal a"
    // full iterations = this->n / chunk_size; do this number of iterations looped
    unsigned long n_iterations = (unsigned long) this->n / chunksize;
    signed int remaining_bytes = this->n % chunksize;

    size_t written;
    // half iteration remainder = this->n % chunk_size; if this number > 0; do it too
    for(uint32_t i = 0; i < n_iterations; i++) {
        written = this->view_fasta_chunk(cache, chunk,
                                         chunksize,
                                         header_offset + (i * chunksize),
                                         fh);

        if(this->flags.is_fourbit()) {
            written = remove_chars(chunk, '-', written);
        }

        EVP_DigestUpdate(mdctx, chunk, written); // new
    }

    if(remaining_bytes > 0) {
        written = this->view_fasta_chunk(cache, chunk, remaining_bytes, header_offset + (n_iterations * chunksize), fh);

        if(this->flags.is_fourbit()) {
            written = remove_chars(chunk, '-', written);
        }

        EVP_DigestUpdate(mdctx, chunk, written); // new

        chunk[remaining_bytes] = '\0';
    }

    //printf(" (%i * %i) + %i =  %i  = %i\n", n_iterations , chunksize, remaining_bytes , (n_iterations * chunksize) + remaining_bytes , this->n);


    //fh->clear(); // because gseek was done before


    unsigned char *md5_digest;
    unsigned int md5_digest_len = EVP_MD_size(EVP_md5());
    md5_digest = (unsigned char *)OPENSSL_malloc(md5_digest_len);
    EVP_DigestFinal_ex(mdctx, md5_digest, &md5_digest_len);
    EVP_MD_CTX_free(mdctx);

    char md5_hash[32 + 1];
    md5_digest_to_hash(md5_digest, md5_hash);
    
    OPENSSL_free(md5_digest);

    return std::string(md5_hash);
}



uint32_t fastafs_seq::n_bits()
{
    uint32_t n = this->n;// number of characters

    // minus number of masked characters
    for(uint32_t i = 0; i < this->n_starts.size(); i++) {
        n -= n_ends[i] - this->n_starts[i] + 1;
    }

    // divided by bits per bytes
    if(this->flags.is_twobit()) {
        // if n actg bits is:
        // 0 -> 0
        // 1,2,3 and 4 -> 1
        return (n + (twobit_byte::nucleotides_per_byte - 1)) / twobit_byte::nucleotides_per_byte;
    } else if(this->flags.is_fourbit()) {
        return (n + (fourbit_byte::nucleotides_per_byte - 1)) / fourbit_byte::nucleotides_per_byte;
    } else {
        return 0; // unclear yet
    }
}





//@brief calculates the number of paddings found in a sequence of length N with
uint32_t fastafs_seq::n_padding(uint32_t offset, uint32_t position_until, uint32_t padding)
{
    // if debug:
    //   if offset > position_until
    //      throw error
    // end if
    uint32_t n = (position_until + 1) / (padding + 1);

    // minus all the n's that occurred before offset
    if(offset > 0) {
        n -= fastafs_seq::n_padding(0, offset - 1, padding);
    }
    return n;
}



//@brief finds the number of N's BEFORE pos, and returns whether POS is N
bool fastafs_seq::get_n_offset(uint32_t pos, uint32_t *num_Ns)
{
    *num_Ns = 0;

    for(uint32_t n_block = 0; n_block < this->n_starts.size(); ++n_block) {
        if(this->n_starts[n_block] > pos) {
            return false;
        } else {

            // als einde kleiner is dan pos, tel verschil op
            if(this->n_ends[n_block] < pos) {
                *num_Ns += (this->n_ends[n_block] - this->n_starts[n_block]) + 1;
            }

            // pos is within N block
            else if(this->n_ends[n_block] >= pos) {
                *num_Ns += (pos - this->n_starts[n_block]);// if pos is N and would be included: + 1

                return true;
            }
        }
    }

    return false;
}



fastafs::fastafs(std::string arg_name) :
    name(arg_name), filetype(compression_type::undefined), crc32f(0)
{
}

fastafs::~fastafs()
{
    for(uint32_t i = 0; i < this->data.size(); i++) {
        delete this->data[i];
    }
}

void fastafs::load(std::string afilename)
{
    std::streampos size;
    unsigned char *memblock;

    chunked_reader fh_in = chunked_reader(afilename.c_str());
    {
        fh_in.fopen(0);
        this->filetype = fh_in.get_filetype();

        memblock = new unsigned char [20 + 1]; //sha1 is 20b
        // if a user can't compile this line, please replace it with C's
        // 'realpath' function and delete/free afterwards and send a PR
        //this->filename = std::filesystem::canonical(afilename);// this path must be absolute because if stuff gets send to FUSE, paths are relative to the FUSE process and probably systemd initialization
        this->filename = realpath_cpp(afilename);

        size = (size_t) fh_in.read(memblock, 16);

        if(size < 16) {
            //file.close();
            throw std::invalid_argument("Corrupt file: " + filename);
        } else {

            fh_in.seek(0);
            uint32_t i;

            // HEADER
            fh_in.read(memblock, 14);
            memblock[16] = '\0';

            // check magic
            for(i = 0 ; i < 4;  i++) {
                if(memblock[i] != FASTAFS_MAGIC[i]) {
                    throw std::invalid_argument("Corrupt file: " + filename);
                }
            }
            for(i = 0 ; i < 4;  i++) {
                if(memblock[i + 4] != FASTAFS_VERSION[i]) {
                    throw std::invalid_argument("Corrupt file: " + filename);
                }
            }

            this->flags.set(&memblock[8]);
            if(this->flags.is_incomplete()) {
                throw std::invalid_argument("Incomplete FASTAFS file (probably terminated during conversion): " + filename);
            }

            std::streampos file_cursor = (std::streampos) fourbytes_to_uint(&memblock[10], 0);

            // INDEX
            fh_in.seek(file_cursor);
            fh_in.read(memblock, 4);
            this->data.resize(fourbytes_to_uint(memblock, 0));//n_seq becomes this->data.size()

            size_t j;
            fastafs_seq *s;
            for(i = 0; i < this->data.size(); i ++) {
                s = new fastafs_seq;

                // flag
                fh_in.read(memblock, 2);
                s->flags.set(memblock);// should be initialized during construction of this class

                // name length
                fh_in.read(memblock, 1);

                // name
                size_t namesize = (unsigned char) memblock[0]; // cast to something that is large enough (> 128)
                //char name[namesize + 1];
                unsigned char *name = new unsigned char[namesize + 1];
                fh_in.read(name, namesize);
                name[(unsigned char) memblock[0]] = '\0';
                s->name = std::string(reinterpret_cast<char*>(name));
                delete[] name;

                // set cursor and save sequence data position
                fh_in.read(memblock, 4);
                file_cursor = fh_in.tell();

                s->data_position = fourbytes_to_uint(memblock, 0);
                fh_in.seek((uint32_t) s->data_position);
                {
                    // sequence stuff
                    // n compressed nucleotides
                    fh_in.read(memblock, 4);
                    s->n = fourbytes_to_uint(memblock, 0);

                    // skip nucleotides
                    if(s->flags.is_twobit()) { // there fit 4 twobits in a byte, thus divide by 4,
                        fh_in.seek((uint32_t) s->data_position + 4 + ((s->n + 3) / 4));
                    } else if(s->flags.is_fourbit()) { // there fit 2 fourbits in a byte, thus divide by 2,
                        fh_in.seek((uint32_t) s->data_position + 4 + ((s->n + 1) / 2));
                    } else {
                        fh_in.seek((uint32_t) s->data_position + 4 + fivebit_fivebytes::nucleotides_to_compressed_offset(s->n));
                    }

                    // N-blocks (and update this->n instantly)
                    fh_in.read(memblock, 4);
                    uint32_t N_blocks = fourbytes_to_uint(memblock, 0);

                    s->n_starts.resize(N_blocks);
                    s->n_ends.resize(N_blocks);
                    for(j = 0; j < s->n_starts.size(); j++) {
                        fh_in.read(memblock, 4);
                        s->n_starts[j] = fourbytes_to_uint(memblock, 0);
                    }
                    for(j = 0; j < s->n_ends.size(); j++) {
                        fh_in.read(memblock, 4);
                        s->n_ends[j] = fourbytes_to_uint(memblock, 0);
                        s->n += s->n_ends[j] - s->n_starts[j] + 1;
                    }

                    // MD5-checksum - only if sequence is complete
                    if(s->flags.is_complete()) {
                        fh_in.read(memblock, 16);
                        for(int j = 0; j < 16 ; j++) {
                            s->md5_digest[j] = memblock[j];
                        }
                    }

                    // M-blocks
                    fh_in.read(memblock, 4);
                    uint32_t M_blocks = fourbytes_to_uint(memblock, 0);
                    s->m_starts.resize(M_blocks);
                    s->m_ends.resize(M_blocks);
                    for(j = 0; j < s->m_starts.size(); j++) {
                        fh_in.read(memblock, 4);
                        s->m_starts[j] = fourbytes_to_uint(memblock, 0);
                    }
                    for(j = 0; j < s->m_ends.size(); j++) {
                        fh_in.read(memblock, 4);
                        s->m_ends[j] = fourbytes_to_uint(memblock, 0);
                    }
                }

                fh_in.seek(file_cursor);
                this->data[i] = s;
            }

            // metadata section - empty for now
            fh_in.read(memblock, 1);

            // crc32 checksum - may be missing because fastafs::load is also used before fastafs::get_crc32 is ran to obtain the checksum
            if(fh_in.read(memblock, 4) == 4) {
                this->crc32f = fourbytes_to_uint(memblock, 0);
            } else {
                //printf("crc32 checksum missing\n");
            }

            delete[] memblock;
        }
    }
}


void fastafs::view_fasta(ffs2f_init* cache)
{
    if(this->filename.size() == 0) {
        throw std::invalid_argument("No filename found");
    }

    //std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    //if(file.is_open()) {
    chunked_reader fh = chunked_reader(this->filename.c_str());
    fh.fopen(0);

    for(uint32_t i = 0; i < this->data.size(); i++) {
        this->data[i]->view_fasta(cache->sequences[i], fh);
    }

    //   file.close();
    //}
}




ffs2f_init* fastafs::init_ffs2f(uint32_t padding, bool allow_masking)
{
    ffs2f_init *ddata = new ffs2f_init(this->data.size(), padding);

    for(size_t i = 0; i < this->data.size(); i++) {
        ddata->sequences[i] = this->data[i]->init_ffs2f_seq(padding, allow_masking);
    }

    return ddata;
}



// estimates the whole file size of a file such as "/seq/chr1:56-"
size_t fastafs::view_sequence_region_size(const char *seq_region_arg)
{
    std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        // parse "chr..:..-.." string
        sequence_region sr = sequence_region(seq_region_arg);

        // 02 : check if 'chr' is equals this->data[i].name
        for(size_t i = 0; i < this->data.size(); i++) {
            if(sr.get_seq_name().compare(this->data[i]->name) == 0) {
                return this->data[i]->view_sequence_region_size(&sr);
            }
        }
    }

    return 0;
}



uint32_t fastafs::view_sequence_region(ffs2f_init* cache, const char *seq_region_arg, char *buffer, size_t buffer_size, off_t file_offset)
{
#if DEBUG
    if(cache == nullptr) {
        throw std::invalid_argument("fastafs::view_sequence_region - error 01\n");
    }

    if(cache->padding_arg != 0) {
        throw std::invalid_argument("fastafs::view_sequence_region - error 02\n");
    }

    if(cache->sequences.size() == 0) {
        throw std::invalid_argument("fastafs::view_sequence_region - error 03\n");
    }
#endif

    chunked_reader fh = chunked_reader(this->filename.c_str());
    fh.fopen(0);
    //std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    //if(file.is_open()) {
    // parse "chr..:..-.." string
    sequence_region sr = sequence_region(seq_region_arg);

    // 02 : check if 'chr' is equals this->data[i].name
    for(size_t i = 0; i < this->data.size(); i++) {
        if(sr.get_seq_name().compare(this->data[i]->name) == 0) {
            return this->data[i]->view_sequence_region(cache->sequences[i], &sr, buffer,  buffer_size, file_offset, fh);
        }
    }
    //}

    return 0;
}




/*
 * fastafs::view_fasta_chunk -
 *
 * @cache:
 * @buffer:
 * @buffer_size:
 * @file_offset:
 *
 * returns
 */
uint32_t fastafs::view_fasta_chunk(ffs2f_init* cache, char *buffer, size_t buffer_size, off_t file_offset)
{
    chunked_reader fh = chunked_reader(this->filename.c_str());
    fh.fopen(0);

    uint32_t s = this->view_fasta_chunk(cache, buffer, buffer_size, file_offset, fh);
    //#printf("%02hhX %02hhX %02hhX %02hhX\n", buffer[0], buffer[1], buffer[2], buffer[3]);

    return s;
}



uint32_t fastafs::view_fasta_chunk(ffs2f_init* cache, char *buffer, size_t buffer_size, off_t file_offset, chunked_reader &fh)
{
    uint32_t written = 0;

    size_t i = 0;// sequence iterator
    uint32_t pos = (uint32_t) file_offset;
    fastafs_seq *seq;

    while(i < data.size()) {
        seq = this->data[i];
        const uint32_t filesize = cache->sequences[i]->filesize;

        if(pos < filesize) {
            const uint32_t written_seq = seq->view_fasta_chunk(
                                             cache->sequences[i],
                                             &buffer[written],
                                             std::min((uint32_t) buffer_size - written, filesize),
                                             pos,
                                             fh);

            written += written_seq;
            pos -= (filesize - written_seq);

            if(written == buffer_size) {
                return written;
            }
        } else {
            pos -= filesize;
        }

        i++;
    }

    return written;
}








//http://genome.ucsc.edu/FAQ/FAQformat.html#format7
//https://www.mathsisfun.com/binary-decimal-hexadecimal-converter.html
uint32_t fastafs::view_ucsc2bit_chunk(char *buffer, size_t buffer_size, off_t file_offset)
{
    uint32_t written = 0;
    uint32_t pos = (uint32_t) file_offset; // iterator (position, in bytes) in file
    uint32_t pos_limit = 0; // counter to keep track of when writing needs to stop for given loop

    //std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    //if(file.is_open()) {
    chunked_reader file = chunked_reader(this->filename.c_str());
    file.fopen(0);
    char n_seq[4];
    pos_limit += 4;// skip this loop after writing first four bytes
    while(pos < pos_limit) {
        buffer[written++] = UCSC2BIT_MAGIC[pos];
        pos++;
        if(written >= buffer_size) {
            return written;
        }
    }
    pos_limit += 4;
    while(pos < pos_limit) {
        buffer[written++] = UCSC2BIT_VERSION[pos - 4];
        pos++;
        if(written >= buffer_size) {
            return written;
        }
    }
    // number sequences
    uint_to_fourbytes_ucsc2bit(n_seq, (uint32_t) this->data.size());
    pos_limit += 4;
    while(pos < pos_limit) {
        buffer[written++] = n_seq[pos - 8];
        pos++;
        if(written >= buffer_size) {
            return written;
        }
    }
    // 4 x nullbyte
    pos_limit += 4;
    while(pos < pos_limit) {
        buffer[written++] = '\0';
        pos++;
        if(written >= buffer_size) {
            return written;
        }
    }
    uint32_t header_block_len = 4 + 4 + 4 + 4 + ((uint32_t) this->data.size() * (1 + 4));
    uint32_t header_offset_previous = 0;
    for(uint32_t i = 0; i < this->data.size(); i++) {
        header_block_len += (uint32_t) this->data[i]->name.size();
    }

    fastafs_seq *sequence;
    size_t i;
    for(i = 0; i < this->data.size(); i++) {
        sequence = this->data[i];

        // single byte can be written, as the while loop has returned true
        pos_limit += 1;
        if(pos < pos_limit) {
            buffer[written++] = (unsigned char) sequence->name.size();
            pos++;
            if(written >= buffer_size) {
                return written;
            }
        }

        // sequence name
        pos_limit += (uint32_t) sequence->name.size();
        while(pos < pos_limit) {
            buffer[written++] = sequence->name[sequence->name.size() - (pos_limit - pos)];
            pos++;
            if(written >= buffer_size) {
                return written;
            }
        }

        // file offset
        uint32_t offset = header_block_len + header_offset_previous;
        uint_to_fourbytes_ucsc2bit(n_seq, offset);
        pos_limit += 4;
        while(pos < pos_limit) {
            buffer[written++] = n_seq[4 - (pos_limit - pos)];
            pos++;
            if(written >= buffer_size) {
                return written;
            }
        }

        header_offset_previous += 4 + 4 + 4 + 4;
        header_offset_previous += 8 * (uint32_t) sequence->n_starts.size();
        header_offset_previous += 8 * (uint32_t) sequence->m_starts.size();
        header_offset_previous += sequence->n / 4;

        if(sequence->n % 4 != 0) {
            header_offset_previous++;
        }
    }

    ffs2f_init* cache = this->init_ffs2f(0, false); // false, no masking needed, always upper-case is fine in this case
    for(i = 0; i < this->data.size(); i++) {
        sequence = this->data[i];

        // number nucleotides
        uint_to_fourbytes_ucsc2bit(n_seq, sequence->n);
        pos_limit += 4;
        while(pos < pos_limit) {
            buffer[written++] = n_seq[4 - (pos_limit - pos)];
            pos++;
            if(written >= buffer_size) {
                delete cache;
                return written;
            }
        }

        // number N blocks
        uint_to_fourbytes_ucsc2bit(n_seq, (uint32_t) sequence->n_starts.size());
        pos_limit += 4;
        while(pos < pos_limit) {
            buffer[written++] = n_seq[4 - (pos_limit - pos)];
            pos++;
            if(written >= buffer_size) {
                delete cache;
                return written;
            }
        }

        // write n-blocks effectively down!
        for(uint32_t k = 0; k < sequence->n_starts.size(); k++) {
            uint_to_fourbytes_ucsc2bit(n_seq, sequence->n_starts[k]);
            pos_limit += 4;
            while(pos < pos_limit) {
                buffer[written++] = n_seq[4 - (pos_limit - pos)];
                pos++;
                if(written >= buffer_size) {
                    delete cache;
                    return written;
                }
            }

            uint_to_fourbytes_ucsc2bit(n_seq, sequence->n_ends[k] - sequence->n_starts[k] + 1);
            pos_limit += 4;
            while(pos < pos_limit) {
                buffer[written++] = n_seq[4 - (pos_limit - pos)];
                pos++;
                if(written >= buffer_size) {
                    delete cache;
                    return written;
                }
            }
        }

        // number M blocks (masked regions; lower case regions)
        uint_to_fourbytes_ucsc2bit(n_seq, (uint32_t) sequence->m_starts.size());
        pos_limit += 4;
        while(pos < pos_limit) {
            buffer[written++] = n_seq[4 - (pos_limit - pos)];
            pos++;

            if(written >= buffer_size) {
                delete cache;
                return written;
            }
        }

        // write m-blocks effectively down!
        for(uint32_t k = 0; k < sequence->m_starts.size(); k++) {
            uint_to_fourbytes_ucsc2bit(n_seq, sequence->m_starts[k]);
            pos_limit += 4;
            while(pos < pos_limit) {
                buffer[written++] = n_seq[4 - (pos_limit - pos)];
                pos++;

                if(written >= buffer_size) {
                    delete cache;
                    return written;
                }
            }

            uint_to_fourbytes_ucsc2bit(n_seq, sequence->m_ends[k] - sequence->m_starts[k] + 1);
            pos_limit += 4;
            while(pos < pos_limit) {
                buffer[written++] = n_seq[4 - (pos_limit - pos)];
                pos++;

                if(written >= buffer_size) {
                    delete cache;
                    return written;
                }
            }
        }

        // reserved block
        pos_limit += 4;
        while(pos < pos_limit) {
            buffer[written++] = '\0';
            pos++;

            if(written >= buffer_size) {
                delete cache;
                return written;
            }
        }

        // twobit coded nucleotides (only containing 4 nucleotides each)
        uint32_t full_twobits = sequence->n / 4;
        twobit_byte_dna t;
        pos_limit += full_twobits;

        while(pos < pos_limit) {
            //printf("%i - %i  = %i  ||  %i\n",pos_limit,pos, (full_twobits - (pos_limit - pos)) * 4, j);
            //sequence->view_fasta_chunk(0, n_seq, sequence->name.size() + 2 + ((full_twobits - (pos_limit - pos)) * 4), 4, &file);
            sequence->view_fasta_chunk(cache->sequences[i], n_seq, 4, sequence->name.size() + 2 + ((full_twobits - (pos_limit - pos)) * 4), file);
            t.set(n_seq);
            buffer[written++] = t.data;
            pos++;
            if(written >= buffer_size) {
                delete cache;
                return written;
            }
        }

        // last byte, may also rely on 1,2 or 3 nucleotides and reqiures setting 0's
        if(full_twobits * 4 < sequence->n) {
            n_seq[0] = 'N';
            n_seq[1] = 'N';
            n_seq[2] = 'N';
            n_seq[3] = 'N';
            pos_limit += 1;
            if(pos < pos_limit) {
                //printf("%i - %i  = %i  ||  %i      ::    %i  == %i \n",pos_limit,pos, full_twobits * 4, j, sequence->n - (full_twobits * 4),  sequence->n - j);
                //sequence->view_fasta_chunk(0, n_seq, sequence->name.size() + 2 + full_twobits * 4, sequence->n - (full_twobits * 4), &file);
                sequence->view_fasta_chunk(cache->sequences[i], n_seq, sequence->n - (full_twobits * 4), sequence->name.size() + 2 + full_twobits * 4, file);
                t.set(n_seq);
                buffer[written++] = t.data;
                pos++;
                if(written >= buffer_size) {
                    delete cache;
                    return written;
                }
            }
        }
    }
    delete cache;
    //file.close();
    //} else {
    //   throw std::runtime_error("[fastafs::view_fasta_chunk] could not load fastafs: " + this->filename);
    // }
    return written;
}



/*
@HD	VN:1.0	SO:unsorted
@SQ	SN:chr1	LN:16	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
@SQ	SN:chr2	LN:16	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
@SQ	SN:chr3.1	LN:13	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
@SQ	SN:chr3.2	LN:14	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
@SQ	SN:chr3.3	LN:15	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
@SQ	SN:chr4	LN:8	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
@SQ	SN:chr5	LN:6	M5:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx	UR:fastafs:///test
 */
size_t fastafs::view_dict_chunk(char *buffer, size_t buffer_size, off_t file_offset)
{
    uint32_t written = 0;
    uint32_t pos = (uint32_t) file_offset; // iterator (position, in bytes) in file
    uint32_t pos_limit = 0; // counter to keep track of when writing needs to stop for given loop

    std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        pos_limit += (uint32_t) DICT_HEADER.size();// skip this loop after writing first four bytes

        while(pos < pos_limit) {
            buffer[written++] = DICT_HEADER[pos];
            pos++;
            if(written >= buffer_size) {
                return written;
            }
        }

        fastafs_seq *sequence;
        for(size_t i = 0; i < this->data.size(); i++) {
            // @SQ \t SN:
            pos_limit += (uint32_t) dict_sq.size();// skip this loop after writing first four bytes
            while(pos < pos_limit) {
                buffer[written++] = dict_sq[dict_sq.size() - (pos_limit - pos)];
                pos++;
                if(written >= buffer_size) {
                    return written;
                }
            }

            // sequence name
            sequence = this->data[i];
            pos_limit += (uint32_t) sequence->name.size();
            while(pos < pos_limit) {
                buffer[written++] = sequence->name[sequence->name.size() - (pos_limit - pos)];
                pos++;
                if(written >= buffer_size) {
                    return written;
                }
            }

            // \t LN:
            pos_limit += (uint32_t) dict_ln.size();// skip this loop after writing first four bytes
            while(pos < pos_limit) {
                buffer[written++] = dict_ln[dict_ln.size() - (pos_limit - pos)];
                pos++;
                if(written >= buffer_size) {
                    return written;
                }
            }

            // 12345
            std::string len = std::to_string(sequence->n);
            pos_limit += (uint32_t) len.size();// skip this loop after writing first four bytes
            while(pos < pos_limit) {
                buffer[written++] = len[len.size() - (pos_limit - pos)];
                pos++;
                if(written >= buffer_size) {
                    return written;
                }
            }

            // \t M5:
            pos_limit += (uint32_t) dict_m5.size();// skip this loop after writing first four bytes
            while(pos < pos_limit) {
                buffer[written++] = dict_m5[dict_m5.size() - (pos_limit - pos)];
                pos++;
                if(written >= buffer_size) {
                    return written;
                }
            }

            // md5 checksum
            char md5_hash[32 + 1] = "";
            md5_hash[32] = '\0';
            md5_digest_to_hash(sequence->md5_digest, md5_hash);

            pos_limit += (uint32_t) 32;// skip this loop after writing first four bytes
            while(pos < pos_limit) {
                buffer[written++] = md5_hash[32 - (pos_limit - pos)];
                pos++;
                if(written >= buffer_size) {
                    return written;
                }
            }

            // \t UR:
            pos_limit += (uint32_t) dict_ur.size();// skip this loop after writing first four bytes
            while(pos < pos_limit) {
                buffer[written++] = dict_ur[dict_ur.size() - (pos_limit - pos)];
                pos++;
                if(written >= buffer_size) {
                    return written;
                }
            }

            // file name
            pos_limit += (uint32_t) this->name.size();
            while(pos < pos_limit) {
                buffer[written++] = this->name[this->name.size() - (pos_limit - pos)];
                pos++;
                if(written >= buffer_size) {
                    return written;
                }
            }

            // '\n'
            pos_limit += 1;
            if(pos < pos_limit) {
                buffer[written++] = '\n';
                pos++;
                if(written >= buffer_size) {
                    return written;
                }
            }

        }

        file.close();
    } else {
        throw std::runtime_error("[fastafs::view_dict_chunk] could not load fastafs: " + this->filename);
    }

    return written;
}



//@todo add unit tests
size_t fastafs::fastafs_filesize(void)
{
    // header + n-sequences
    size_t n = 4 + 4 + 2 + 4;

    // number sequences
    n += 4;

    // per sequence
    for(uint32_t i = 0; i < this->data.size(); i++) {
        n += 2;// flags
        n += 1; // name length
        n += this->data[i]->name.size();// name
        n += 4; // reference to compr. data

        // compr dataa
        n += 4 + 4 + 4;// compressed nuc. + n blocks + m blocks
        n += this->data[i]->n_bits();
        n += this->data[i]->n_starts.size() * 8;
        n += 16;//md5 sum, always present?
        n += this->data[i]->m_starts.size() * 8;
    }

    // metadata
    n += 1; // @ todo more sophi.

    // crc32
    n += 4;

    return n;
}


size_t fastafs::fasta_filesize(uint32_t padding)
{
    size_t n = 0;

    //std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    //if(file.is_open()) {
    //    file.close();


    for(size_t i = 0; i < this->data.size(); i++) {
        n += this->data[i]->fasta_filesize(padding);
    }

    //} else {
    //    throw std::runtime_error("[fastafs::fasta_filesize] could not load fastafs: " + this->filename);
    //}
    return n;
}



size_t fastafs::ucsc2bit_filesize(void)
{
    size_t nn = 4 + 4 + 4 + 4;// header, version, n-seq, rsrvd
    fastafs_seq *sequence;

    for(size_t i = 0; i < this->data.size(); i++) {
        sequence = this->data[i];
        nn += 1; // namesize
        nn += 4; // offset in file
        nn += 4;// dna size
        nn += 4;// n blocks
        nn += sequence->n_starts.size() * (4 * 2); // both start and end
        nn += 4;// masked regions - so far always 0
        nn += sequence->m_starts.size() * (4 * 2); // both start and end
        nn += 4;// reserved
        nn += sequence->name.size();
        nn += (sequence->n + 3) / 4; // math.ceil hack
    }

    return nn;
}




size_t fastafs::dict_filesize(void)
{
    size_t size = DICT_HEADER.size();

    // every sequence has a:
    // '@SQ \t SN:' + '\t LN: ??len??? \t' + 'S1:' + 32 + '\t UR:fastafs:///' + this->name.size()  + '\n'
    //  ||| |  |||     |  |||           |     |||           | ||||||||||||||                           |
    size += (31 + 32 + this->name.size()) * this->data.size();

    for(size_t i = 0; i < this->data.size(); i++) {
        size += this->data[i]->name.size();

        std::string seq_size = std::to_string(this->data[i]->n);
        size += seq_size.size();
    }

    return size;
}



std::string fastafs::get_faidx(uint32_t padding)
{
    std::string contents = "";
    std::string padding_s = std::to_string(padding);
    std::string padding_s2 = std::to_string(padding + 1);// padding + newline

    std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

    if(file.is_open()) {
        file.close();
        uint32_t offset = 0;

        for(size_t i = 0; i < this->data.size(); i++) {
            offset += 1;// '>'
            offset += (uint32_t) this->data[i]->name.size() + 1; // "chr1\n"

            // fasta headers may contanis spaces etc. which need to be excluded from fai files

            std::string name_t;
            std::string::size_type truncate = data[i]->name.find(" ");
            if(truncate > 1) {
                name_t = data[i]->name.substr(0, truncate);
            } else {
                name_t = data[i]->name;
            }

            contents += name_t + "\t" + std::to_string(this->data[i]->n) + "\t" + std::to_string(offset) + "\t" + padding_s + "\t" + padding_s2 + "\n";

            offset += this->data[i]->n; // ACTG NNN
            offset += (this->data[i]->n + (padding - 1)) / padding;// number of newlines corresponding to ACTG NNN lines
        }
        //while(written < buffer_size and written + file_offset < contents.size()) {
        //	buffer[written] = contents[written];
        //	written++;
        //	}
    } else {
        throw std::runtime_error("[fastafs::get_faidx] could not load fastafs: " + this->filename);
    }

    return contents;
}



uint32_t fastafs::view_faidx_chunk(uint32_t padding, char *buffer, size_t buffer_size, off_t file_offset)
{
    std::string contents = this->get_faidx(padding);

    size_t to_copy = std::min(buffer_size, contents.size() - file_offset);

    return (uint32_t) contents.copy(buffer, to_copy, file_offset);
}




/*
https://www.ebi.ac.uk/ena/cram/sha1/7716832754e642d068e6fbd8f792821ca5544309
Hello message sent

HTTP/1.1 301 Moved Permanently
Content-Type: text/html
Date: Fri, 15 Feb 2019 11:20:13 GMT
Location: https://www.ebi.ac.uk/ena/cram/sha1/7716832754e642d068e6fbd8f792821ca5544309
Connection: Keep-Alive
Content-Length: 0
*
*
good response:*
* HTTP/1.0 200 OK
Cache-Control: max-age=31536000
X-Cache: MISS from pg-ena-cram-1.ebi.ac.uk
Content-Type: text/plain
Strict-Transport-Security: max-age=0
Date: Fri, 15 Feb 2019 11:32:17 GMT
X-Application-Context: application:8080
X-Cache-Lookup: MISS from pg-ena-cram-1.ebi.ac.uk:8080
Via: 1.0 pg-ena-cram-1.ebi.ac.uk (squid/3.1.23)
Connection: keep-alive
Content-Length: 249250621


bad response:
HTTP/1.0 404 Not Found
X-Cache: MISS from pg-ena-cram-1.ebi.ac.uk
Content-Type: text/html;charset=utf-8
Strict-Transport-Security: max-age=0
Date: Fri, 15 Feb 2019 11:32:58 GMT
X-Application-Context: application:8080
Content-Language: en
X-Cache-Lookup: MISS from pg-ena-cram-1.ebi.ac.uk:8080
Via: 1.0 pg-ena-cram-1.ebi.ac.uk (squid/3.1.23)
Connection: keep-alive
Content-Length: 1047

 */
int fastafs::info(bool ena_verify_checksum)
{
    if(this->filename.size() == 0) {
        throw std::invalid_argument("No filename found");
    }

    char md5_hash[32 + 1] = "";
    md5_hash[32] = '\0';

    std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        std::cout << "# FASTAFS NAME: " << this->filename << "\n";
        std::cout << "# FORMAT: v0-x32";
        switch(this->filetype) {
        case compression_type::undefined:
            printf("?\n");
            break;
        case compression_type::uncompressed :
            printf("\n");
            break;
        case compression_type::zstd:
            printf("+Z\n");
            break;
        }
        printf("# SEQUENCES:    %u\n", (uint32_t) this->data.size());

        for(uint32_t i = 0; i < this->data.size(); i++) {
            md5_digest_to_hash(this->data[i]->md5_digest, md5_hash);

            std::string compression_type;
            if(this->data[i]->flags.is_twobit()) {
                compression_type = "2bit  ";
            } else if(this->data[i]->flags.is_fourbit()) {
                compression_type = "4bit  ";
            } else if(this->data[i]->flags.is_protein()) {
                compression_type = "5/8bit";
            } else {
                compression_type = "????  ";
            }


            // print sequence name & size & checksum
            printf("%s\t%i\t%s\t%s", this->data[i]->name.c_str(), this->data[i]->n, compression_type.c_str(), md5_hash);


            if(ena_verify_checksum) {
                //wget header of:
                //https://www.ebi.ac.uk/ena/cram/md5/<md5>
                //std::cout << "https://www.ebi.ac.uk/ena/cram/md5/" << md5_hash << "\n";
                SSL *ssl;

                //int sock_ssl = 0;
                //struct sockadfiledr_in address;
                int sock = 0;
                struct sockaddr_in serv_addr;
                std::string hello2 = "GET /ena/cram/md5/" + std::string(md5_hash) + " HTTP/1.1\r\nHost: www.ebi.ac.uk\r\nConnection: Keep-Alive\r\n\r\n";

                //char *hello = &hello2.c_str();
                char buffer[1024] = {0};
                if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return -1;
                }

                memset(&serv_addr, '0', sizeof(serv_addr));
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(443);

                // Convert IPv4 and IPv6 addresses from text to binary form
                if(inet_pton(AF_INET, "193.62.193.80", &serv_addr.sin_addr) <= 0) {
                    printf("\nInvalid address/ Address not supported \n");
                    return -1;
                }

                if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                    printf("\nConnection Failed \n");
                    return -1;
                }

                SSL_library_init();
                SSLeay_add_ssl_algorithms();
                SSL_load_error_strings();
                const SSL_METHOD *meth = TLS_client_method();// defining version specificity in here often results in deprecation warnings over time
                SSL_CTX *ctx = SSL_CTX_new(meth);

                ssl = SSL_new(ctx);
                if(!ssl) {
                    printf("Error creating SSL.\n");
                    //log_ssl();
                    return -1;
                }

                //int sock_ssl = SSL_get_fd(ssl);
                SSL_set_fd(ssl, sock);
                int err = SSL_connect(ssl);
                if(err <= 0) {
                    printf("Error creating SSL connection.  err=%x\n", err);

                    //log_ssl();
                    fflush(stdout);
                    return -1;
                }
                SSL_write(ssl, hello2.c_str(), (signed int) hello2.length());

                int NNvalread = SSL_read(ssl, buffer, 32);
                if(NNvalread < 0) {
                    printf("   <connection error>");
                } else if(std::string(buffer).find(" 200 ") != (size_t) -1) { // sequence is in ENA
                    printf("   https://www.ebi.ac.uk/ena/cram/md5/%s", md5_hash);
                } else {
                    printf("   ---");
                }
            }

            printf("\n");
        }

        file.close();
    }

    return 0;
}


// skips first four bytes and do not include crc32 at the end either
uint32_t fastafs::get_crc32(void)
{
    if(is_zstd_file((const char*) this->filename.c_str())) {
        printf("crc32 verification does not work for zstd compressed archives yet\n");
        exit(1);
        return false;
    }

    // @ todo rewrite function only providing the chunked_reader obj
    return  file_crc32(this->filename, 4, this->fastafs_filesize() - 4);  // not sure why -4 rather than -4-4, but seems to work?
}


//true = integer
//false = corrupt
bool fastafs::check_file_integrity(bool verbose)
{
    uint32_t crc32_current = this->get_crc32();

    unsigned char buf_old[5] = "\x00\x00\x00\x00";
    uint_to_fourbytes(buf_old, (uint32_t) this->crc32f);

    if(crc32_current != this->crc32f) {

        unsigned char buf_new[5] = "\x00\x00\x00\x00";
        uint_to_fourbytes(buf_new, (uint32_t) crc32_current);

        if(verbose) {
            printf("ERROR\t%02hhx%02hhx%02hhx%02hhx (encoded in fastafs)  !=  %02hhx%02hhx%02hhx%02hhx (on disk)\n--\n",
                   (unsigned char) buf_old[0],
                   (unsigned char) buf_old[1],
                   (unsigned char) buf_old[2],
                   (unsigned char) buf_old[3],

                   (unsigned char) buf_new[0],
                   (unsigned char) buf_new[1],
                   (unsigned char) buf_new[2],
                   (unsigned char) buf_new[3]);
        }
    } else {
        if(verbose) {
            printf("OK\t%02hhx%02hhx%02hhx%02hhx\n--\n",
                   (unsigned char) buf_old[0],
                   (unsigned char) buf_old[1],
                   (unsigned char) buf_old[2],
                   (unsigned char) buf_old[3]);
        }
    }

    return (crc32_current == this->crc32f);
}


//true = integer
//false = corrupt
bool fastafs::check_sequence_integrity(bool verbose)
{
    if(this->filename.size() == 0) {
        throw std::invalid_argument("No filename found");
    }

    bool retcode = true;

    char md5_hash[32 + 1] = "";
    md5_hash[32] = '\0';
    std::string old_hash;

    ffs2f_init* cache = this->init_ffs2f(0, false);// do not use masking, this checksum requires capital / upper case nucleotides

    chunked_reader file = chunked_reader(this->filename.c_str());
    file.fopen(0);
    //std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    //if(file.is_open()) {
    for(uint32_t i = 0; i < this->data.size(); i++) {
        md5_digest_to_hash(this->data[i]->md5_digest, md5_hash);
        old_hash = std::string(md5_hash);

        std::string new_hash = this->data[i]->md5(cache->sequences[i], file);
        if(old_hash.compare(new_hash) == 0) {
            if(verbose) {
                printf("OK\t%s\n", this->data[i]->name.c_str());
            }
        } else {
            if(verbose) {
                printf("ERROR\t%s\t%s (encoded in fastafs)  !=  %s (on disk)\n", this->data[i]->name.c_str(), md5_hash, new_hash.c_str());
            }

            retcode = false;
        }
    }
    //file.close();
    //} else {
    //    throw std::runtime_error("[fastafs::check_sequence_integrity] could not load fastafs: " + this->filename);
    //}

    delete cache;

    return retcode;
}


uint32_t fastafs::n()
{
    uint32_t n = 0;

    for(unsigned i = 0; i < this->data.size(); i++) {
        n += this->data[i]->n;
    }

    return n;
}


/*
std::string fastafs::basename()
{
    return "";
}
*/
