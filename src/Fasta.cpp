#include <iostream>
#include <fstream>

#include "Fasta.hpp"


// is resource by filename required?
// not yet, maybe other classes or empty?
Fasta::Fasta(std::string *fname)
{
	this->filename = fname;
}

// http://genome.ucsc.edu/FAQ/FAQformat.html#format7 2bit format explained
int Fasta::cache(void)
{
	std::cout << "parsing " << *this->filename << std::endl;
	
	std::string line;
	std::ifstream myfile (*this->filename);
	std::string sequence = "";

	if (myfile.is_open()) {
		
		while(getline (myfile, line)) {
			if (line[0] == '>') {
				line.erase(0, 1);// erases first part, quicker would be pointer from first char
				std::cout << line << '\n';
			}
			else {
				/*
				char some_char = 0;
				some_char |= 1 << 0; // set the 7th bit (least significant bit)
				some_char |= 1 << 1; // set the 6th bit
				some_char |= 1 << 2; // set the 5th bit
				 */
				
				for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
					switch(*it) { //  T - 00, C - 01, A - 10, G - 11
						case 'a':
						case 'A':
							sequence.push_back('0');
						break;
						case 'c':
						case 'C':
							sequence.push_back('1');
							break;
						case 't':
						case 'T':
						case 'u':
						case 'U':
							sequence.push_back('2');
							break;
						case 'g':
						case 'G':
							sequence.push_back('3');
						break;
					}
				}
			}
		}
		myfile.close();
	}
	
	return 0;
}
