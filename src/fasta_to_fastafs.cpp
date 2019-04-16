#include <iostream>
#include <fstream>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "utils.hpp"





fasta_to_fastafs_seq::fasta_to_fastafs_seq(void) :
    fastafs_seq(),
    previous_was_N(false),
    N(0)
{
    SHA1_Init(&this->ctx);
}




void fasta_to_fastafs_seq::add_N()
{
    if(!this->previous_was_N) {
        this->n_starts.push_back(this->n);
    }

    this->previous_was_N = true;
    this->n++;
    this->N++;
}



void fasta_to_fastafs_seq::add_nucleotide(unsigned char nucleotide)
{
    /*
    // 9,123,866,399  ???:fasta_to_fastafs_seq::add_nucleotide(unsigned char) [/home/youri/.local/bin/fastafs]
    switch((this->n - this->N) % 4) {
    	case 0:
    		this->twobit_data.set(6, nucleotide);
    		break;
    	case 1:
    		this->twobit_data.set(4, nucleotide);
    		break;
    	case 2:
    		this->twobit_data.set(2, nucleotide);
    		break;
    	case 3:
    		this->twobit_data.set(0, nucleotide);
    		this->twobits.push_back(this->twobit_data.data);

    		break;
    #if DEBUG
    	default:
    		throw std::invalid_argument("add_nucleotide needs to be 1, 2 or 3\n");
    		break;
    #endif //DEBUG
    }
    */

    uint32_t offset = 6 - (2 * ((this->n - this->N) % 4) ) ;

    this->twobit_data.set( (unsigned char) offset, nucleotide);
    if(offset == 0) {
        this->twobits.push_back(this->twobit_data.data);
    }

    if(this->previous_was_N) {
        this->n_ends.push_back(this->n - 1);
    }

    this->previous_was_N = false;
    this->n++;
}


/**
 * @brief MAY ONLY BE USED TO INSERT FULL TWOBIT BYTES REPRESENTING FOUR CHARS!
 *  otherwise use add_twobit_sticky_end(twobit_byte& tb, i < 4);
 */
void fasta_to_fastafs_seq::add_twobit(twobit_byte &tb)
{
    this->twobits.push_back(tb.data);
    this->n++;
}


void fasta_to_fastafs_seq::close_reading()
{
    unsigned char sticky_end = (this->n - this->N) % 4;
    if(sticky_end != 0) { // sticky end
        signed int i;
        // if i = 1, last three 2bits need to be set to 00
        // 3 - i = 2,
        for(i = 3 - (signed int) sticky_end; i >= 0; i--) {
            this->twobit_data.set((unsigned char) (i * 2), 0);
        }

        // insert sticky-ended twobit
        this->twobits.push_back(this->twobit_data.data);
    }

    if(this->previous_was_N) {
        this->n_ends.push_back(this->n - 1);
    }

    SHA1_Final(this->sha1_digest, &this->ctx);
    char sha1_hash[41] = "";
    sha1_digest_to_hash(this->sha1_digest, sha1_hash);
    //delete this->twobit_data;
    //this->twobit_data = nullptr;
}



size_t fasta_to_fastafs_seq::size(void)
{
    return this->twobits.size();
}






// is resource by filename required?
// not yet, maybe other classes or empty?
fasta_to_fastafs::fasta_to_fastafs(std::string name, std::string fname)
{
    this->name = name;
    this->filename = fname;
}


fasta_to_fastafs::~fasta_to_fastafs()
{
    for(uint32_t i = 0; i < this->data.size(); i++) {
        delete this->data[i];
    }
}

