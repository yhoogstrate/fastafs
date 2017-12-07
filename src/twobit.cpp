#include <iostream>
#include <fstream>

#include "config.hpp"

#include "twobit.hpp"


// is resource by filename required?
// not yet, maybe other classes or empty?
twobit::twobit(std::string *fname)
{
    this->filename = fname;
}


twobit::~twobit()
{
    for(unsigned int i = 0; i < this->data.size(); i++) {
        delete this->data[i];
    }
}

int twobit::cache(void)
{
    std::cout << "parsing " << *this->filename << std::endl;

    twobit_seq *s = nullptr;

    std::string line;
    std::ifstream myfile (*this->filename);
    std::string sequence = "";

    if (myfile.is_open()) {

        while(getline (myfile, line)) {
            if (line[0] == '>') {
                line.erase(0, 1);// erases first part, quicker would be pointer from first char

                if(s != nullptr) {
                    s->close_reading();

                    this->data.push_back(s);
                    s = nullptr;
                }

                s = new twobit_seq();
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
    }


    if(s != nullptr) {
        s->close_reading();

        this->data.push_back(s);
        s = nullptr;

    }
    return 0;
}

void twobit::print(void)
{
    for(unsigned int i = 0; i < this->data.size(); i++) {
        this->data[i]->print();
    }
}

unsigned int twobit::get_index_size()
{
    unsigned int n = (unsigned int) this->data.size() * 5;// one byte to describe each size + 32bits to describe offset

    for(unsigned int i = 0; i < this->data.size(); i++) {
        n += (unsigned int) this->data[i]->name.size();
    }

    return n;
}

unsigned int twobit::get_sequence_offset(unsigned int sequence)
{
    unsigned int n = 4+4+4+4 + this->get_index_size();

    printf("estimating offset for [%i]\n",sequence);
    for(unsigned int i = 0; i < sequence; i++) {
        n += 4; // dna_size
        n += 4; // n_block_count
        n += (unsigned int) this->data[i]->n_starts.size() * 4 * 2;//nBlockStarts + nBlockSizes
        n += 4;//maskBlockCount
        n += 0;//maskBlockStarts
        n += 0;//maskBlockSizes
        n += 4;//reserved
        n += (unsigned int) this->data[i]->size();//packedDna

        //printf(" packedDNA=%i  || [%i] -> %i\n", this->data[i]->size(),i, n);
    }

    //printf(" [==> %i\n", n);
    return n;
}



// closely similar to: http://genome.ucsc.edu/FAQ/FAQformat.html#format7
void twobit::write(std::string filename)
{

    std::fstream twobit_out_stream (filename.c_str(),std::ios :: out | std::ios :: binary);

    unsigned int four_bytes;
    unsigned char byte;

    char ch1[] = TWOBIT_MAGIC;
    twobit_out_stream.write(reinterpret_cast<char *> (&ch1),(size_t) 4);
    
    char ch2[] = TWOBIT_VERSION;
    twobit_out_stream.write(reinterpret_cast<char *> (&ch2),(size_t) 4);

    //four_bytes = (unsigned int) this->data.size();
    unsigned int n = (unsigned int) this->data.size();
    
    char ch3[4];
    ch3[0] = (unsigned char) ((n >> 24) & 0xFF);
    ch3[1] = (unsigned char) ((n >> 16) & 0xFF);
    ch3[2] = (unsigned char) ((n >> 8) & 0xFF);
    ch3[3] = (unsigned char) (n & 0xFF);
    twobit_out_stream.write(reinterpret_cast<char *> (&ch3),(size_t) 4);

    char ch4[] = "\0\0\0\0";
    twobit_out_stream.write(reinterpret_cast<char *> (&ch4),(size_t) 4);

    // write indices
    for(unsigned int i = 0; i < this->data.size(); i++) {
        byte = (unsigned char) this->data[i]->name.size();
        twobit_out_stream.write((char*) &byte,(size_t) 1);

        for(unsigned int j = 0; j < this->data[i]->name.size(); j++) {
            byte = (unsigned char) this->data[i]->name[j];
            twobit_out_stream.write((char*) &byte,(size_t) 1);
        }

        four_bytes = this->get_sequence_offset(i);
        twobit_out_stream.write( reinterpret_cast<char*>(&four_bytes), 4 );
    }

    // write data
    for(unsigned int i = 0; i < this->data.size(); i++) {
        four_bytes = this->data[i]->n;
        twobit_out_stream.write( reinterpret_cast<char*>(&four_bytes), 4 );

        four_bytes = (unsigned int) this->data[i]->n_starts.size();
        twobit_out_stream.write( reinterpret_cast<char*>(&four_bytes), 4 );

        // N regions
        for(unsigned int j = 0; j < this->data[i]->n_starts.size(); j++) {
            four_bytes = (unsigned int) this->data[i]->n_starts[j];
            twobit_out_stream.write( reinterpret_cast<char*>(&four_bytes), 4 );
        }

        for(unsigned int j = 0; j < this->data[i]->n_ends.size(); j++) {
            four_bytes = (unsigned int) this->data[i]->n_ends[j];
            twobit_out_stream.write( reinterpret_cast<char*>(&four_bytes), 4 );
        }

        // masked, not yet implemented
        four_bytes = (unsigned int) 0;
        twobit_out_stream.write( reinterpret_cast<char*>(&four_bytes), 4 );
        twobit_out_stream.write( reinterpret_cast<char*>(&four_bytes), 4 );
        twobit_out_stream.write( reinterpret_cast<char*>(&four_bytes), 4 );

        //reserved
        twobit_out_stream.write( reinterpret_cast<char*>(&four_bytes), 4 );

        for(unsigned int j = 0; j < this->data[i]->data.size(); j++) {
            byte = (unsigned char) this->data[i]->data[j];
            twobit_out_stream.write((char*) &byte,(size_t) 1);
        }
    }

    twobit_out_stream.close();

}
