#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>


#include "config.hpp"

#include "twobit_byte.hpp"
#include "ucsc2bit.hpp"
#include "utils.hpp"



ucsc2bit_seq::ucsc2bit_seq(): n(0)
{
}



uint32_t ucsc2bit_seq::fasta_filesize(uint32_t padding)
{
#if DEBUG
    if(padding == 0) {
        throw std::invalid_argument("Padding is set to 0, should have been set to this->n elsewhere.\n");
    }
#endif

    //               >                   chr                 \n  ACTG NNN  /number of newlines corresponding to ACTG NNN lines
    return 1 + (uint32_t) this->name.size() + 1 + this->n + (this->n + (padding - 1)) / padding;
}






//void ucsc2bit_seq::view_fasta(ffs2f_init_seq* cache, std::ifstream *fh)
//{
//char buffer[READ_BUFFER_SIZE];// = new char [READ_BUFFER_SIZE];
//uint32_t offset = 0;

////@todo figure out if a do {} while() loop isn't more in place here?
//uint32_t written = this->view_fasta_chunk_cached(cache, buffer, READ_BUFFER_SIZE, offset, fh);
//while(written > 0) {
//std::cout << std::string(buffer, written);
//offset += written;
//written = this->view_fasta_chunk_cached(cache, buffer, READ_BUFFER_SIZE, offset, fh);
//}
//}




uint32_t ucsc2bit_seq::view_fasta_chunk(uint32_t padding, char *buffer, size_t buffer_size, off_t start_pos_in_fasta, std::ifstream *fh)
{
#if DEBUG
    if(padding == 0) {
        throw std::runtime_error("Padding cannot be 0\n");
    }
#endif //DEBUG

    uint32_t written = 0;

    if(written >= buffer_size) { // requesting a buffer of size=0, should throw an exception?
        return written;
    }

//uint32_t pos = (uint32_t) start_pos_in_fasta;
//uint32_t pos_limit = 0;
//// >
//pos_limit += 1;
//if(pos < pos_limit) {
//buffer[written++] = '>';
//pos++;
//if(written >= buffer_size) {
//return written;
//}
//}
//// sequence name
//pos_limit += (uint32_t) this->name.size();
//while(pos < pos_limit) {
//buffer[written++] = this->name[this->name.size() - (pos_limit - pos)];
//pos++;
//if(written >= buffer_size) {
//return written;
//}
//}
//// \n
//pos_limit += 1;
//if(pos < pos_limit) {
//buffer[written++] = '\n';
//pos++;
//if(written >= buffer_size) {
//return written;
//}
//}
//const uint32_t offset_from_sequence_line = pos - pos_limit;
//size_t n_block = cache->n_starts.size();
//size_t m_block = cache->m_starts.size();
//uint32_t newlines_passed = offset_from_sequence_line / (cache->padding + 1);// number of newlines passed (within the sequence part)
//uint32_t nucleotide_pos = offset_from_sequence_line - newlines_passed;// requested nucleotide in file
//// calculate file position for next twobit
//// when we are in an OPEN n block, we need to go to the first non-N base after, and place the file pointer there
//uint32_t n_passed = 0;
//this->get_n_offset(nucleotide_pos, &n_passed);
//fh->seekg((uint32_t) this->data_position + 4 + ((nucleotide_pos - n_passed) / 4), fh->beg);
///*
//0  0  0  0  1  1  1  1 << desired offset from starting point
//A  C  T  G  A  C  T  G
//*

//handigste is om file pointer naar de byte ervoor te zetten
//vervolgens wanneer twobit_offset gelijk is aan nul, lees je de volgende byte
//* nooit out of bound

//*/
//twobit_byte t = twobit_byte();
//const char *chunk = twobit_byte::twobit_hash[0];
//unsigned char twobit_offset = (nucleotide_pos - n_passed) % 4;
//if(twobit_offset != 0) {
//fh->read((char*)(&t.data), 1);
//chunk = t.get();
//}
//while(n_block > 0 and pos <= cache->n_ends[n_block - 1]) { // iterate back
//n_block--;
//}
//while(m_block > 0 and pos <= cache->m_ends[m_block - 1]) { // iterate back
//m_block--;
//}
//// write sequence
//pos_limit += newlines_passed * (cache->padding + 1);// passed sequence-containg lines
//while(newlines_passed < cache->total_sequence_containing_lines) { // only 'complete' lines that are guarenteed 'padding' number of nucleotides long [ this loop starts at one to be unsigned-safe ]
//pos_limit += std::min(cache->padding, this->n - (newlines_passed * cache->padding));// only last line needs to be smaller ~ calculate from the beginning of newlines_passed
//// write nucleotides
//while(pos < pos_limit) {// while next sequence-containing-line is open
//if(pos >= cache->n_starts[n_block]) {
//if(pos >= cache->m_starts[m_block]) { // IN an m block; lower-case
//buffer[written++] = 'n';
//} else {
//buffer[written++] = 'N';
//}
//} else {
//if(twobit_offset % 4 == 0) {
//fh->read((char*)(&t.data), 1);
//chunk = t.get();
//}

//if(pos >= cache->m_starts[m_block]) { // IN an m block; lower-case
//buffer[written++] = (unsigned char)(chunk[twobit_offset] + 32);
//} else {
//buffer[written++] = chunk[twobit_offset];
//}

//twobit_offset = (unsigned char)(twobit_offset + 1) % 4;
//}
//if(pos == cache->n_ends[n_block]) {
//n_block++;
//}
//if(pos == cache->m_ends[m_block]) {
//m_block++;
//}
//pos++;
//if(written >= buffer_size) {
////fh->clear();
//return written;
//}
//}
//// write newline
//pos_limit += 1;
//if(pos < pos_limit) {
//buffer[written++] = '\n';
//pos++;
//if(written >= buffer_size) {
////fh->clear();
//return written;
//}
//}
//newlines_passed++;
//}

    //fh->clear();
    return written;
}





