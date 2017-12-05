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
	
	//twobit_byte *b;
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
		s->print();
		delete s;
	}
	return 0;
}
