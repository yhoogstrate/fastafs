#include <iostream>
#include <fstream>
#include "zlib.h"

#include "config.hpp"

#include "fastafs.hpp"
#include "fasta_to_twobit_fastafs.hpp"
#include "flags.hpp"
#include "utils.hpp"



const static char nt[2] = "T";
const static char nc[2] = "C";
const static char na[2] = "A";
const static char ng[2] = "G";

const static char nn[2] = "N";


const static char nu[2] = "U";
const static char nr[2] = "R";
const static char ny[2] = "Y";
const static char nk[2] = "K";
const static char nm[2] = "M";
const static char ns[2] = "S";
const static char nw[2] = "W";
const static char nb[2] = "B";
const static char nd[2] = "D";
const static char nh[2] = "H";
const static char nv[2] = "V";




void fasta_seq_header_twobit_conversion_data::add_twobit_ACTG(unsigned char nucleotide, std::ofstream &fh_fastafs)
{
    this->twobit_data.set(twobit_byte::iterator_to_offset(this->n_actg), nucleotide);//0 = TU, 1 =

    // if fourth nucleotide, 2bit is complete; write to disk
    if(this->n_actg % 4 == 3) {
        fh_fastafs << this->twobit_data.data;
    }

    if(this->previous_was_N) {
        this->n_block_ends.push_back(this->n_actg + this->N - 1);
    }

    this->previous_was_N = false;
    this->n_actg++;
}

void fasta_seq_header_twobit_conversion_data::add_twobit_N()
{
    if(!this->previous_was_N) {
        this->n_block_starts.push_back(this->n_actg + this->N);
    }

    this->previous_was_N = true;
    this->N++;
}



void fasta_seq_header_twobit_conversion_data::finish_twobit_sequence(std::ofstream &fh_fastafs)
{
    uint32_t j;

    // flush last nucleotide
    if(this->n_actg % 4 != 0) {
        for(j = this->n_actg % 4; j < 4; j++) {
            this->twobit_data.set(twobit_byte::iterator_to_offset(j), 0);
        }
        fh_fastafs << this->twobit_data.data;
    }

    if(this->previous_was_N) {
        this->n_block_ends.push_back(this->n_actg + this->N - 1);
    }

    // do M block
    if(this->in_m_block) {
        this->m_block_ends.push_back(this->n_actg + this->N - 1);
        //printf("closing m-block: %u\n",this->n_actg + this->N - 1);
    }

#if DEBUG
    if(this->m_block_starts.size() != this->m_block_ends.size()) {
        throw std::runtime_error("M blocks not correctly parsed\n");
    }
#endif //DEBUG

    char buffer[4 +  1];

    // (over)write number nucleotides
    std::streamoff index_file_position = fh_fastafs.tellp();
    fh_fastafs.seekp(this->file_offset_in_fastafs, std::ios::beg);
    uint_to_fourbytes(buffer, this->n_actg);
    fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);

    fh_fastafs.seekp(index_file_position, std::ios::beg);

    // N blocks
    uint_to_fourbytes(buffer, (uint32_t) this->n_block_starts.size());
    fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    for(j = 0; j < this->n_block_starts.size(); j++) {
        uint_to_fourbytes(buffer, this->n_block_starts[j]);
        fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    }
    for(j = 0; j < this->n_block_ends.size(); j++) {
        uint_to_fourbytes(buffer, this->n_block_ends[j]);
        fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    }

    // write checksum
    MD5_Final(this->md5_digest, &this->ctx);
    fh_fastafs.write(reinterpret_cast<char *>(&this->md5_digest), (size_t) 16);

    // M blocks
    uint_to_fourbytes(buffer, (uint32_t) this->m_block_starts.size());
    fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    for(j = 0; j < this->m_block_starts.size(); j++) {
        uint_to_fourbytes(buffer, this->m_block_starts[j]);
        fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    }
    for(j = 0; j < this->m_block_ends.size(); j++) {
        uint_to_fourbytes(buffer, this->m_block_ends[j]);
        fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    }
}