int fasta_to_fastafs::cache(void)
{
    fasta_to_fastafs_seq *s = nullptr;

    std::string line;
    std::ifstream myfile (this->filename.c_str());
    std::string sequence = "";

    static char nt[2] = "T";
    static char nc[2] = "C";
    static char na[2] = "A";
    static char ng[2] = "G";
    static char nn[2] = "N";

    if (myfile.is_open()) {
        while(getline(myfile, line)) {
            if (line[0] == '>') {
                if(s != nullptr) {
                    s->close_reading();

                    this->data.push_back(s);
                    s = nullptr;
                }

                line.erase(0, 1);// erases first part, quicker would be pointer from first char
                s = new fasta_to_fastafs_seq();
                s->name = line;
            } else {

                for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
                    switch(*it) {
                    case 't':
                    case 'T':
                    case 'u':
                    case 'U':
                        s->add_nucleotide(NUCLEOTIDE_T);
                        SHA1_Update(&s->ctx, nt, 1);// this needs to be pu in add_nucleotide
                        break;
                    case 'c':
                    case 'C':
                        s->add_nucleotide(NUCLEOTIDE_C);
                        SHA1_Update(&s->ctx, nc, 1);
                        break;
                    case 'a':
                    case 'A':
                        s->add_nucleotide(NUCLEOTIDE_A);
                        SHA1_Update(&s->ctx, na, 1);
                        break;
                    case 'g':
                    case 'G':
                        s->add_nucleotide(NUCLEOTIDE_G);
                        SHA1_Update(&s->ctx, ng, 1);
                        break;
                    case 'n':
                    case 'N':
                        s->add_N();
                        SHA1_Update(&s->ctx, nn, 1);
                        break;
                    default:
                        std::cerr << "invalid chars in FASTA file" << std::endl;
                        exit(1);
                        break;
                    }
                }
            }
        }
        myfile.close();
    } else {
        throw std::runtime_error("could not cache fasta file: " + this->filename);
    }

    if(s != nullptr) {
        s->close_reading();

        this->data.push_back(s);
        s = nullptr;
    }
    return 0;
}



uint32_t fasta_to_fastafs::get_index_size()
{
    // each sequence header has:
    // 1  byte  for length of name
    // 20 bytes for SHA1 sum
    // 4  bytes for file-offset
    uint32_t n = (uint32_t) this->data.size() * (1 + 20 + 4);// one byte to describe each size + 32bits to describe offset

    for(uint32_t i = 0; i < this->data.size(); i++) {
        n += (uint32_t) this->data[i]->name.size();
    }

    return n;
}




// closely similar to: http://genome.ucsc.edu/FAQ/FAQformat.html#format7
void fasta_to_fastafs::write(std::string filename)
{
    std::ofstream fh_fastafs(filename.c_str(), std::ios :: out | std::ios :: binary);
    if(fh_fastafs.is_open()) {
        fh_fastafs << FASTAFS_MAGIC;
        fh_fastafs << FASTAFS_VERSION;

        fh_fastafs << "\x00\x00"s;// the flag for now, set to INCOMPLETE as writing is in progress
        fh_fastafs << "\x00\x00\x00\x00"s;// position of metedata ~ unknown YET

        std::vector<uint32_t> sequence_data_positions(this->data.size());
        uint32_t four_bytes;
        char buffer[4];
        unsigned char byte;
        // write data
        for(uint32_t i = 0; i < this->data.size(); i++) {
            sequence_data_positions[i] = (uint32_t) fh_fastafs.tellp();

            unsigned int n_without_N = this->data[i]->n;

            //s->n - s->N (total number of ACTG's in 2bit compressed bytes
            fh_fastafs << "\x00\x00\x00\x00"s;// unknown before writing everything

            // @todo this->data[i].write_twobit_data(fstream);
            for(uint32_t j = 0; j < this->data[i]->twobits.size(); ++j) {
                fh_fastafs.write((char *) &this->data[i]->twobits[j], (size_t) 1);
            }

            //s-N
            uint_to_fourbytes(buffer, (uint32_t )this->data[i]->n_starts.size());
            fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);

            //s->n_starts
            for(uint32_t j = 0; j < this->data[i]->n_starts.size(); ++j) {
                uint_to_fourbytes(buffer, this->data[i]->n_starts[j]);
                fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
            }

            //s->n_ends
            for(uint32_t j = 0; j < this->data[i]->n_ends.size(); ++j) {
                uint_to_fourbytes(buffer, this->data[i]->n_ends[j]);
                fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);

                n_without_N -= this->data[i]->n_ends[j] - this->data[i]->n_starts[j] + 1;
            }

            // update length as it is now known
            unsigned int curpos = (uint32_t) fh_fastafs.tellp();
            fh_fastafs.seekp(sequence_data_positions[i], std::ios::beg);
            uint_to_fourbytes(buffer, n_without_N);
            fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
            fh_fastafs.seekp(curpos, std::ios::beg);

            //if(this->complete)
            fh_fastafs.write(reinterpret_cast<char *> (&this->data[i]->sha1_digest), (size_t) 20);

            //s->M
            four_bytes = (uint32_t) 0;
            fh_fastafs.write( reinterpret_cast<char *>(&four_bytes), 4 );

        }

        // save startposition of index
        unsigned int index_file_position = (uint32_t) fh_fastafs.tellp();

        // write down tellg
        uint_to_fourbytes(buffer, (uint32_t) this->data.size());
        fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);

        // write indices
        for(uint32_t i = 0; i < this->data.size(); i++) {
            fh_fastafs << "\x00\x08"s;

            byte = (unsigned char) this->data[i]->name.size();
            fh_fastafs.write((char *) &byte, (size_t) 1);

            for(uint32_t j = 0; j < this->data[i]->name.size(); j++) {
                byte = (unsigned char) this->data[i]->name[j];
                fh_fastafs.write((char *) &byte, (size_t) 1);
            }

            uint_to_fourbytes(buffer, sequence_data_positions[i]);
            fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
        }
        
        // write metadata:
        fh_fastafs << "\x00"s; // no metadata tags (YET)

        // update header: set to updated
        fh_fastafs.seekp(8, std::ios::beg);

        fh_fastafs << "\x00\x01"s; // updated flag
        uint_to_fourbytes(buffer, index_file_position);//position of header
        fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);


        fh_fastafs.close();
    } else {
        throw std::runtime_error("Could not write to file: " + filename);
    }
}







