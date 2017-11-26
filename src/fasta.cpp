#include <iostream>
#include <fstream>

#include "config.hpp"

#include "twobit_seq.hpp" // #include "twobit_byte.hpp"
#include "fasta.hpp"


// is resource by filename required?
// not yet, maybe other classes or empty?
fasta::fasta(std::string *fname)
{
	this->filename = fname;
}

// http://genome.ucsc.edu/FAQ/FAQformat.html#format7 2bit format explained
int fasta::cache(void)
{
	std::cout << "parsing " << *this->filename << std::endl;
	
	twobit_byte *b;
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
					s->print();
					delete s;
				}
				
				s = new twobit_seq();
				s->name = line;
				//s->open_reading();
			} else {
				unsigned int i = 0, n_start = 0, n_stop = 0;
				uint8_t j = 6;
				b = new twobit_byte();
				
				for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
					switch(*it) {
						case 't':
						case 'T':
						case 'u':
						case 'U':
							b->set(j, NUCLEOTIDE_T);
							s->add_nucleotide(NUCLEOTIDE_T);
							break;
						case 'c':
						case 'C':
							b->set(j, NUCLEOTIDE_C);
							s->add_nucleotide(NUCLEOTIDE_C);
							break;
						case 'a':
						case 'A':
							b->set(j, NUCLEOTIDE_A);
							s->add_nucleotide(NUCLEOTIDE_A);
							break;
						case 'g':
						case 'G':
							b->set(j, NUCLEOTIDE_G);
							s->add_nucleotide(NUCLEOTIDE_G);
							break;
						case 'n':
						case 'N':
							if(n_start == 0) {
								n_start = i + 1;
								n_stop = i + 1;
							} else if(n_stop == i) {
								n_stop++;
							}
							// store 1-based and use 0 als null
							// store begin_n and end_n;
							// if last_n == i-1:
							//     last_n++
							break;
						default:
							std::cerr << "invalid chars in FASTA file" << std::endl;
							exit(1);
							break;
					}
					
					
					
					/* ---- flush function ----------- */
					if(n_start != 0 && *it != 'n' && *it != 'N') {
						//printf("store N [%i, %i]\n", n_start - 1, n_stop - 1);
						n_start = 0;
						//n_stop = 0;
						// set n_start and n_stop to 0
					}
					
					if(j == 0) {
						this->twobit_string.push_back((unsigned char) b->data);
						delete b;
						b = new twobit_byte();
						j = 6;
					}
					/* --------------------------------- */
					else {
						j --;
						j --;// needs to iterat back {6, 4, 2, 0}
					}
					
					i++;
				}
				
				/* ---- flush function ----------- */
				if(n_start != 0) {
					n_stop++;
					printf("store N [%i, %i]\n", n_start - 1, n_stop - 1);
				}
				
				if(i % 4 != 0) {
					this->twobit_string.push_back((unsigned char) b->data);
					delete b;
					b = new twobit_byte();
				}
				/* --------------------------------- */
				
				delete b;
			}
		}
		myfile.close();
	}
	
	
	
	if(s != nullptr) {
		s->print();
		s->close_reading();
		delete s;
	}
	return 0;
}
