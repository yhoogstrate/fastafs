#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>

#include <openssl/sha.h>

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


#include "config.hpp"

#include "twobit_byte.hpp"
#include "fastafs.hpp"
#include "utils.hpp"



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
        throw std::invalid_argument("Padding is set to 0, should have been set to this->n elsewhere.\n");
    }
#endif
    //               >                   chr                 \n  ACTG NNN  /number of newlines corresponding to ACTG NNN lines
    return 1 + (uint32_t) this->name.size() + 1 + this->n + (this->n + (padding - 1)) / padding;
}

void fastafs_seq::view_fasta(ffs2f_init_seq* cache, std::ifstream *fh)
{
    char buffer[READ_BUFFER_SIZE];// = new char [READ_BUFFER_SIZE];
    uint32_t offset = 0;
    //@todo figure out if a do {} while() loop isn't more in place here?
    uint32_t written = this->view_fasta_chunk_cached(cache, buffer, READ_BUFFER_SIZE, offset, fh);
    while(written > 0) {
        std::cout << std::string(buffer, written);
        offset += written;
        written = this->view_fasta_chunk_cached(cache, buffer, READ_BUFFER_SIZE, offset, fh);
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
        data = new ffs2f_init_seq(padding, this->n_starts.size() + 1,  this->m_starts.size() + 1, total_sequence_containing_lines);
    } else {
        data = new ffs2f_init_seq(padding, this->n_starts.size() + 1,  1, total_sequence_containing_lines);
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
    data->m_starts[block_size - 1]  = max_val;
    data->m_ends[block_size - 1] = max_val;

    return data;
}

/*
 * fastafs_seq::view_fasta_chunk_cached -
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
uint32_t fastafs_seq::view_fasta_chunk_cached(
    ffs2f_init_seq* cache,
    char *buffer,

    size_t buffer_size,
    off_t start_pos_in_fasta,

    std::ifstream *fh)
{
#if DEBUG
    if(cache == nullptr) {
        throw std::runtime_error("Empty cache was provided\n");
    }
#endif //DEBUG
    uint32_t written = 0;
    if(written >= buffer_size) { // requesting a buffer of size=0, should throw an exception?
        return written;
    }
    uint32_t pos = (uint32_t) start_pos_in_fasta;
    uint32_t pos_limit = 0;
    // >
    pos_limit += 1;
    if(pos < pos_limit) {
        buffer[written++] = '>';
        pos++;
        if(written >= buffer_size) {
            return written;
        }
    }
    // sequence name
    pos_limit += (uint32_t) this->name.size();
    while(pos < pos_limit) {
        buffer[written++] = this->name[this->name.size() - (pos_limit - pos)];
        pos++;
        if(written >= buffer_size) {
            return written;
        }
    }
    // \n
    pos_limit += 1;
    if(pos < pos_limit) {
        buffer[written++] = '\n';
        pos++;
        if(written >= buffer_size) {
            return written;
        }
    }
    const uint32_t offset_from_sequence_line = pos - pos_limit;
    size_t n_block = cache->n_starts.size();
    size_t m_block = cache->m_starts.size();
    uint32_t newlines_passed = offset_from_sequence_line / (cache->padding + 1);// number of newlines passed (within the sequence part)
    uint32_t nucleotide_pos = offset_from_sequence_line - newlines_passed;// requested nucleotide in file
    // calculate file position for next twobit
    // when we are in an OPEN n block, we need to go to the first non-N base after, and place the file pointer there
    uint32_t n_passed = 0;
    this->get_n_offset(nucleotide_pos, &n_passed);
    fh->seekg((uint32_t) this->data_position + 4 + ((nucleotide_pos - n_passed) / 4), fh->beg);
    /*
     0  0  0  0  1  1  1  1 << desired offset from starting point
     A  C  T  G  A  C  T  G
    *

    handigste is om file pointer naar de byte ervoor te zetten
    vervolgens wanneer twobit_offset gelijk is aan nul, lees je de volgende byte
    * nooit out of bound

    */
    twobit_byte t = twobit_byte();
    const char *chunk = twobit_byte::twobit_hash[0];
    unsigned char twobit_offset = (nucleotide_pos - n_passed) % 4;
    if(twobit_offset != 0) {
        fh->read((char*)(&t.data), 1);
        chunk = t.get();
    }
    while(n_block > 0 and pos <= cache->n_ends[n_block - 1]) { // iterate back
        n_block--;
    }
    while(m_block > 0 and pos <= cache->m_ends[m_block - 1]) { // iterate back
        m_block--;
    }
    // write sequence
    pos_limit += newlines_passed * (cache->padding + 1);// passed sequence-containg lines
    while(newlines_passed < cache->total_sequence_containing_lines) { // only 'complete' lines that are guarenteed 'padding' number of nucleotides long [ this loop starts at one to be unsigned-safe ]
        pos_limit += std::min(cache->padding, this->n - (newlines_passed * cache->padding));// only last line needs to be smaller ~ calculate from the beginning of newlines_passed
        // write nucleotides
        while(pos < pos_limit) {// while next sequence-containing-line is open
            if(pos >= cache->n_starts[n_block]) {
                if(pos >= cache->m_starts[m_block]) { // IN an m block; lower-case
                    buffer[written++] = 'n';
                } else {
                    buffer[written++] = 'N';
                }
            } else {
                if(twobit_offset % 4 == 0) {
                    fh->read((char*)(&t.data), 1);
                    chunk = t.get();
                }
                if(pos >= cache->m_starts[m_block]) { // IN an m block; lower-case
                    buffer[written++] = chunk[twobit_offset] + 32;
                } else {
                    buffer[written++] = chunk[twobit_offset];
                }
                twobit_offset = (unsigned char)(twobit_offset + 1) % 4;
            }
            if(pos == cache->n_ends[n_block]) {
                n_block++;
            }
            if(pos == cache->m_ends[m_block]) {
                m_block++;
            }
            pos++;
            if(written >= buffer_size) {
                //fh->clear();
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
                return written;
            }
        }
        newlines_passed++;
    }
    //fh->clear();
    return written;
}



