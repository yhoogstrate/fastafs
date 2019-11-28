#include <iostream>
#include <fstream>

#include "config.hpp"

#include "fasta_to_fourbit_fastafs.hpp"
#include "utils.hpp"



const static char na[2] = "A";
const static char nc[2] = "C";
const static char ng[2] = "G";
const static char nt[2] = "T";

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

const static char nn[2] = "N";

//const char fourbit_byte::fourbit_alhpabet[17] = "ACGTURYKMSWBDHVN";



void fasta_seq_header_fourbit_conversion_data::add_ACTG(unsigned char nucleotide, std::ofstream &fh_fastafs)
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

void fasta_seq_header_fourbit_conversion_data::add_N()
{
    if(!this->previous_was_N) {
        this->n_block_starts.push_back(this->n_actg + this->N);
    }

    this->previous_was_N = true;
    this->N++;
}



void fasta_seq_header_fourbit_conversion_data::finish_sequence(std::ofstream &fh_fastafs)
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



size_t fasta_to_fourbit_fastafs(const std::string fasta_file, const std::string fastafs_file)
{
    std::vector<fasta_seq_header_fourbit_conversion_data*> index;
    fasta_seq_header_fourbit_conversion_data* s;

    // @todo use ifstream and ofstream argument types
    std::string line;
    std::ifstream fh_fasta(fasta_file.c_str(), std::ios :: in | std::ios :: binary);
    std::ofstream fh_fastafs(fastafs_file.c_str(), std::ios :: out | std::ios :: binary);
	s = nullptr;
    if(fh_fasta.is_open() and fh_fastafs.is_open()) {
        fh_fastafs << FASTAFS_MAGIC;
        fh_fastafs << FASTAFS_VERSION;
        fh_fastafs << "\x00\x00"s;// the flag for now, set to INCOMPLETE as writing is in progress
        fh_fastafs << "\x00\x00\x00\x00"s;// position of metedata ~ unknown YET

        // iterate until first sequence is found, ensuring we won't write to uninitialized sequences
        while(s == nullptr and getline(fh_fasta, line)) {
            if(line[0] == '>') {
				
				// init new sequence
                line.erase(0, 1);// erases first part, quicker would be pointer from first char
                s = new fasta_seq_header_fourbit_conversion_data(fh_fastafs.tellp(), line);
                fh_fastafs << "\x00\x00\x00\x00"s;// placeholder for sequence length
                index.push_back(s);
            }
        }

        if(s != nullptr) {
            while(getline(fh_fasta, line)) {
                if(line[0] == '>') {
                    s->finish_sequence(fh_fastafs);
                    
                    // init sequence
                    line.erase(0, 1);// erases first part, quicker would be pointer from first char
                    s = new fasta_seq_header_fourbit_conversion_data(fh_fastafs.tellp(), line);
                    fh_fastafs << "\x00\x00\x00\x00"s;// number of 2bit encoded nucleotides, not yet known
                    index.push_back(s);
                } else {
                    for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
                        switch(*it) {

                        case 'A':
                            if(s->in_m_block) {
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(0, fh_fastafs);
                            MD5_Update(&s->ctx, na, 1);
                            break;
                        case 'a':
                            if(!s->in_m_block) {
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(0, fh_fastafs);
                            MD5_Update(&s->ctx, na, 1);
                            break;
                        case 'C':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(1, fh_fastafs);
                            MD5_Update(&s->ctx, na, 1);
                            break;
                        case 'c':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(1, fh_fastafs);
                            MD5_Update(&s->ctx, na, 1);
                            break;
                        case 'G':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(2, fh_fastafs);
                            MD5_Update(&s->ctx, ng, 1);
                            break;
                        case 'g':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(2, fh_fastafs);
                            MD5_Update(&s->ctx, ng, 1);
                            break;
                        case 'T':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(3, fh_fastafs);
                            MD5_Update(&s->ctx, nt, 1);
                            break;
                        case 't':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(3, fh_fastafs);
                            MD5_Update(&s->ctx, nt, 1);
                            break;
                        case 'U':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(4, fh_fastafs);
                            MD5_Update(&s->ctx, nu, 1);
                            break;
                        case 'u':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(4, fh_fastafs);
                            MD5_Update(&s->ctx, nu, 1);
                            break;

// ====================================================

                        case 'R':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(5, fh_fastafs);
                            MD5_Update(&s->ctx, nr, 1);
                            break;
                        case 'r':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(5, fh_fastafs);
                            MD5_Update(&s->ctx, nr, 1);
                            break;
                        case 'Y':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(6, fh_fastafs);
                            MD5_Update(&s->ctx, ny, 1);
                            break;
                        case 'y':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(6, fh_fastafs);
                            MD5_Update(&s->ctx, ny, 1);
                            break;
                        case 'K':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(7, fh_fastafs);
                            MD5_Update(&s->ctx, nk, 1);
                            break;
                        case 'k':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(7, fh_fastafs);
                            MD5_Update(&s->ctx, nk, 1);
                            break;
                        case 'M':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(8, fh_fastafs);
                            MD5_Update(&s->ctx, nm, 1);
                            break;
                        case 'm':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(8, fh_fastafs);
                            MD5_Update(&s->ctx, nm, 1);
                            break;
                        case 'S':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(9, fh_fastafs);
                            MD5_Update(&s->ctx, ns, 1);
                            break;
                        case 's':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(9, fh_fastafs);
                            MD5_Update(&s->ctx, ns, 1);
                            break;
                        case 'W':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(10, fh_fastafs);
                            MD5_Update(&s->ctx, nw, 1);
                            break;
                        case 'w':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(10, fh_fastafs);
                            MD5_Update(&s->ctx, nw, 1);
                            break;
                        case 'B':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(11, fh_fastafs);
                            MD5_Update(&s->ctx, nb, 1);
                            break;
                        case 'b':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(11, fh_fastafs);
                            MD5_Update(&s->ctx, nb, 1);
                            break;
                        case 'D':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(12, fh_fastafs);
                            MD5_Update(&s->ctx, nd, 1);
                            break;
                        case 'd':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(12, fh_fastafs);
                            MD5_Update(&s->ctx, nd, 1);
                            break;
                        case 'H':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(13, fh_fastafs);
                            MD5_Update(&s->ctx, nh, 1);
                            break;
                        case 'h':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(13, fh_fastafs);
                            MD5_Update(&s->ctx, nh, 1);
                            break;
                        case 'V':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(14, fh_fastafs);
                            MD5_Update(&s->ctx, nv, 1);
                            break;
                        case 'v':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(14, fh_fastafs);
                            MD5_Update(&s->ctx, nv, 1);
                            break;
                        case 'N':
                            if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }

                            s->add_ACTG(15, fh_fastafs);
                            MD5_Update(&s->ctx, nn, 1);
                            break;
                        case 'n':
                            if(!s->in_m_block) {
                                //printf("starting M block: %d\n", s->N + s->n_actg);
                                s->m_block_starts.push_back(s->N + s->n_actg);
                                s->in_m_block = true;
                            }

                            s->add_ACTG(15, fh_fastafs);
                            MD5_Update(&s->ctx, nn, 1);
                            break;
                        case '-':
                            /*if(s->in_m_block) {
                                //printf("ending M block: %d\n", s->N + s->n_actg - 1);
                                s->m_block_ends.push_back(s->N + s->n_actg - 1);
                                s->in_m_block = false;
                            }*/

                            s->add_N();
                            //MD5_Update(&s->ctx, nn, 1);
                            break;

                        default:
                            std::cerr << "invalid chars in FASTA file" << std::endl;
                            exit(1);
                            break;
                        }
                    }
                }
            }
        }
        fh_fasta.close();
    }
    if(s != nullptr) {
        s->finish_sequence(fh_fastafs);// finish last sequence
    }

    // write index/footer
    unsigned int index_file_position = (uint32_t) fh_fastafs.tellp();
    char buffer[4 +  1];
    uint_to_fourbytes(buffer, (uint32_t) index.size());
    fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);

    for(size_t i = 0; i < index.size(); i++) {
        s = index[i];

        // flag
        fh_fastafs << "\x00\x0A"s;// 00001010 (IUPEC + completed-with-checksum) | this probably has to be mirrored as last and first bit are swapped

        // name
        unsigned char name_size = (unsigned char) s->name.size();
        fh_fastafs.write((char *) &name_size, 1); // name size
        fh_fastafs.write(s->name.c_str(), (size_t) s->name.size());// name

        // location of sequence data in file
        uint_to_fourbytes(buffer, (uint32_t) s->file_offset_in_fasta);
        fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
        delete s;
    }
    fh_fastafs << "\x00"s;// no metadata tags (YET)

    // update header: set to updated
    fh_fastafs.seekp(8, std::ios::beg);
    fh_fastafs << "\x00\x01"s; // updated flag

    uint_to_fourbytes(buffer, index_file_position);//position of header
    fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);

    // calc written size
    fh_fastafs.seekp(0, std::ios::end);
    size_t written = fh_fastafs.tellp();

    fh_fasta.close();
    fh_fastafs.close();

    return written;
}