void fasta_seq_header_twobit_conversion_data::add_fourbit_ACTG(unsigned char nucleotide, std::ofstream &fh_fastafs)
{
    this->fourbit_data.set(fourbit_byte::iterator_to_offset(this->n_actg), nucleotide);//0 = TU, 1 =

    // if fourth nucleotide, 2bit is complete; write to disk
    if(this->n_actg % 2 == 1) {
        fh_fastafs << this->fourbit_data.data;
    }

    if(this->previous_was_N) {
        this->n_block_ends.push_back(this->n_actg + this->N - 1);
    }

    this->previous_was_N = false;
    this->n_actg++;
}

void fasta_seq_header_twobit_conversion_data::add_fourbit_N()
{
    if(!this->previous_was_N) {
        this->n_block_starts.push_back(this->n_actg + this->N);
    }

    this->previous_was_N = true;
    this->N++;
}



void fasta_seq_header_twobit_conversion_data::finish_fourbit_sequence(std::ofstream &fh_fastafs)
{
    uint32_t j;

    // flush last nucleotide
    if(this->n_actg % 2 != 0) {
        this->fourbit_data.set(fourbit_byte::iterator_to_offset(this->n_actg), 0);

        fh_fastafs << this->fourbit_data.data;
    }

    if(this->previous_was_N) {
        this->n_block_ends.push_back(this->n_actg + this->N - 1);
    }

    // do M block
    if(this->in_m_block) {
        this->m_block_ends.push_back(this->n_actg + this->N - 1);
        //printf("closing m-block: %u\n",this->n_actg + this->N - 1);
    }

#if DEBUG
    if(this->m_block_starts.size() != this->m_block_ends.size()) {
        throw std::runtime_error("M blocks not correctly parsed\n");
    }
#endif //DEBUG

    char buffer[4 +  1];

    // (over)write number nucleotides
    std::streamoff index_file_position = fh_fastafs.tellp();
    fh_fastafs.seekp(this->file_offset_in_fasta, std::ios::beg);
    uint_to_fourbytes(buffer, this->n_actg);
    fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);

    fh_fastafs.seekp(index_file_position, std::ios::beg);

    // N blocks
    uint_to_fourbytes(buffer, (uint32_t) this->n_block_starts.size());
    fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    for(j = 0; j < this->n_block_starts.size(); j++) {
        uint_to_fourbytes(buffer, this->n_block_starts[j]);
        fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    }
    for(j = 0; j < this->n_block_ends.size(); j++) {
        uint_to_fourbytes(buffer, this->n_block_ends[j]);
        fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    }

    // write checksum
    MD5_Final(this->md5_digest, &this->ctx);
    fh_fastafs.write(reinterpret_cast<char *>(&this->md5_digest), (size_t) 16);

    // M blocks
    uint_to_fourbytes(buffer, (uint32_t) this->m_block_starts.size());
    fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    for(j = 0; j < this->m_block_starts.size(); j++) {
        uint_to_fourbytes(buffer, this->m_block_starts[j]);
        fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    }
    for(j = 0; j < this->m_block_ends.size(); j++) {
        uint_to_fourbytes(buffer, this->m_block_ends[j]);
        fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    }
}










