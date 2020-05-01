#include <iostream>
#include <fstream>

#include "config.hpp"

#include "fastafs.hpp"
#include "fasta_to_fastafs.hpp"
#include "flags.hpp"
#include "utils.hpp"




const static char na[2] = "A";
const static char nc[2] = "C";
const static char ng[2] = "G";
const static char nt[2] = "T";
const static char nu[2] = "U";

const static char nn[2] = "N";

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

const static char ne[2] = "E";
const static char nf[2] = "F";
const static char ni[2] = "I";
const static char nj[2] = "J";
const static char nl[2] = "L";
const static char no[2] = "O";
const static char np[2] = "P";
const static char nq[2] = "Q";
const static char nz[2] = "Z";
const static char nx[2] = "X";



size_t fasta_to_fastafs_seq::N_bytes_used()
{
    // just the number of n-blocks, not their actual size
    return (size_t)(4 + (this->n_block_starts.size() * (4 * 2)));
}

size_t fasta_to_fastafs_seq::twobit_bytes_used()
{
    //printf("n_actg: %i\n", n_actg);
    //printf("n_actg: %i + 3 = \n", n_actg, n_actg + 3);
    //printf("n_actg: (%i + 3) / 4 = %i\n", n_actg, (n_actg + 3) / 4);

    //return (size_t)((this->n_actg + (twobit_byte::nucleotides_per_byte - 1)) / twobit_byte::nucleotides_per_byte);
    return twobit_byte::nucleotides_to_compressed_offset(this->n_actg);

}




void fasta_to_fastafs_seq::twobit_add(unsigned char nucleotide, std::ofstream &fh_fastafs)
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