/*
@todo see if this can be a std::ifstream or some kind of stream type of object?
* padding = number of spaces?
* char buffer =
* start_pos_in_fasta =
* len_to_copy =
* fh = filestream to fastafs file
*/
uint32_t fastafs_seq::view_fasta_chunk(uint32_t padding, char *buffer, off_t start_pos_in_fasta, size_t buffer_size, std::ifstream *fh)
{
    uint32_t written = 0;
    if(written >= buffer_size) { // requesting a buffer of size=0
        fh->clear();
        return written;
    }
    uint32_t pos = (uint32_t) start_pos_in_fasta;
    uint32_t pos_limit = 0;
    if(padding == 0) {
        padding = this->n;
    }
    // >
    pos_limit += 1;
    if(pos < pos_limit) {
        buffer[written++] = '>';
        pos++;
        if(written >= buffer_size) {
            fh->clear();
            return written;
        }
    }
    // sequence name
    pos_limit += (uint32_t) this->name.size();
    while(pos < pos_limit) {
        buffer[written++] = this->name[this->name.size() - (pos_limit - pos)];
        pos++;
        if(written >= buffer_size) {
            fh->clear();
            return written;
        }
    }
    // \n
    pos_limit += 1;
    if(pos < pos_limit) {
        buffer[written++] = '\n';
        pos++;
        if(written >= buffer_size) {
            fh->clear();
            return written;
        }
    }
    // convert from nucleotide positions to file positions (by taking newlines/padding into account)
    std::vector<uint32_t> n_starts_w(this->n_starts.size() + 1);
    std::vector<uint32_t> n_ends_w(this->n_starts.size() + 1);
    for(size_t i = 0; i < this->n_starts.size(); i++) {
        n_starts_w[i] = pos_limit + this->n_starts[i] + (this->n_starts[i] / padding);
        n_ends_w[i] = pos_limit + this->n_ends[i] + (this->n_ends[i] / padding);
    }
    size_t n_block = n_starts_w.size();
    const uint32_t total_sequence_containing_lines = (this->n + padding - 1) / padding;// calculate total number of full nucleotide lines
    n_starts_w[n_block - 1]  = pos_limit + this->n + total_sequence_containing_lines + 1;
    n_ends_w[n_block - 1] = pos_limit + this->n + total_sequence_containing_lines + 1;
    /*
    calc newlines passed:
    >chr1 \n
        ACTG\n      0 1 2 3 4
        ACTG\n      5 6 7 8 9

        0   0
        1   0
        2   0
        3   0
        4   0
        5   1
        6   1
        7   1
        8   1
        9   1
    */
    const uint32_t offset_from_sequence_line = pos - pos_limit;
    uint32_t newlines_passed = offset_from_sequence_line / (padding + 1);// number of newlines passed (within the sequence part)
    uint32_t nucleotide_pos = offset_from_sequence_line - newlines_passed;// requested nucleotide in file
    // calculate file position for next twobit
    // when we are in an OPEN n block, we need to go to the first non-N base after, and place the file pointer there
    uint32_t n_passed = 0;
    this->get_n_offset(nucleotide_pos, &n_passed);
    fh->seekg((uint32_t) this->data_position + 4 + ((nucleotide_pos - n_passed) / 4), fh->beg);
    /*
     0  0  0  0  1  1  1  1 << desired offset from starting point
     A  C  T  G  A  C  T  G
    *

    handigste is om file pointer naar de byte ervoor te zetten
    vervolgens wanneer twobit_offset gelijk is aan nul, lees je de volgende byte
    * nooit out of bound

    */
    twobit_byte t = twobit_byte();
    const char *chunk = twobit_byte::twobit_hash[0];
    unsigned char twobit_offset = (nucleotide_pos - n_passed) % 4;
    if(twobit_offset != 0) {
        fh->read((char*)(&t.data), 1);
        chunk = t.get();
    }
    while(n_block > 0 and pos <= n_ends_w[n_block - 1]) { // iterate back
        n_block--;
    }
    // write sequence
    pos_limit += newlines_passed * (padding + 1);// passed sequence-containg lines
    while(newlines_passed < total_sequence_containing_lines) { // only 'complete' lines that are guarenteed 'padding' number of nucleotides long [ this loop starts at one to be unsigned-safe ]
        pos_limit += std::min(padding, this->n - (newlines_passed * padding));// only last line needs to be smaller ~ calculate from the beginning of newlines_passed
        // write nucleotides
        while(pos < pos_limit) {// while next sequence-containing-line is open
            if(pos >= n_starts_w[n_block]) {
                buffer[written++] = 'N';
            } else {
                if(twobit_offset % 4 == 0) {
                    fh->read((char*)(&t.data), 1);
                    chunk = t.get();
                }
                buffer[written++] = chunk[twobit_offset];
                twobit_offset = (unsigned char)(twobit_offset + 1) % 4;
            }
            if(pos == n_ends_w[n_block]) {
                n_block++;
            }
            pos++;
            if(written >= buffer_size) {
                fh->clear();
                return written;
            }
        }
        // write newline
        pos_limit += 1;
        if(pos < pos_limit) {
            buffer[written++] = '\n';
            pos++;
            if(written >= buffer_size) {
                fh->clear();
                return written;
            }
        }
        newlines_passed++;
    }
    fh->clear();
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
std::string fastafs_seq::sha1(ffs2f_init_seq* cache, std::ifstream *fh)
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
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    fh->clear();
    // "(a/b)*b + a%b shall equal a"
    // full iterations = this->n / chunk_size; do this number of iterations looped
    unsigned long n_iterations = (unsigned long) this->n / chunksize;
    signed int remaining_bytes = this->n % chunksize;
    // half iteration remainder = this->n % chunk_size; if this number > 0; do it too
    for(uint32_t i = 0; i < n_iterations; i++) {
        this->view_fasta_chunk_cached(cache, chunk,
                                      chunksize,
                                      header_offset + (i * chunksize),
                                      fh);
        SHA1_Update(&ctx, chunk, chunksize);
    }
    if(remaining_bytes > 0) {
        this->view_fasta_chunk_cached(cache, chunk, remaining_bytes, header_offset + (n_iterations * chunksize), fh);
        SHA1_Update(&ctx, chunk, remaining_bytes);
        chunk[remaining_bytes] = '\0';
    }
    //printf(" (%i * %i) + %i =  %i  = %i\n", n_iterations , chunksize, remaining_bytes , (n_iterations * chunksize) + remaining_bytes , this->n);
    unsigned char sha1_digest[SHA_DIGEST_LENGTH];
    SHA1_Final(sha1_digest, &ctx);
    fh->clear(); // because gseek was done before
    char sha1_hash[41];
    sha1_digest_to_hash(sha1_digest, sha1_hash);
    return std::string(sha1_hash);
}