size_t fasta_to_twobit_fastafs(const std::string &fasta_file, const std::string &fastafs_file)
{
    std::vector<fasta_seq_header_twobit_conversion_data*> index;
    fasta_seq_header_twobit_conversion_data* s;

    fastafs_flags ffsf;
    ffsf.set_incomplete();

    // @todo use ifstream and ofstream argument types
    std::string line;
    std::ifstream fh_fasta(fasta_file.c_str(), std::ios :: in | std::ios :: binary);
    std::ofstream fh_fastafs(fastafs_file.c_str(), std::ios :: out | std::ios :: binary);
    s = nullptr;
    if(fh_fasta.is_open() and fh_fastafs.is_open()) {
        fh_fastafs << FASTAFS_MAGIC;
        fh_fastafs << FASTAFS_VERSION;

        // the flag for now, set to INCOMPLETE as writing is in progress || spacer that will be overwritten later
        fh_fastafs << ffsf.get_bits()[0];
        fh_fastafs << ffsf.get_bits()[1];

        fh_fastafs << "\x00\x00\x00\x00"s;// position of metedata ~ unknown YET

        // iterate until first sequence is found, ensuring we won't write to uninitialized sequences
        while(s == nullptr and getline(fh_fasta, line)) {
            if(line[0] == '>') {
                line.erase(0, 1);// erases first part, quicker would be pointer from first char

                //s = new fasta_seq_header_twobit_conversion_data( fh_fastafs.tellp(), line);
                s = new fasta_seq_header_twobit_conversion_data(fh_fasta.tellg(), fh_fastafs.tellp(), line);
                fh_fastafs << "\x00\x00\x00\x00"s;// placeholder for sequence length
                index.push_back(s);
            }
        }

        if(s != nullptr) {
            while(getline(fh_fasta, line)) {
                if(line[0] == '>') {
                    if(s->current_dict == DICT_TWOBIT) {
                        s->finish_twobit_sequence(fh_fastafs);// finish last sequence
                    }
                    else {
                        s->finish_fourbit_sequence(fh_fastafs);// finish last sequence
                    }
                    line.erase(0, 1);// erases first part, quicker would be pointer from first char

                    //s = new fasta_seq_header_twobit_conversion_data(fh_fastafs.tellp(), line);
                    s = new fasta_seq_header_twobit_conversion_data(fh_fasta.tellg(), fh_fastafs.tellp(), line);
                    fh_fastafs << "\x00\x00\x00\x00"s;// number of 2bit encoded nucleotides, not yet known

                    index.push_back(s);
                } else {
                    for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
                        if(s->current_dict ==  DICT_TWOBIT ) {
                            switch(*it) {

                            // keeping daling with upper-case and lower-case in separate cases is quicker than one if/else before the switch, simply beacuse switches are faster than if-statements.
                            case 'U':
                            case 'T':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_twobit_ACTG(NUCLEOTIDE_T, fh_fastafs);
                                MD5_Update(&s->ctx, nt, 1);// this needs to be pu in add_twobit_Nucleotide
                                break;
                            case 'u':// lower case = m block
                            case 't':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_twobit_ACTG(NUCLEOTIDE_T, fh_fastafs);
                                MD5_Update(&s->ctx, nt, 1);// this needs to be pu in add_twobit_Nucleotide
                                break;
                            case 'C':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_twobit_ACTG(NUCLEOTIDE_C, fh_fastafs);
                                MD5_Update(&s->ctx, nc, 1);
                                break;
                            case 'c':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_twobit_ACTG(NUCLEOTIDE_C, fh_fastafs);
                                MD5_Update(&s->ctx, nc, 1);
                                break;
                            case 'A':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_twobit_ACTG(NUCLEOTIDE_A, fh_fastafs);
                                MD5_Update(&s->ctx, na, 1);
                                break;
                            case 'a':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_twobit_ACTG(NUCLEOTIDE_A, fh_fastafs);
                                MD5_Update(&s->ctx, na, 1);
                                break;
                            case 'G':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_twobit_ACTG(NUCLEOTIDE_G, fh_fastafs);
                                MD5_Update(&s->ctx, ng, 1);
                                break;
                            case 'g':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_twobit_ACTG(NUCLEOTIDE_G, fh_fastafs);
                                MD5_Update(&s->ctx, ng, 1);
                                break;
                            case 'N':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_twobit_N();
                                MD5_Update(&s->ctx, nn, 1);
                                break;
                            case 'n':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_twobit_N();
                                MD5_Update(&s->ctx, nn, 1);
                                break;
                            default:
                                // set to fourbit and re-intialize
                                // seek fasta header to beg + s->file_offset_in_fasta
                                // seek fastafs back to beg + s->file_offset_in_fastafs and overwrite
                                //throw std::runtime_error("[fasta_to_twobit_fastafs] invalid chars in FASTA file");
                                fh_fasta.seekg(s->file_offset_in_fasta, std::ios::beg);
                                fh_fastafs.seekp(s->file_offset_in_fastafs, std::ios::beg);
                                s->current_dict = DICT_FOURBIT;
                                printf("rollback back two files:)\n");
                                break;
                            }
                        }
                        else { // four bit decoding

                            switch(*it) {

                            case 'A':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(0, fh_fastafs);
                                MD5_Update(&s->ctx, na, 1);
                                break;
                            case 'a':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(0, fh_fastafs);
                                MD5_Update(&s->ctx, na, 1);
                                break;
                            case 'C':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(1, fh_fastafs);
                                MD5_Update(&s->ctx, nc, 1);
                                break;
                            case 'c':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(1, fh_fastafs);
                                MD5_Update(&s->ctx, nc, 1);
                                break;
                            case 'G':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(2, fh_fastafs);
                                MD5_Update(&s->ctx, ng, 1);
                                break;
                            case 'g':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(2, fh_fastafs);
                                MD5_Update(&s->ctx, ng, 1);
                                break;
                            case 'T':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(3, fh_fastafs);
                                MD5_Update(&s->ctx, nt, 1);
                                break;
                            case 't':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(3, fh_fastafs);
                                MD5_Update(&s->ctx, nt, 1);
                                break;
                            case 'U':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(4, fh_fastafs);
                                MD5_Update(&s->ctx, nu, 1);
                                break;
                            case 'u':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(4, fh_fastafs);
                                MD5_Update(&s->ctx, nu, 1);
                                break;

                            case 'R':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(5, fh_fastafs);
                                MD5_Update(&s->ctx, nr, 1);
                                break;
                            case 'r':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(5, fh_fastafs);
                                MD5_Update(&s->ctx, nr, 1);
                                break;
                            case 'Y':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(6, fh_fastafs);
                                MD5_Update(&s->ctx, ny, 1);
                                break;
                            case 'y':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(6, fh_fastafs);
                                MD5_Update(&s->ctx, ny, 1);
                                break;
                            case 'K':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(7, fh_fastafs);
                                MD5_Update(&s->ctx, nk, 1);
                                break;
                            case 'k':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(7, fh_fastafs);
                                MD5_Update(&s->ctx, nk, 1);
                                break;
                            case 'M':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(8, fh_fastafs);
                                MD5_Update(&s->ctx, nm, 1);
                                break;
                            case 'm':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(8, fh_fastafs);
                                MD5_Update(&s->ctx, nm, 1);
                                break;
                            case 'S':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(9, fh_fastafs);
                                MD5_Update(&s->ctx, ns, 1);
                                break;
                            case 's':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(9, fh_fastafs);
                                MD5_Update(&s->ctx, ns, 1);
                                break;
                            case 'W':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(10, fh_fastafs);
                                MD5_Update(&s->ctx, nw, 1);
                                break;
                            case 'w':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(10, fh_fastafs);
                                MD5_Update(&s->ctx, nw, 1);
                                break;
                            case 'B':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(11, fh_fastafs);
                                MD5_Update(&s->ctx, nb, 1);
                                break;
                            case 'b':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(11, fh_fastafs);
                                MD5_Update(&s->ctx, nb, 1);
                                break;
                            case 'D':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(12, fh_fastafs);
                                MD5_Update(&s->ctx, nd, 1);
                                break;
                            case 'd':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(12, fh_fastafs);
                                MD5_Update(&s->ctx, nd, 1);
                                break;
                            case 'H':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(13, fh_fastafs);
                                MD5_Update(&s->ctx, nh, 1);
                                break;
                            case 'h':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(13, fh_fastafs);
                                MD5_Update(&s->ctx, nh, 1);
                                break;
                            case 'V':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(14, fh_fastafs);
                                MD5_Update(&s->ctx, nv, 1);
                                break;
                            case 'v':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(14, fh_fastafs);
                                MD5_Update(&s->ctx, nv, 1);
                                break;
                            case 'N':
                                if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_fourbit_ACTG(15, fh_fastafs);
                                MD5_Update(&s->ctx, nn, 1);
                                break;
                            case 'n':
                                if(!s->in_m_block) {
                                    //printf("starting M block: %d\n", s->N + s->n_actg);
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_fourbit_ACTG(15, fh_fastafs);
                                MD5_Update(&s->ctx, nn, 1);
                                break;
                            case '-':
                                /*if(s->in_m_block) {
                                    //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }*/

                                s->add_fourbit_N();
                                //MD5_Update(&s->ctx, nn, 1);
                                break;

                            default:
                                throw std::runtime_error("[fasta_to_x_fastafs] invalid chars in FASTA file");
                                break;
                            }
                        }
                    }
                }
            }
        }
        fh_fasta.close();
    }
    if(s != nullptr) {
        if(s->current_dict == DICT_TWOBIT) {
            s->finish_twobit_sequence(fh_fastafs);// finish last sequence
        }
        else {
            s->finish_fourbit_sequence(fh_fastafs);// finish last sequence
        }
    }
    
    printf("checkpoint aa\n");

    // write index/footer
    unsigned int index_file_position = (uint32_t) fh_fastafs.tellp();
    char buffer[4 +  1];
    uint_to_fourbytes(buffer, (uint32_t) index.size());
    fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);

    for(size_t i = 0; i < index.size(); i++) {
        printf("checkpoint bb\n");

        s = index[i];

        // set and write flag
        fastafs_sequence_flags fsf;
        fsf.set_linear();

        if(s->current_dict == DICT_TWOBIT) {
            fsf.set_dna(); // @todo fix RNA
        }
        else {
            fsf.set_iupec_nucleotide();
        }

        fsf.set_dna();
        fsf.set_complete();
        fh_fastafs << fsf.get_bits()[0];
        fh_fastafs << fsf.get_bits()[1];

        // name
        unsigned char name_size = (unsigned char) s->name.size();
        fh_fastafs.write((char *) &name_size, 1); // name size
        fh_fastafs.write(s->name.c_str(), (size_t) s->name.size());// name

        // location of sequence data in file
        uint_to_fourbytes(buffer, (uint32_t) s->file_offset_in_fastafs);
        fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);

        delete s;
    }
    printf("checkpoint cc\n");

    
    fh_fastafs << "\x00"s;// no metadata tags (YET)

    // update header: set to updated
    fh_fastafs.seekp(8, std::ios::beg);
    ffsf.set_complete();
    fh_fastafs << ffsf.get_bits()[0];
    fh_fastafs << ffsf.get_bits()[1];

    uint_to_fourbytes(buffer, index_file_position);//position of header
    fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);

    fh_fasta.close();

    // now calculate crc32 checksum, as all bits have been set.
    fh_fastafs.seekp(0, std::ios::end);
