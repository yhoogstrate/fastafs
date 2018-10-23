#include <iostream>
#include <fstream>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"







fasta_to_fastafs_seq::fasta_to_fastafs_seq(void) :
    fastafs_seq(),
    previous_was_N(false),
    N(0)
{
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

    unsigned int offset = 6 - (2 * ((this->n - this->N) % 4) ) ;

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

    //delete this->twobit_data;
    //this->twobit_data = nullptr;
}




/*
void fasta_to_fastafs_seq::print(void)
{
    if(this->n_starts.size() != this->n_ends.size()) {
        throw std::invalid_argument("unequal number of start and end positions for N regions\n");
    }

    bool in_N = false;
    twobit_byte t = twobit_byte();
    unsigned int i;

    printf(">%s (size=%i [ACTG: %i, N: %i], compressed ACTG=%i)\n", this->name.c_str(), this->n, this->n - this->N, this->N, (unsigned int) this->size());

    printf("\nN[s]: ");
    for(i = 0; i < this->n_starts.size(); i++) {
        printf("%i\t", this->n_starts[i]);
    }
    printf("\nN[e]: ");
    for(i = 0; i < this->n_ends.size(); i++) {
        printf("%i\t", this->n_ends[i]);
    }
    printf("\n----------------------\n");

    unsigned int i_n_start = 0;//@todo make iterator
    unsigned int i_n_end = 0;//@todo make iterator
    unsigned int i_in_seq = 0;
    unsigned int chunk_offset;
    const char *chunk;

    for(i = 0; i < this->n; i++) {
        if(i % 4 == 0 and i != 0) {
            printf("\n");
        }
        printf("%i\t", i);



        if(this->n_starts.size() > i_n_start and i == this->n_starts[i_n_start]) {
            in_N = true;
        }

        if(in_N) {
            printf("N\n");

            if(i == this->n_ends[i_n_end]) {
                i_n_end++;
                in_N = false;
            }
        } else {
            // load new twobit chunk when needed
            chunk_offset = i_in_seq % 4;
            if(chunk_offset == 0) {
                t.data = this->twobits[i_in_seq / 4];
                chunk = t.get();
            }
            printf("%c\n", chunk[chunk_offset]);

            i_in_seq++;
        }
    }
    printf("\n\n");
}
*/

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
    for(unsigned int i = 0; i < this->data.size(); i++) {
        delete this->data[i];
    }
}

