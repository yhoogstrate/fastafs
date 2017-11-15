#include <iostream>
#include <fstream>

#include "TwoBitByte.hpp"
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
	
	TwoBitByte *b;
	
	std::string line;
	std::ifstream myfile (*this->filename);
	std::string sequence = "";
	
	if (myfile.is_open()) {
	
		while(getline (myfile, line)) {
			if (line[0] == '>') {
				line.erase(0, 1);// erases first part, quicker would be pointer from first char
				std::cout << line << '\n';
			} else {
				unsigned int i = 0;
				char j = 6;
				b = new TwoBitByte();
				for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
					switch(*it) {
						case 't':
						case 'T':
						case 'u':
						case 'U':
							b->set(j, 0);
							break;
						case 'c':
						case 'C':
							b->set(j, 1);
							break;
						case 'a':
						case 'A':
							b->set(j, 2);
							break;
						case 'g':
						case 'G':
							b->set(j, 3);
							break;
						case 'n':
						case 'N':
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
					
					if(j == 0) {
						b->print();
						
						this->twobit_string.push_back(b->data);
						delete b;
						b = new TwoBitByte();
						j = 6;
					}
					else {
						j = (j - 2);// needs to iterat back {6, 4, 2, 0}
					}
					
					i++;
				}
				
				// if i % 4 > 0
				//    save semi filled 2bit byte
				
				delete b;
			}
		}
		myfile.close();
	}
	
	return 0;
}