/*
    fastafs f("");
    f.load(fastafs_file);
    uint32_t crc32c = f.get_crc32();

    char byte_enc[5] = "\x00\x00\x00\x00";
    uint_to_fourbytes(byte_enc, (uint32_t) crc32c);
    //printf("[%i][%i][%i][%i] input!! \n", byte_enc[0], byte_enc[1], byte_enc[2], byte_enc[3]);
    fh_fastafs.write(reinterpret_cast<char *>(&byte_enc), (size_t) 4);

*/
    /*
    std::ifstream fh_fastafs_crc(fastafs_file.c_str(), std::ios :: out | std::ios :: binary);
    fh_fastafs_crc.seekg(4, std::ios::beg);// skip magic number, this must be ok otherwise the toolkit won't use the file anyway

    uint32_t nnn = 0;
    uint32_t iii;

    uLong crc = crc32(0L, Z_NULL, 0);

    bool terminate = false;
    bool togo = true;
    while(togo)
    {
        if(!fh_fastafs_crc.read(buffer, 4)) {
            terminate = true;
        }
        //printf("alive [%i]\n", fh_fastafs_crc.gcount());
        iii = fh_fastafs_crc.gcount();
        crc = crc32(crc, (const Bytef*)& buffer, iii);
        nnn += iii;

        if(terminate) {
            togo = false;
        }
    };
    // --
    printf("nnn = %i\n",nnn);

    //write crc
    uint_to_fourbytes(byte_enc, (uint32_t) crc);
    printf("[%i][%i][%i][%i] input!! \n", byte_enc[0], byte_enc[1], byte_enc[2], byte_enc[3]);
    fh_fastafs.write(reinterpret_cast<char *>(&byte_enc), (size_t) 4);
    */

    // finalize file
    size_t written = fh_fastafs.tellp();
    fh_fastafs.close();




    return written;
}