void fasta_to_fastafs_seq::finish_sequence(std::ofstream &fh_fastafs)
{
    if(this->current_dict == DICT_TWOBIT) {
        this->twobit_finish_sequence(fh_fastafs);
    } else if(this->current_dict == DICT_FOURBIT) {
        this->fourbit_finish_sequence(fh_fastafs);
    } else {
        this->fivebit_finish_sequence(fh_fastafs);
    }

    uint32_t j;

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



void fasta_to_fastafs_seq::twobit_finish_sequence(std::ofstream &fh_fastafs)
{
    // flush last nucleotide
    if(this->n_actg % 4 != 0) {
        for(uint32_t j = this->n_actg % 4; j < 4; j++) {
            this->twobit_data.set(twobit_byte::iterator_to_offset(j), 0);
        }
        fh_fastafs << this->twobit_data.data;
    }
}



void fasta_to_fastafs_seq::fourbit_add(unsigned char nucleotide, std::ofstream &fh_fastafs)
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


void fasta_to_fastafs_seq::fourbit_finish_sequence(std::ofstream &fh_fastafs)
{
    // flush last nucleotide
    if(this->n_actg % 2 != 0) {
        this->fourbit_data.set(fourbit_byte::iterator_to_offset(this->n_actg), 0);

        fh_fastafs << this->fourbit_data.data;
    }

}


void fasta_to_fastafs_seq::fivebit_add(unsigned char amino_acid, std::ofstream &fh_fastafs)
{
    unsigned char off = fivebit_fivebytes::iterator_to_offset(this->n_actg);
    this->fivebit_data.set(off, amino_acid);

    if(this->n_actg % 8 == 7) {
        //fh_fastafs << this->fivebit_data.data_compressed;
        fh_fastafs.write((const char*) this->fivebit_data.data_compressed, 5);
    }

    this->n_actg++;
}



void fasta_to_fastafs_seq::fivebit_finish_sequence(std::ofstream &fh_fastafs)
{
    unsigned char n_sticky = (unsigned char)(this->n_actg % 8);  // sticky end
    unsigned char n_sticky_compressed = fivebit_fivebytes::decompressed_to_compressed_bytes(n_sticky);

    // flush last nucleotide
    if(n_sticky_compressed > 0) {
        for(unsigned char i = n_sticky; i < 8 ; i++) {
            this->fivebit_data.set(i, 0);
        }

        printf(" writing only %i bytes at the end\n", n_sticky_compressed);
        fh_fastafs.write((const char*) this->fivebit_data.data_compressed, n_sticky_compressed);
    }
}




void fasta_to_fastafs_seq::add_unknown()
{
    if(!this->previous_was_N) {
        this->n_block_starts.push_back(this->n_actg + this->N);
    }

    this->previous_was_N = true;
    this->N++;
}





void fasta_to_fastafs_seq::flush()
{
    this->N = 0;
    this->n_actg = 0;

    this->n_block_starts.clear();
    this->n_block_ends.clear();

    this->m_block_starts.clear();
    this->m_block_ends.clear();
    this->in_m_block = false;
    this->previous_was_N = false;

    this->has_T = false;
    this->has_U = false;

    MD5_Init(&this->ctx);
}







size_t fasta_to_fastafs(const std::string &fasta_file, const std::string &fastafs_file, bool auto_recompress_to_fourbit)
{
    std::vector<fasta_to_fastafs_seq*> index;
    fasta_to_fastafs_seq* s;

    fastafs_flags ffsf;
    ffsf.set_incomplete();

    // @todo use ifstream and ofstream argument types
    std::string line;
    std::ifstream fh_fasta(fasta_file.c_str(), std::ios :: in | std::ios :: binary);
    std::ofstream fh_fastafs(fastafs_file.c_str(), std::ios :: out | std::ios :: binary);
    s = nullptr;
    if(!fh_fasta.is_open() or !fh_fastafs.is_open()) {
        throw std::invalid_argument("[fasta_to_fastafs:1] Could not open one of file: " + fasta_file + " | " + fastafs_file);
    } else {
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

                //s = new fasta_to_fastafs_seq( fh_fastafs.tellp(), line);
                s = new fasta_to_fastafs_seq(fh_fasta.tellg(), fh_fastafs.tellp(), line);
                fh_fastafs << "\x00\x00\x00\x00"s;// placeholder for sequence length
                index.push_back(s);
            }
        }

        if(s != nullptr) {
            bool running = getline(fh_fasta, line).good();
            while(running) {
                if(line[0] == '>') {
                    // more N-bytes than 2-bit bytes - 4bit is more efficient
                    if(auto_recompress_to_fourbit && s->current_dict == DICT_TWOBIT && s->N_bytes_used() > s->twobit_bytes_used()) {
                        fh_fasta.seekg(s->file_offset_in_fasta, std::ios::beg);
                        fh_fastafs.seekp(s->file_offset_in_fastafs + 4, std::ios::beg);// plus four, skipping the size

                        s->flush();
                        s->current_dict = DICT_FOURBIT;
                    } else {
                        s->finish_sequence(fh_fastafs);

                        line.erase(0, 1);// erases first part, quicker would be pointer from first char

                        //s = new fasta_to_fastafs_seq(fh_fastafs.tellp(), line);
                        s = new fasta_to_fastafs_seq(fh_fasta.tellg(), fh_fastafs.tellp(), line);
                        fh_fastafs << "\x00\x00\x00\x00"s;// number of 2bit encoded nucleotides, not yet known

                        index.push_back(s);
                    }
                } else {
                    if(s->current_dict == DICT_TWOBIT) {
                        for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
                            switch(*it) {

                            // keeping daling with upper-case and lower-case in separate cases is quicker than one if/else before the switch, simply beacuse switches are faster than if-statements.
                            case 'U':
                                if(s->has_T) {
                                    s->current_dict = DICT_FOURBIT;
                                }
                                s->has_U = true;

                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->twobit_add(NUCLEOTIDE_T, fh_fastafs);
                                MD5_Update(&s->ctx, nu, 1);// this needs to be pu in add_unknownucleotide
                                break;
                            case 'u':// lower case = m block
                                if(s->has_T) {
                                    s->current_dict = DICT_FOURBIT;
                                }
                                s->has_U = true;

                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->twobit_add(NUCLEOTIDE_T, fh_fastafs);
                                MD5_Update(&s->ctx, nu, 1);// this needs to be pu in add_unknownucleotide
                                break;
                            case 'T':
                                if(s->has_U) {
                                    s->current_dict = DICT_FOURBIT;
                                }
                                s->has_T = true;

                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->twobit_add(NUCLEOTIDE_T, fh_fastafs);
                                MD5_Update(&s->ctx, nt, 1);// this needs to be pu in add_unknownucleotide
                                break;
                            case 't':
                                if(s->has_U) {
                                    s->current_dict = DICT_FOURBIT;
                                }
                                s->has_T = true;

                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->twobit_add(NUCLEOTIDE_T, fh_fastafs);
                                MD5_Update(&s->ctx, nt, 1);// this needs to be pu in add_unknownucleotide
                                break;
                            case 'C':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->twobit_add(NUCLEOTIDE_C, fh_fastafs);
                                MD5_Update(&s->ctx, nc, 1);
                                break;
                            case 'c':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->twobit_add(NUCLEOTIDE_C, fh_fastafs);
                                MD5_Update(&s->ctx, nc, 1);
                                break;
                            case 'A':
                                if(s->in_m_block) {
                                    ;
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->twobit_add(NUCLEOTIDE_A, fh_fastafs);
                                MD5_Update(&s->ctx, na, 1);
                                break;
                            case 'a':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->twobit_add(NUCLEOTIDE_A, fh_fastafs);
                                MD5_Update(&s->ctx, na, 1);
                                break;
                            case 'G':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->twobit_add(NUCLEOTIDE_G, fh_fastafs);
                                MD5_Update(&s->ctx, ng, 1);
                                break;
                            case 'g':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->twobit_add(NUCLEOTIDE_G, fh_fastafs);
                                MD5_Update(&s->ctx, ng, 1);
                                break;
                            case 'N':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->add_unknown();
                                MD5_Update(&s->ctx, nn, 1);
                                break;
                            case 'n':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->add_unknown();
                                MD5_Update(&s->ctx, nn, 1);
                                break;
                            case 'r':
                            case 'R':
                            case 'y':
                            case 'Y':
                            case 'k':
                            case 'K':
                            case 'm':
                            case 'M':
                            case 's':
                            case 'S':
                            case 'w':
                            case 'W':
                            case 'b':
                            case 'B':
                            case 'd':
                            case 'D':
                            case 'h':
                            case 'H':
                            case 'v':
                            case 'V':
                            case '-':
                                s->current_dict = DICT_FOURBIT;
                                break;
                            default:
                                s->current_dict = DICT_FIVEBIT;
                                break;
                            }
                        }


                        if(s->current_dict != DICT_TWOBIT) {
                            char dict = s->current_dict; // DICT_FOURBIT | DICT_FIVEBIT

                            // set to fourbit and re-intialize
                            // seek fasta header to beg + s->file_offset_in_fasta
                            // seek fastafs back to beg + s->file_offset_in_fastafs and overwrite
                            //throw std::runtime_error("[fasta_to_fastafs] invalid chars in FASTA file");
                            fh_fasta.seekg(s->file_offset_in_fasta, std::ios::beg);
                            fh_fastafs.seekp(s->file_offset_in_fastafs + 4, std::ios::beg);// plus four, skipping the size

                            s->flush();
                            s->current_dict = dict; // set back to dict
                        }

                    } else if(s->current_dict == DICT_FOURBIT) {  // four bit decoding
                        for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
                            switch(*it) {

                            case 'A':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(0, fh_fastafs);
                                MD5_Update(&s->ctx, na, 1);
                                break;
                            case 'a':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(0, fh_fastafs);
                                MD5_Update(&s->ctx, na, 1);
                                break;
                            case 'C':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(1, fh_fastafs);
                                MD5_Update(&s->ctx, nc, 1);
                                break;
                            case 'c':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(1, fh_fastafs);
                                MD5_Update(&s->ctx, nc, 1);
                                break;
                            case 'G':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(2, fh_fastafs);
                                MD5_Update(&s->ctx, ng, 1);
                                break;
                            case 'g':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(2, fh_fastafs);
                                MD5_Update(&s->ctx, ng, 1);
                                break;
                            case 'T':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(3, fh_fastafs);
                                MD5_Update(&s->ctx, nt, 1);
                                break;
                            case 't':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(3, fh_fastafs);
                                MD5_Update(&s->ctx, nt, 1);
                                break;
                            case 'U':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(4, fh_fastafs);
                                MD5_Update(&s->ctx, nu, 1);
                                break;
                            case 'u':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(4, fh_fastafs);
                                MD5_Update(&s->ctx, nu, 1);
                                break;

                            case 'R':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(5, fh_fastafs);
                                MD5_Update(&s->ctx, nr, 1);
                                break;
                            case 'r':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(5, fh_fastafs);
                                MD5_Update(&s->ctx, nr, 1);
                                break;
                            case 'Y':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(6, fh_fastafs);
                                MD5_Update(&s->ctx, ny, 1);
                                break;
                            case 'y':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(6, fh_fastafs);
                                MD5_Update(&s->ctx, ny, 1);
                                break;
                            case 'K':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(7, fh_fastafs);
                                MD5_Update(&s->ctx, nk, 1);
                                break;
                            case 'k':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(7, fh_fastafs);
                                MD5_Update(&s->ctx, nk, 1);
                                break;
                            case 'M':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(8, fh_fastafs);
                                MD5_Update(&s->ctx, nm, 1);
                                break;
                            case 'm':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(8, fh_fastafs);
                                MD5_Update(&s->ctx, nm, 1);
                                break;
                            case 'S':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(9, fh_fastafs);
                                MD5_Update(&s->ctx, ns, 1);
                                break;
                            case 's':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(9, fh_fastafs);
                                MD5_Update(&s->ctx, ns, 1);
                                break;
                            case 'W':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(10, fh_fastafs);
                                MD5_Update(&s->ctx, nw, 1);
                                break;
                            case 'w':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(10, fh_fastafs);
                                MD5_Update(&s->ctx, nw, 1);
                                break;
                            case 'B':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(11, fh_fastafs);
                                MD5_Update(&s->ctx, nb, 1);
                                break;
                            case 'b':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(11, fh_fastafs);
                                MD5_Update(&s->ctx, nb, 1);
                                break;
                            case 'D':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(12, fh_fastafs);
                                MD5_Update(&s->ctx, nd, 1);
                                break;
                            case 'd':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(12, fh_fastafs);
                                MD5_Update(&s->ctx, nd, 1);
                                break;
                            case 'H':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(13, fh_fastafs);
                                MD5_Update(&s->ctx, nh, 1);
                                break;
                            case 'h':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(13, fh_fastafs);
                                MD5_Update(&s->ctx, nh, 1);
                                break;
                            case 'V':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(14, fh_fastafs);
                                MD5_Update(&s->ctx, nv, 1);
                                break;
                            case 'v':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(14, fh_fastafs);
                                MD5_Update(&s->ctx, nv, 1);
                                break;
                            case 'N':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fourbit_add(15, fh_fastafs);
                                MD5_Update(&s->ctx, nn, 1);
                                break;
                            case 'n':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fourbit_add(15, fh_fastafs);
                                MD5_Update(&s->ctx, nn, 1);
                                break;
                            case '-':
                                s->add_unknown();
                                break;

                            // @todo case for those only in protein seq

                            default:
                                throw std::runtime_error("[fasta_to_x_fastafs] invalid chars in FASTA file");
                                break;
                            }
                        }
                    } else { // s->current_dict == DICT_FIVEBIT
                        for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
                            switch(*it) {

                            //ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                            case 'A':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(0, fh_fastafs);
                                MD5_Update(&s->ctx, na, 1);
                                break;
                            case 'a':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(0, fh_fastafs);
                                MD5_Update(&s->ctx, na, 1);
                                break;
                            case 'B':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(1, fh_fastafs);
                                MD5_Update(&s->ctx, nb, 1);
                                break;
                            case 'b':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(1, fh_fastafs);
                                MD5_Update(&s->ctx, nb, 1);
                                break;
                            case 'C':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(2, fh_fastafs);
                                MD5_Update(&s->ctx, nc, 1);
                                break;
                            case 'c':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(2, fh_fastafs);
                                MD5_Update(&s->ctx, nc, 1);
                                break;
                            case 'D':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(3, fh_fastafs);
                                MD5_Update(&s->ctx, nd, 1);
                                break;
                            case 'd':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(3, fh_fastafs);
                                MD5_Update(&s->ctx, nd, 1);
                                break;
                            case 'E':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(4, fh_fastafs);
                                MD5_Update(&s->ctx, ne, 1);
                                break;
                            case 'e':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(4, fh_fastafs);
                                MD5_Update(&s->ctx, ne, 1);
                                break;
                            case 'F':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(5, fh_fastafs);
                                MD5_Update(&s->ctx, nf, 1);
                                break;
                            case 'f':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(5, fh_fastafs);
                                MD5_Update(&s->ctx, nf, 1);
                                break;
                            case 'G'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(6, fh_fastafs);
                                MD5_Update(&s->ctx, ng, 1);
                                break;
                            case 'g':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(6, fh_fastafs);
                                MD5_Update(&s->ctx, ng, 1);
                                break;
                            case 'H'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(7, fh_fastafs);
                                MD5_Update(&s->ctx, nh, 1);
                                break;
                            case 'h':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(7, fh_fastafs);
                                MD5_Update(&s->ctx, nh, 1);
                                break;
                            case 'I'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(8, fh_fastafs);
                                MD5_Update(&s->ctx, ni, 1);
                                break;
                            case 'i':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(8, fh_fastafs);
                                MD5_Update(&s->ctx, ni, 1);
                                break;
                            case 'J'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(9, fh_fastafs);
                                MD5_Update(&s->ctx, nj, 1);
                                break;
                            case 'j':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(9, fh_fastafs);
                                MD5_Update(&s->ctx, nj, 1);
                                break;
                            case 'K'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(10, fh_fastafs);
                                MD5_Update(&s->ctx, nk, 1);
                                break;
                            case 'k':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(10, fh_fastafs);
                                MD5_Update(&s->ctx, nk, 1);
                                break;
                            case 'L'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(11, fh_fastafs);
                                MD5_Update(&s->ctx, nl, 1);
                                break;
                            case 'l':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(11, fh_fastafs);
                                MD5_Update(&s->ctx, nl, 1);
                                break;
                            case 'M'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(12, fh_fastafs);
                                MD5_Update(&s->ctx, nm, 1);
                                break;
                            case 'm':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(12, fh_fastafs);
                                MD5_Update(&s->ctx, nm, 1);
                                break;
                            case 'N'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(13, fh_fastafs);
                                MD5_Update(&s->ctx, nn, 1);
                                break;
                            case 'n':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(13, fh_fastafs);
                                MD5_Update(&s->ctx, nn, 1);
                                break;
                            case 'O'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(14, fh_fastafs);
                                MD5_Update(&s->ctx, no, 1);
                                break;
                            case 'o':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(14, fh_fastafs);
                                MD5_Update(&s->ctx, no, 1);
                                break;
                            case 'P'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(15, fh_fastafs);
                                MD5_Update(&s->ctx, np, 1);
                                break;
                            case 'p':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(15, fh_fastafs);
                                MD5_Update(&s->ctx, np, 1);
                                break;
                            case 'Q'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(16, fh_fastafs);
                                MD5_Update(&s->ctx, nq, 1);
                                break;
                            case 'q':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(16, fh_fastafs);
                                MD5_Update(&s->ctx, nq, 1);
                                break;
                            case 'R'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(17, fh_fastafs);
                                MD5_Update(&s->ctx, nr, 1);
                                break;
                            case 'r':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(17, fh_fastafs);
                                MD5_Update(&s->ctx, nr, 1);
                                break;
                            case 'S'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(18, fh_fastafs);
                                MD5_Update(&s->ctx, ns, 1);
                                break;
                            case 's':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(18, fh_fastafs);
                                MD5_Update(&s->ctx, ns, 1);
                                break;
                            case 'T'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(19, fh_fastafs);
                                MD5_Update(&s->ctx, nt, 1);
                                break;
                            case 't':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(19, fh_fastafs);
                                MD5_Update(&s->ctx, nt, 1);
                                break;
                            case 'U':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(20, fh_fastafs);
                                MD5_Update(&s->ctx, nu, 1);
                                break;
                            case 'u':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(20, fh_fastafs);
                                MD5_Update(&s->ctx, nu, 1);
                                break;
                            case 'V':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(21, fh_fastafs);
                                MD5_Update(&s->ctx, nv, 1);
                                break;
                            case 'v':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(21, fh_fastafs);
                                MD5_Update(&s->ctx, nv, 1);
                                break;

                            case 'W'://ABCDEFGHIJKLMNOPQRSTUVWYZX*-
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(22, fh_fastafs);
                                MD5_Update(&s->ctx, nw, 1);
                                break;
                            case 'w':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(22, fh_fastafs);
                                MD5_Update(&s->ctx, nw, 1);
                                break;

                            case 'Y':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(23, fh_fastafs);
                                MD5_Update(&s->ctx, ny, 1);
                                break;
                            case 'y':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(23, fh_fastafs);
                                MD5_Update(&s->ctx, ny, 1);
                                break;
                            case 'Z':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(24, fh_fastafs);
                                MD5_Update(&s->ctx, nz, 1);
                                break;
                            case 'z':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(24, fh_fastafs);
                                MD5_Update(&s->ctx, nz, 1);
                                break;
                            case 'X':
                                if(s->in_m_block) {
                                    s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                    s->in_m_block = false;
                                }

                                s->fivebit_add(25, fh_fastafs);
                                MD5_Update(&s->ctx, nx, 1);
                                break;
                            case 'x':
                                if(!s->in_m_block) {
                                    s->m_block_starts.push_back(s->N + s->n_actg);
                                    s->in_m_block = true;
                                }

                                s->fivebit_add(25, fh_fastafs);
                                MD5_Update(&s->ctx, nx, 1);
                                break;

                            case '*':
                                s->fivebit_add(26, fh_fastafs);
                                break;
                            case '-':
                                s->fivebit_add(27, fh_fastafs);
                                break;


                            // @todo case for those only in protein seq

                            default:
                                throw std::runtime_error("[fasta_to_x_fastafs] invalid chars in FASTA file");
                                break;
                            }


                        }
                    }
                }

                running = getline(fh_fasta, line).good();

                // if not running, recheck
                if(!running) {
                    if(auto_recompress_to_fourbit && s->current_dict == DICT_TWOBIT && s->N_bytes_used() > s->twobit_bytes_used()) {
                        fh_fasta.clear();// get it out of EOF state

                        fh_fasta.seekg(s->file_offset_in_fasta, std::ios::beg);
                        fh_fastafs.seekp(s->file_offset_in_fastafs + 4, std::ios::beg);// plus four, skipping the size

                        s->flush();
                        s->current_dict = DICT_FOURBIT;

                        //after re-opening file and setting the file pointer, read line again
                        running = getline(fh_fasta, line).good();
                    } else {
                        s->finish_sequence(fh_fastafs);
                    }
                }
            }
        }
        fh_fasta.close();
    }


    // write index/footer
    unsigned int index_file_position = (uint32_t) fh_fastafs.tellp();
    char buffer[4 + 1];
    uint_to_fourbytes(buffer, (uint32_t) index.size());
    fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);

    for(size_t i = 0; i < index.size(); i++) {
        s = index[i];

        // set and write flag
        fastafs_sequence_flags fsf;
        fsf.set_linear();

        if(s->current_dict == DICT_TWOBIT) {
            if(s->has_U) {
                fsf.set_rna();
            } else {
                fsf.set_dna();
            }
        } else if(s->current_dict == DICT_FOURBIT) {
            fsf.set_iupec_nucleotide();
        } else {
            fsf.set_protein(); // set protein
        }

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
    fh_fasta.close();

    // write metadata
    fh_fastafs << "\x00"s;// no metadata tags (YET)
    size_t written = fh_fastafs.tellp();

    // update header and set to complete: set to updated
    fh_fastafs.seekp(8, std::ios::beg);
    ffsf.set_complete();
    fh_fastafs << ffsf.get_bits()[0];
    fh_fastafs << ffsf.get_bits()[1];
    uint_to_fourbytes(buffer, index_file_position);//position of header
    fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);

    // close fastafs, calc crc32, re-open and save
    fh_fastafs.close();
    uint32_t crc32c = file_crc32(fastafs_file, 4, written);
    char byte_enc[5] = "\x00\x00\x00\x00";
    uint_to_fourbytes(byte_enc, (uint32_t) crc32c);
    std::ofstream fh_fastafs2(fastafs_file.c_str(), std::ios::out | std::ios::binary | std::ios::app);
    if(fh_fastafs2.is_open()) {
        fh_fastafs2.seekp(written, std::ios::beg); // don't blindly because of possible race conditions?
        fh_fastafs2.write(reinterpret_cast<char *>(&byte_enc), (size_t) 4);
        fh_fastafs2.flush();

        written = fh_fastafs2.tellp();
        fh_fastafs2.close();
    } else {
        throw std::invalid_argument("[fasta_to_fastafs:2] Could not open file: " + fastafs_file);
    }

    return written;
}
