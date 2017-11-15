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
				char i = 6;
				b = new TwoBitByte();
				for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
					switch(*it) {
						case 't':
						case 'T':
						case 'u':
						case 'U':
							b->set(i, 0);
							break;
						case 'c':
						case 'C':
							b->set(i, 1);
							break;
						case 'a':
						case 'A':
							b->set(i, 2);
							break;
						case 'g':
						case 'G':
							b->set(i, 3);
							break;
					}
					
					if(i == 0) {
						b->print();
						
						this->twobit_string.push_back(b->data);
						//delete b;
						b = new TwoBitByte();
						i = 6;
					}
					else {
						i = (i - 2);// needs to iterat back {6, 4, 2, 0}
					}
				}
			}
		}
		myfile.close();
	}
	
	return 0;
}