uint32_t fastafs_seq::n_twobits()
{
    // if n actg bits is:
    // 0 -> 0
    // 1,2,3 and 4 -> 1
    uint32_t n = this->n;
    for(uint32_t i = 0; i < this->n_starts.size(); i++) {
        n -= n_ends[i] - this->n_starts[i] + 1;
    }
    return (n + 3) / 4;
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
    name(arg_name)
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
    char *memblock;
    std::ifstream file(afilename, std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        this->filename = afilename;
        size = file.tellg();
        if(size < 16) {
            file.close();
            throw std::invalid_argument("Corrupt file: " + filename);
        } else {
            memblock = new char [20 + 1]; //sha1 is 20b
            file.seekg(0, std::ios::beg);
            uint32_t i;
            // HEADER
            file.read(memblock, 14);
            memblock[16] = '\0';
            // check magic
            for(i = 0 ; i < 4;  i++) {
                if(memblock[i] != FASTAFS_MAGIC[i]) {
                    throw std::invalid_argument("Corrupt file: " + filename);
                }
            }
            for(i = 4 ; i < 8;  i++) {
                if(memblock[i] != FASTAFS_VERSION[i]) {
                    throw std::invalid_argument("Corrupt file: " + filename);
                }
            }
            this->flag = twobytes_to_uint(&memblock[8]);
            std::streampos file_cursor = (std::streampos) fourbytes_to_uint(&memblock[10], 0);
            // INDEX
            file.seekg(file_cursor, std::ios::beg);
            file.read(memblock, 4);
            this->data.resize(fourbytes_to_uint(memblock, 0));//n_seq becomes this->data.size()
            unsigned char j;
            fastafs_seq *s;
            for(i = 0; i < this->data.size(); i ++) {
                s = new fastafs_seq;
                // flag
                file.read(memblock, 2);
                s->flag = twobytes_to_uint(memblock);
                // name length
                file.read(memblock, 1);
                // name
                char name[memblock[0] + 1];
                file.read(name, memblock[0]);
                name[(unsigned char) memblock[0]] = '\0';
                s->name = std::string(name);
                // set cursor and save sequence data position
                file.read(memblock, 4);
                file_cursor = file.tellg();
                s->data_position = fourbytes_to_uint(memblock, 0);
                file.seekg((uint32_t) s->data_position, file.beg);
                {
                    // sequence stuff
                    // n compressed nucleotides
                    file.read(memblock, 4);
                    s->n = fourbytes_to_uint(memblock, 0);
                    // skip nucleotides
                    file.seekg((uint32_t) s->data_position + 4 + ((s->n + 3) / 4), file.beg);
                    // N-blocks (and update this->n instantly)
                    file.read(memblock, 4);
                    uint32_t N_blocks = fourbytes_to_uint(memblock, 0);
                    s->n_starts.resize(N_blocks);
                    s->n_ends.resize(N_blocks);
                    for(j = 0; j < s->n_starts.size(); j++) {
                        file.read(memblock, 4);
                        s->n_starts[j] = fourbytes_to_uint(memblock, 0);
                    }
                    for(j = 0; j < s->n_ends.size(); j++) {
                        file.read(memblock, 4);
                        s->n_ends[j] = fourbytes_to_uint(memblock, 0);
                        s->n += s->n_ends[j] - s->n_starts[j] + 1;
                    }
                    // SHA1-checksum
                    file.read(memblock, 20);
                    for(int j = 0; j < 20 ; j ++) {
                        s->sha1_digest[j] = memblock[j];
                    }
                    // M-blocks
                    file.read(memblock, 4);
                    uint32_t M_blocks = fourbytes_to_uint(memblock, 0);
                    s->m_starts.resize(M_blocks);
                    s->m_ends.resize(M_blocks);
                    for(j = 0; j < s->m_starts.size(); j++) {
                        file.read(memblock, 4);
                        s->m_starts[j] = fourbytes_to_uint(memblock, 0);
                    }
                    for(j = 0; j < s->m_ends.size(); j++) {
                        file.read(memblock, 4);
                        s->m_ends[j] = fourbytes_to_uint(memblock, 0);
                    }
                }
                file.seekg(file_cursor, file.beg);
                this->data[i] = s;
            }
            file.close();
            delete[] memblock;
        }
    } else {
        std::cout << "Unable to open file";
    }
}