size_t f2fs(const std::string fasta_file, const std::string fastafs_file) {
    size_t written = 0;
    
    std::vector<fasta_seq_header_conversion_data*> index;
    fasta_seq_header_conversion_data* s;

    
    // @todo use ifstream and ofstream argument types
    std::string line;
    std::ifstream fh_fasta(fasta_file.c_str(), std::ios :: in | std::ios :: binary);
    std::ofstream fh_fastafs(fastafs_file.c_str(), std::ios :: out | std::ios :: binary);
    if(fh_fasta.is_open() and fh_fastafs.is_open()) {
        fh_fastafs << FASTAFS_MAGIC;
        fh_fastafs << FASTAFS_VERSION;

        fh_fastafs << "\x00\x00"s;// the flag for now, set to INCOMPLETE as writing is in progress
        fh_fastafs << "\x00\x00\x00\x00"s;// position of metedata ~ unknown YET

        // iterate until first sequence is found, ensuring we won't write to uninitialized sequences
        s = nullptr;
        while(s == nullptr and getline(fh_fasta, line)) {
            if (line[0] == '>') {
                
                line.erase(0, 1);// erases first part, quicker would be pointer from first char
                s = new fasta_seq_header_conversion_data(3, line);
                
                index.push_back(s);
            }
        }

        while(getline(fh_fasta, line)) {
            if(line[0] == '>') {
                printf(" another sequence ! \n");

                line.erase(0, 1);// erases first part, quicker would be pointer from first char
                s = new fasta_seq_header_conversion_data(fh_fastafs.tellp(), line);
                
                index.push_back(s);
            } else {
                fh_fastafs << "1234";
                /*for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
                    switch(*it) {
                    case 't':
                    case 'T':
                    case 'u':
                    case 'U':
                        s->add_nucleotide(NUCLEOTIDE_T);
                        SHA1_Update(&s->ctx, nt, 1);// this needs to be pu in add_nucleotide
                        break;
                    case 'c':
                    case 'C':
                        s->add_nucleotide(NUCLEOTIDE_C);
                        SHA1_Update(&s->ctx, nc, 1);
                        break;
                    case 'a':
                    case 'A':
                        s->add_nucleotide(NUCLEOTIDE_A);
                        SHA1_Update(&s->ctx, na, 1);
                        break;
                    case 'g':
                    case 'G':
                        s->add_nucleotide(NUCLEOTIDE_G);
                        SHA1_Update(&s->ctx, ng, 1);
                        break;
                    case 'n':
                    case 'N':
                        s->add_N();
                        SHA1_Update(&s->ctx, nn, 1);
                        break;
                    default:
                        std::cerr << "invalid chars in FASTA file" << std::endl;
                        exit(1);
                        break;
                    }
                }*/
            }
        }
        
        fh_fasta.close();
    }

    // doing index
    for(size_t i = 0; i < index.size(); i++) {
        printf("seq:         %s \n",index[i]->name.c_str());
        printf("pos-in-file: %d \n",index[i]->file_offset_in_fasta);
        
        printf("\n");
    }

    return written;
}