int fasta_to_fastafs::cache(void)
{
    fasta_to_fastafs_seq *s = nullptr;

    std::string line;
    std::ifstream myfile (this->filename.c_str());
    std::string sequence = "";

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
                        break;
                    case 'c':
                    case 'C':
                        s->add_nucleotide(NUCLEOTIDE_C);
                        break;
                    case 'a':
                    case 'A':
                        s->add_nucleotide(NUCLEOTIDE_A);
                        break;
                    case 'g':
                    case 'G':
                        s->add_nucleotide(NUCLEOTIDE_G);
                        break;
                    case 'n':
                    case 'N':
                        s->add_N();
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


/*
void fasta_to_fastafs::print(void)
{
    for(unsigned int i = 0; i < this->data.size(); i++) {
        this->data[i]->print();
    }
}
*/

unsigned int fasta_to_fastafs::get_index_size()
{
    unsigned int n = (unsigned int) this->data.size() * 5;// one byte to describe each size + 32bits to describe offset

    for(unsigned int i = 0; i < this->data.size(); i++) {
        n += (unsigned int) this->data[i]->name.size();
    }

    return n;
}

unsigned int fasta_to_fastafs::get_sequence_offset(unsigned int sequence)
{
    unsigned int n = 4 + 4 + 4 + 4 + this->get_index_size();

    for(unsigned int i = 0; i < sequence; i++) {
        n += 4; // dna_size
        n += 4; // n_block_count
        n += (unsigned int) this->data[i]->n_starts.size() * 4 * 2;//nBlockStarts + nBlockSizes
        n += 4;//maskBlockCount
        n += 0;//maskBlockStarts
        n += 0;//maskBlockSizes
        //n += 4;//reserved
        n += (unsigned int) this->data[i]->size();//packedDna

    }

    return n;
}



// closely similar to: http://genome.ucsc.edu/FAQ/FAQformat.html#format7
void fasta_to_fastafs::write(std::string filename)
{
    std::fstream twobit_out_stream(filename.c_str(), std::ios :: out | std::ios :: binary);
    if(twobit_out_stream.is_open()) {
        unsigned int four_bytes;
        unsigned char byte;

        char ch1[] = TWOBIT_MAGIC;
        twobit_out_stream.write(reinterpret_cast<char *> (&ch1), (size_t) 4);

        char ch2[] = TWOBIT_VERSION;
        twobit_out_stream.write(reinterpret_cast<char *> (&ch2), (size_t) 4);

        //four_bytes = (unsigned int) this->data.size();
        //unsigned int n = (unsigned int) this->data.size();

        char ch3[4];
        uint_to_fourbytes(ch3, (unsigned int) this->data.size());
        twobit_out_stream.write(reinterpret_cast<char *> (&ch3), (size_t) 4);

        char ch4[] = "\0\0\0\0";
        twobit_out_stream.write(reinterpret_cast<char *> (&ch4), (size_t) 4);

        // write indices
        for(unsigned int i = 0; i < this->data.size(); i++) {
            byte = (unsigned char) this->data[i]->name.size();
            twobit_out_stream.write((char *) &byte, (size_t) 1);

            for(unsigned int j = 0; j < this->data[i]->name.size(); j++) {
                byte = (unsigned char) this->data[i]->name[j];
                twobit_out_stream.write((char *) &byte, (size_t) 1);
            }

            uint_to_fourbytes(ch3, this->get_sequence_offset(i));
            twobit_out_stream.write(reinterpret_cast<char *> (&ch3), (size_t) 4);
        }

        // write data
        for(unsigned int i = 0; i < this->data.size(); i++) {
            //s->n
            uint_to_fourbytes(ch3, this->data[i]->n);
            twobit_out_stream.write(reinterpret_cast<char *> (&ch3), (size_t) 4);

            //s-N
            uint_to_fourbytes(ch3, (unsigned int )this->data[i]->n_starts.size());
            twobit_out_stream.write(reinterpret_cast<char *> (&ch3), (size_t) 4);

            //s->n_starts
            for(unsigned int j = 0; j < this->data[i]->n_starts.size(); ++j) {
                uint_to_fourbytes(ch3, this->data[i]->n_starts[j]);
                twobit_out_stream.write(reinterpret_cast<char *> (&ch3), (size_t) 4);
            }

            //s->n_ends
            for(unsigned int j = 0; j < this->data[i]->n_ends.size(); ++j) {
                uint_to_fourbytes(ch3, this->data[i]->n_ends[j]);
                twobit_out_stream.write(reinterpret_cast<char *> (&ch3), (size_t) 4);
            }

            //s->M
            four_bytes = (unsigned int) 0;
            twobit_out_stream.write( reinterpret_cast<char *>(&four_bytes), 4 );

            // @todo this->data[i].write_twobit_data(fstream);
            for(unsigned int j = 0; j < this->data[i]->twobits.size(); ++j) {
                twobit_out_stream.write((char *) &this->data[i]->twobits[j], (size_t) 1);
            }

            /*
            // this is slower for some reason
            for(std::vector<unsigned char>::iterator j = this->data[i]->twobits.begin(); j != this->data[i]->twobits.end(); ++j) {
                twobit_out_stream.write((char *)&(*j), (size_t) 1);
            }
            */
        }

        twobit_out_stream.close();
    } else {
        throw std::runtime_error("Could not write to file: " + filename);
    }
}