//@brief calculates the number of paddings found in a sequence of length N with
uint32_t ucsc2bit_seq::n_padding(uint32_t offset, uint32_t position_until, uint32_t padding)
{
    uint32_t n = (position_until + 1) / (padding + 1);
    // minus all the n's that occurred before offset
    if(offset > 0) {
        n -= ucsc2bit_seq::n_padding(0, offset - 1, padding);
    }
    return n;
}







ucsc2bit::ucsc2bit(std::string arg_name) :
    name(arg_name)
{
}

ucsc2bit::~ucsc2bit()
{
    for(uint32_t i = 0; i < this->data.size(); i++) {
        delete this->data[i];
    }
}

void ucsc2bit::load(std::string afilename)
{
    std::streampos size; // to determine ucsc2bit file size
    char *memblock; // buffer?

    std::ifstream file(afilename, std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        this->filename = afilename;
        size = file.tellg();

        if(size < 16) {
            file.close();
            throw std::invalid_argument("Corrupt file: " + filename);
        } else {
            memblock = new char [20 + 1]; //
            file.seekg(0, std::ios::beg);
            uint32_t i;

            // HEADER
            file.read(memblock, 8);
            memblock[16] = '\0';

            // check magic
            for(i = 0 ; i < 4;  i++) {
                if(memblock[i] != UCSC2BIT_MAGIC[i]) {
                    throw std::invalid_argument("Corrupt file: " + filename);
                }
            }

            // check version
            for(i = 4 ; i < 8;  i++) {
                if(memblock[i] != UCSC2BIT_VERSION[i]) {
                    throw std::invalid_argument("Corrupt file: " + filename);
                }
            }

            // read sequenceCount and reserve that number of memblocks
            file.read(memblock, 4);
            this->data.resize(fourbytes_to_uint_ucsc2bit(memblock, 0));//n_seq becomes this->data.size()

            // 4 x reserved 0
            file.read(memblock, 4);
            for(i = 0 ; i < 4;  i++) {
                if(memblock[i] != '\0') {
                    throw std::invalid_argument("Corrupt file: " + filename);
                }
            }

            ucsc2bit_seq *s;
            for(i = 0; i < this->data.size(); i ++) {
                s = new ucsc2bit_seq;

                // name length
                file.read(memblock, 1);

                // name
                char name[memblock[0] + 1];
                file.read(name, memblock[0]);
                name[(unsigned char) memblock[0]] = '\0';
                s->name = std::string(name);

                // file offset for seq-block
                file.read(memblock, 4);
                s->data_position = fourbytes_to_uint_ucsc2bit(memblock, 0);
                printf("data-pos: %d\n", s->data_position);


                this->data[i] = s;
            }

            printf("\n\n");

            size_t j;
            for(i = 0; i < this->data.size(); i ++) {
                s = data[i];
                file.seekg(s->data_position, std::ios::beg);

                file.read(memblock, 4);
                s->n = fourbytes_to_uint_ucsc2bit(memblock, 0);
                printf("dna-size: %d\n", s->n);

                // n blocks
                file.read(memblock, 4);
                uint32_t n_blocks = fourbytes_to_uint_ucsc2bit(memblock, 0);
                s->n_starts.resize(n_blocks);
                s->n_ends.resize(n_blocks);
                printf("n_blocks: %d\n", n_blocks);
                for(j = 0; j < n_blocks; j++) {
                    file.read(memblock, 8);
                    uint32_t n_block_s = fourbytes_to_uint_ucsc2bit(memblock, 0);

                    s->n_starts[j] = n_block_s;
                    s->n_ends[j] = n_block_s + fourbytes_to_uint_ucsc2bit(memblock, 4) - 1;
                }

                // m blocks
                file.read(memblock, 4);
                uint32_t m_blocks = fourbytes_to_uint_ucsc2bit(memblock, 0);
                s->m_starts.resize(m_blocks);
                s->m_ends.resize(m_blocks);
                printf("m_blocks: %d\n", m_blocks);
                for(j = 0; j < m_blocks; j++) {
                    file.read(memblock, 8);
                    uint32_t m_block_s = fourbytes_to_uint_ucsc2bit(memblock, 0);

                    s->m_starts[j] = m_block_s;
                    s->m_ends[j] = m_block_s + fourbytes_to_uint_ucsc2bit(memblock, 4) - 1;
                }

                s->sequence_data_position = s->data_position + 4 + 4 + 4 + (8 * n_blocks) + (8 * m_blocks);

                printf("\n");
            }

            file.close();
            delete[] memblock;
        }
    } else {
        throw std::invalid_argument("Unable to open file '" + afilename + "'");
    }
}



