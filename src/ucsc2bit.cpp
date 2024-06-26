#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
//#include <filesystem>


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


    // set file pointer to sequqnece data?
    const uint32_t offset_from_sequence_line = pos - pos_limit;
    uint32_t newlines_passed = offset_from_sequence_line / (padding + 1);// number of newlines passed (within the sequence part)


    uint32_t nucleotide_pos = offset_from_sequence_line - newlines_passed;// requested nucleotide in file
    fh->seekg((uint32_t) this->sequence_data_position + ((nucleotide_pos) / 4), std::ios::beg);// std::ios::beg | fh->beg

    twobit_byte t = twobit_byte(ENCODE_HASH_TWOBIT_DNA);
    const char *chunk = t.encode_hash[0];

    unsigned char twobit_offset = nucleotide_pos % 4;

    if(twobit_offset != 0) {
        fh->read((char *)(&t.data), 1);
        chunk = t.get();
    }

    uint32_t fs_max = (uint32_t) this->fasta_filesize(padding) + 2;

    size_t n_block = this->n_starts.size();
    size_t m_block = this->m_starts.size();

    this->n_ends.push_back(fs_max);
    this->n_starts.push_back(fs_max);
    while(n_block > 0 and pos <= this->n_ends[n_block - 1] + newlines_passed + this->name.size() + 2) { // iterate back
        n_block--;
    }

    this->m_ends.push_back(fs_max);
    this->m_starts.push_back(fs_max);
    while(m_block > 0 and pos <= this->m_ends[m_block - 1] + newlines_passed + this->name.size() + 2) { // iterate back
        m_block--;
    }

    // write sequence
    pos_limit += newlines_passed * (padding + 1);// passed sequence-containg lines
    uint32_t sequence_lines = (this->n + padding - 1) / padding;

    while(newlines_passed < sequence_lines) { // only 'complete' lines that are guarenteed 'padding' number of nucleotides long [ this loop starts at one to be unsigned-safe ]
        pos_limit += std::min(padding, this->n - (newlines_passed * padding));// only last line needs to be smaller ~ calculate from the beginning of newlines_passed

        // write nucleotides
        while(pos < pos_limit) {// while next sequence-containing-line is open
            if(twobit_offset % 4 == 0) {
                fh->read((char *)(&t.data), 1);
                chunk = t.get();
            }

            if(pos >= this->n_starts[n_block] + newlines_passed + this->name.size() + 2) {
                buffer[written] = 'N';
            } else {
                buffer[written] = chunk[twobit_offset];
            }

            if(pos >= this->m_starts[m_block] + newlines_passed + this->name.size() + 2) {
                buffer[written] = (unsigned char)(buffer[written] + 32);
            }

            written++;



            twobit_offset = (unsigned char)(twobit_offset + 1) % 4;

            if(pos == this->n_ends[n_block]  + newlines_passed + this->name.size() + 2) {
                n_block++;
            }
            if(pos == this->m_ends[m_block]  + newlines_passed + this->name.size() + 2) {
                m_block++;
            }

            pos++;

            if(written >= buffer_size) {
                //fh->clear();
                this->n_starts.pop_back();
                this->n_ends.pop_back();

                this->m_starts.pop_back();
                this->m_ends.pop_back();

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

                this->n_starts.pop_back();
                this->n_ends.pop_back();

                this->m_starts.pop_back();
                this->m_ends.pop_back();

                return written;
            }
        }

        newlines_passed++;
    }

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
    std::ifstream file(afilename, std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        // if a user can't compile this line, please replace it with C's
        // 'realpath' function and delete/free afterwards and send a PR
        //this->filename = std::filesystem::canonical(afilename);// this path must be absolute because if stuff gets send to FUSE, paths are relative to the FUSE process and probably systemd initialization
        this->filename = realpath_cpp(afilename);

        if(file.tellg() < 16) {
            file.close();
            throw std::invalid_argument("Corrupt file: " + filename);
        } else {
            unsigned char *memblock = new unsigned char [20 + 1]; // buffer
            if(memblock == 0) {
                throw std::invalid_argument("Could not alloc\n");
            }
            memblock[20] = '\0';

            file.seekg(0, std::ios::beg);
            uint32_t i;

            // HEADER
            if(!file.read((char *) &memblock[0], 16)) {
                delete[] memblock;
                throw std::invalid_argument("Corrupt, unreadable or truncated file (early EOF): " + filename);
            }

            // check magic
            for(i = 0 ; i < 4;  i++) {
                if(memblock[i] != UCSC2BIT_MAGIC[i]) {
                    delete[] memblock;
                    throw std::invalid_argument("Not a 2bit file: " + filename);
                }
            }

            // check version
            for(i = 0 ; i < 4;  i++) {
                if(memblock[i + 4] != UCSC2BIT_VERSION[i]) {
                    delete[] memblock;
                    throw std::invalid_argument("Corrupt 2bit file. unknown version: " + filename);
                }
            }

            // read sequenceCount and reserve that number of memblocks
            this->data.resize(fourbytes_to_uint_ucsc2bit(memblock, 8));//n_seq becomes this->data.size()

            // 4 x reserved 0
            for(i = 12 ; i < 16; i++) {
                if(memblock[i] != '\0') {
                    delete[] memblock;
                    throw std::invalid_argument("Corrupt 2bit file: " + filename);
                }
            }

            ucsc2bit_seq *s;
            for(i = 0; i < this->data.size(); i ++) {
                s = new ucsc2bit_seq;

                // name length
                if(!file.read((char *) &memblock[0], 1)) {
                    delete[] memblock;
                    throw std::invalid_argument("Corrupt, unreadable or truncated file (early EOF): " + filename);
                }

                // name
                //char name[memblock[0] + 1];
                char *name = new char[memblock[0] + 1];
                if(!file.read(name, memblock[0])) {
                    delete[] memblock;
                    throw std::invalid_argument("Corrupt, unreadable or truncated file (early EOF): " + filename);
                }

                name[(unsigned char) memblock[0]] = '\0';
                s->name = std::string(name);
                delete[] name;

                // file offset for seq-block
                if(!file.read((char *) &memblock[0], 4)) {
                    delete[] memblock;
                    throw std::invalid_argument("Corrupt, unreadable or truncated file (early EOF): " + filename);
                }
                s->data_position = fourbytes_to_uint_ucsc2bit(memblock, 0);


                this->data[i] = s;
            }

            size_t j;
            for(i = 0; i < this->data.size(); i ++) {
                s = data[i];
                file.seekg(s->data_position, std::ios::beg);

                file.read((char *) &memblock[0], 4);
                s->n = fourbytes_to_uint_ucsc2bit(memblock, 0);

                // n blocks
                if(!file.read((char *) &memblock[0], 4)) {
                    delete[] memblock;
                    throw std::invalid_argument("Corrupt, unreadable or truncated file (early EOF): " + filename);
                }
                uint32_t n_blocks = fourbytes_to_uint_ucsc2bit(memblock, 0);
                s->n_starts.resize(n_blocks);
                s->n_ends.resize(n_blocks);
                for(j = 0; j < n_blocks; j++) {
                    file.read((char *) &memblock[0], 8);
                    uint32_t n_block_s = fourbytes_to_uint_ucsc2bit(memblock, 0);

                    s->n_starts[j] = n_block_s;
                    s->n_ends[j] = n_block_s + fourbytes_to_uint_ucsc2bit(memblock, 4) - 1;
                }

                // m blocks
                if(!file.read((char *) &memblock[0], 4)) {
                    delete[] memblock;
                    throw std::invalid_argument("Corrupt, unreadable or truncated file (early EOF): " + filename);
                }
                uint32_t m_blocks = fourbytes_to_uint_ucsc2bit(memblock, 0);
                s->m_starts.resize(m_blocks);
                s->m_ends.resize(m_blocks);
                for(j = 0; j < m_blocks; j++) {
                    file.read((char *) &memblock[0], 8);
                    uint32_t m_block_s = fourbytes_to_uint_ucsc2bit(memblock, 0);

                    s->m_starts[j] = m_block_s;
                    s->m_ends[j] = m_block_s + fourbytes_to_uint_ucsc2bit(memblock, 4) - 1;
                }

                s->sequence_data_position = s->data_position + 4 + 4 + 4 + 4 + (8 * n_blocks) + (8 * m_blocks);
            }

            file.close();
            delete[] memblock;
        }
    } else {
        throw std::invalid_argument("Unable to open file '" + afilename + "'");
    }
}