void fastafs::view_fasta(ffs2f_init* cache)
{
    if(this->filename.size() == 0) {
        throw std::invalid_argument("No filename found");
    }
    std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        for(uint32_t i = 0; i < this->data.size(); i++) {
            this->data[i]->view_fasta(cache->sequences[i], &file);
        }
        file.close();
    }
}




ffs2f_init* fastafs::init_ffs2f(uint32_t padding, bool allow_masking)
{
    ffs2f_init *ddata = new ffs2f_init(this->data.size(), padding);
    for(size_t i = 0; i < this->data.size(); i++) {
        ddata->sequences[i] = this->data[i]->init_ffs2f_seq(padding, allow_masking);
    }
    return ddata;
}

/*
 * fastafs::view_fasta_chunk_cached -
 *
 * @cache:
 * @buffer:
 * @buffer_size:
 * @file_offset:
 *
 * returns
 */
uint32_t fastafs::view_fasta_chunk_cached(
    ffs2f_init* cache,
    char *buffer,

    size_t buffer_size,
    off_t file_offset)
{
    uint32_t written = 0;
    std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        size_t i = 0;// sequence iterator
        uint32_t pos = (uint32_t) file_offset;
        fastafs_seq *seq;
        while(i < data.size()) {
            seq = this->data[i];
            const uint32_t sequence_file_size = seq->fasta_filesize(cache->padding_arg);
            if(pos < sequence_file_size) {
                const uint32_t written_seq = seq->view_fasta_chunk_cached(
                                                 cache->sequences[i],
                                                 &buffer[written],
                                                 std::min((uint32_t) buffer_size - written, sequence_file_size),
                                                 pos,
                                                 &file);
                written += written_seq;
                pos -= (sequence_file_size - written_seq);
                if(written == buffer_size) {
                    file.close();
                    return written;
                }
            } else {
                pos -= sequence_file_size;
            }
            i++;
        }
        file.close();
    } else {
        throw std::runtime_error("could not load fastafs: " + this->filename);
    }
    return written;
}

