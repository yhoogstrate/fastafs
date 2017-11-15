#include <iostream>
#include <fstream>

#include "Fasta.hpp"


// is resource by filename required?
// not yet, maybe other classes or empty?
Fasta::Fasta(std::string *fname)
{
	this->filename = fname;
}


int Fasta::cache(void)
{
	std::cout << "parsing " << *this->filename << std::endl;
	
	std::string line;
	std::ifstream myfile (*this->filename);
	if (myfile.is_open()) {
		while( getline (myfile, line)) {
			std::cout << line << '\n';
		}
		myfile.close();
	}
	
	return 0;
}