/*
* ucsc2bit::view_fasta_chunk -
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
                const uint32_t written_seq = seq->view_fasta_chunk(
                                                 padding,
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
        throw std::runtime_error("[ucsc2bit::view_fasta_chunk] could not load ucsc2bit: " + this->filename);
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



std::string ucsc2bit::get_faidx(uint32_t padding)
{
    std::string contents = "";
    std::string padding_s = std::to_string(padding);
    std::string padding_s2 = std::to_string(padding + 1);// padding + newline

    //std::ifstream file(this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    //if(file.is_open()) {
    //    file.close();
    uint32_t offset = 0;

    for(size_t i = 0; i < this->data.size(); i++) {
        offset += 1;// '>'
        offset += (uint32_t) this->data[i]->name.size() + 1; // "chr1\n"

        contents += data[i]->name + "\t" + std::to_string(this->data[i]->n) + "\t" + std::to_string(offset) + "\t" + padding_s + "\t" + padding_s2 + "\n";

        offset += this->data[i]->n; // ACTG NNN
        offset += (this->data[i]->n + (padding - 1)) / padding;// number of newlines corresponding to ACTG NNN lines
    }
    //} else {
    //    throw std::runtime_error("[ucsc2bit::get_faidx] could not load ucsc2bit: " + this->filename);
    //}

    return contents;
}



uint32_t ucsc2bit::view_faidx_chunk(uint32_t padding, char *buffer, size_t buffer_size, off_t file_offset)
{
    std::string contents = this->get_faidx(padding);
    uint32_t written = 0;

    while(written < buffer_size and written + file_offset < (uint32_t) contents.size()) {
        buffer[written] = contents[written];
        written++;
    }

    return written;
}



uint32_t ucsc2bit::n()
{
    uint32_t n = 0;

    for(unsigned i = 0; i < this->data.size(); i++) {
        n += this->data[i]->n;
    }

    return n;
}