uint32_t fastafs::view_fasta_chunk(uint32_t padding, char *buffer, size_t buffer_size, off_t file_offset)
{
    uint32_t written = 0;
    std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        size_t i = 0;// sequence iterator
        uint32_t pos = (uint32_t) file_offset;
        fastafs_seq *seq;
        while(i < data.size()) {
            seq = this->data[i];
            const uint32_t sequence_file_size = seq->fasta_filesize(padding);
            if(pos < sequence_file_size) {
                const uint32_t written_seq = seq->view_fasta_chunk(
                                                 padding,
                                                 &buffer[written],
                                                 pos,
                                                 std::min((uint32_t) buffer_size - written, sequence_file_size),
                                                 &file);
                written += written_seq;
                pos -= (sequence_file_size - written_seq);
                if(written == buffer_size) {
                    file.close();
                    return written;
                }
            } else {
                pos -= sequence_file_size;
            }
            i++;
        }
        file.close();
    } else {
        throw std::runtime_error("could not load fastafs: " + this->filename);
    }
    return written;
}



off_t fastafs::ucsc2bit_filesize(void)
{
    off_t nn = 4 + 4 + 4 + 4;// header, version, n-seq, rsrvd
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


//http://genome.ucsc.edu/FAQ/FAQformat.html#format7
//https://www.mathsisfun.com/binary-decimal-hexadecimal-converter.html
uint32_t fastafs::view_ucsc2bit_chunk(char *buffer, size_t buffer_size, off_t file_offset)
{
    uint32_t written = 0;
    uint32_t pos = (uint32_t) file_offset; // iterator (position, in bytes) in file
    uint32_t pos_limit = 0; // counter to keep track of when writing needs to stop for given loop
    std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
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
            twobit_byte t;
            pos_limit += full_twobits;
            while(pos < pos_limit) {
                //printf("%i - %i  = %i  ||  %i\n",pos_limit,pos, (full_twobits - (pos_limit - pos)) * 4, j);
                //sequence->view_fasta_chunk(0, n_seq, sequence->name.size() + 2 + ((full_twobits - (pos_limit - pos)) * 4), 4, &file);
                sequence->view_fasta_chunk_cached(cache->sequences[i], n_seq, 4, sequence->name.size() + 2 + ((full_twobits - (pos_limit - pos)) * 4), &file);
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
                    sequence->view_fasta_chunk_cached(cache->sequences[i], n_seq, sequence->n - (full_twobits * 4), sequence->name.size() + 2 + full_twobits * 4, &file);
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
        file.close();
    } else {
        throw std::runtime_error("could not load fastafs: " + this->filename);
    }
    return written;
}






uint32_t fastafs::fasta_filesize(uint32_t padding)
{
    uint32_t n = 0;
    std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        file.close();
        for(uint32_t i = 0; i < this->data.size(); i++) {
            n += this->data[i]->fasta_filesize(padding);
        }
    } else {
        throw std::runtime_error("could not load fastafs: " + this->filename);
    }
    return n;
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
        for(uint32_t i = 0; i < this->data.size(); i++) {
            offset += 1;// '>'
            offset += (uint32_t) this->data[i]->name.size() + 1; // "chr1\n"
            contents += data[i]->name + "\t" + std::to_string(this->data[i]->n) + "\t" + std::to_string(offset) + "\t" + padding_s + "\t" + padding_s2 + "\n";
            offset += this->data[i]->n; // ACTG NNN
            offset += (this->data[i]->n + (padding - 1)) / padding;// number of newlines corresponding to ACTG NNN lines
        }
        //while(written < buffer_size and written + file_offset < contents.size()) {
        //	buffer[written] = contents[written];
        //	written++;
        //	}
    } else {
        throw std::runtime_error("could not load fastafs: " + this->filename);
    }
    return contents;
}