//void ucsc2bit::view_fasta(ffs2f_init* cache)
//{
//if(this->filename.size() == 0) {
//throw std::invalid_argument("No filename found");
//}

//std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
//if(file.is_open()) {
//for(uint32_t i = 0; i < this->data.size(); i++) {
//this->data[i]->view_fasta(cache->sequences[i], &file);
//}
//file.close();
//}
//}




//ffs2f_init* ucsc2bit::init_ffs2f(uint32_t padding, bool allow_masking)
//{
//ffs2f_init *ddata = new ffs2f_init(this->data.size(), padding);

//for(size_t i = 0; i < this->data.size(); i++) {
//ddata->sequences[i] = this->data[i]->init_ffs2f_seq(padding, allow_masking);
//}



//return ddata;
//}

/*
* ucsc2bit::view_fasta_chunk_cached -
*
* @padding: size of padding - placement of newlines (default = 60)
* @buffer:
* @buffer_size:
* @file_offset:
*
* returns number of bytes written to buffer
*/
uint32_t ucsc2bit::view_fasta_chunk(uint32_t padding, char *buffer, size_t buffer_size, off_t file_offset)
{
    uint32_t written = 0;

    std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        size_t i = 0;// sequence iterator
        uint32_t pos = (uint32_t) file_offset;
        ucsc2bit_seq *seq;

        while(i < data.size()) {
            seq = this->data[i];
            const uint32_t sequence_file_size = seq->fasta_filesize(padding);

            if(pos < sequence_file_size) {
                const uint32_t written_seq = 0;
                /*
                seq->view_fasta_chunk_cached(
                    cache->sequences[i],
                    &buffer[written],
                    std::min((uint32_t) buffer_size - written, sequence_file_size),
                    pos,
                    &file);
                */

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
        throw std::runtime_error("[ucsc2bit::view_fasta_chunk_cached] could not load ucsc2bit: " + this->filename);
    }
    return written;
}



size_t ucsc2bit::fasta_filesize(uint32_t padding)
{
    size_t n = 0;

    for(size_t i = 0; i < this->data.size(); i++) {
        n += this->data[i]->fasta_filesize(padding);
    }

    return n;
}




//std::string ucsc2bit::get_faidx(uint32_t padding)
//{
//std::string contents = "";
//std::string padding_s = std::to_string(padding);
//std::string padding_s2 = std::to_string(padding + 1);// padding + newline

//std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

//if(file.is_open()) {
//file.close();
//uint32_t offset = 0;

//for(size_t i = 0; i < this->data.size(); i++) {
//offset += 1;// '>'
//offset += (uint32_t) this->data[i]->name.size() + 1; // "chr1\n"

//contents += data[i]->name + "\t" + std::to_string(this->data[i]->n) + "\t" + std::to_string(offset) + "\t" + padding_s + "\t" + padding_s2 + "\n";

//offset += this->data[i]->n; // ACTG NNN
//offset += (this->data[i]->n + (padding - 1)) / padding;// number of newlines corresponding to ACTG NNN lines
//}
////while(written < buffer_size and written + file_offset < contents.size()) {
////	buffer[written] = contents[written];
////	written++;
////	}
//} else {
//throw std::runtime_error("[ucsc2bit::get_faidx] could not load ucsc2bit: " + this->filename);
//}
//return contents;
//}



//uint32_t ucsc2bit::view_faidx_chunk(uint32_t padding, char *buffer, size_t buffer_size, off_t file_offset)
//{
//std::string contents = this->get_faidx(padding);
//uint32_t written = 0;

//while(written < buffer_size and written + file_offset < (uint32_t) contents.size()) {
//buffer[written] = contents[written];
//written++;
//}

//return written;
//}



uint32_t ucsc2bit::n()
{
    uint32_t n = 0;
    for(unsigned i = 0; i < this->data.size(); i++) {
        n += this->data[i]->n;
    }
    return n;
}