uint32_t fastafs::view_faidx_chunk(uint32_t padding, char *buffer, size_t buffer_size, off_t file_offset)
{
    std::string contents = this->get_faidx(padding);
    uint32_t written = 0;
    while(written < buffer_size and written + file_offset < (uint32_t) contents.size()) {
        buffer[written] = contents[written];
        written++;
    }
    return written;
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
    char sha1_hash[41] = "";
    sha1_hash[40] = '\0';
    std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        std::cout << "FASTAFS NAME: " << this->filename << "\n";
        printf("SEQUENCES:    %u\n", (uint32_t) this->data.size());
        for(uint32_t i = 0; i < this->data.size(); i++) {
            sha1_digest_to_hash(this->data[i]->sha1_digest, sha1_hash);
            if(ena_verify_checksum) {
                //wget header of:
                //https://www.ebi.ac.uk/ena/cram/sha1/<sha1>
                //std::cout << "https://www.ebi.ac.uk/ena/cram/sha1/" << sha1_hash << "\n";
                SSL *ssl;
                //int sock_ssl = 0;
                //struct sockadfiledr_in address;
                int sock = 0;
                struct sockaddr_in serv_addr;
                std::string hello2 = "GET /ena/cram/sha1/" + std::string(sha1_hash) + " HTTP/1.1\r\nHost: www.ebi.ac.uk\r\nConnection: Keep-Alive\r\n\r\n";
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
                    printf("    >%-24s%-12i%s   <connection error>\n", this->data[i]->name.c_str(), this->data[i]->n, sha1_hash);
                } else if(std::string(buffer).find(" 200 ") != (size_t) -1) { // sequence is in ENA
                    printf("    >%-24s%-12i%s   https://www.ebi.ac.uk/ena/cram/sha1/%s\n", this->data[i]->name.c_str(), this->data[i]->n, sha1_hash, sha1_hash);
                } else {
                    printf("    >%-24s%-12i%s   ---\n", this->data[i]->name.c_str(), this->data[i]->n, sha1_hash);
                }
            } else {
                printf("    >%-24s%-12i%s\n", this->data[i]->name.c_str(), this->data[i]->n, sha1_hash);
            }
        }
        file.close();
    }
    return 0;
}


int fastafs::check_integrity()
{
    if(this->filename.size() == 0) {
        throw std::invalid_argument("No filename found");
    }
    int retcode = 0;
    char sha1_hash[41] = "";
    sha1_hash[40] = '\0';
    std::string old_hash;
    ffs2f_init* cache = this->init_ffs2f(0, false);// do not use masking, this checksum requires capital / upper case nucleotides
    std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        for(uint32_t i = 0; i < this->data.size(); i++) {
            sha1_digest_to_hash(this->data[i]->sha1_digest, sha1_hash);
            old_hash = std::string(sha1_hash);
            /*
             *     const uint32_t padding;// padding used for this sequence, cannot be 0
            const uint32_t total_sequence_containing_lines;// calculate total number of full nucleotide lines: (this->n + padding - 1) / padding

            std::vector<uint32_t> n_starts;
            std::vector<uint32_t> n_ends;
             * */
            std::string new_hash = this->data[i]->sha1(cache->sequences[i], &file);
            if(old_hash.compare(new_hash) == 0) {
                printf("OK\t%s\n", this->data[i]->name.c_str());
            } else {
                printf("ERROR\t%s\t%s != %s\n", this->data[i]->name.c_str(), sha1_hash, new_hash.c_str());
                retcode = EIO;
            }
        }
        file.close();
    }
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



std::string fastafs::basename()
{
    return "";
}
