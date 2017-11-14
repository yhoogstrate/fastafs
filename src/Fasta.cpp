
#include "include/Fasta.hpp"


// is resource by filename required?
// not yet, maybe other classes or empty?
Fasta::Fasta(std::string *fname) {
	this->filename = fname;
}


int Fasta::cache(void) {
	std::cout << "yeah -- " << *this->filename << std::endl;
	//printf("\n[%s]\n", this->filename.c_str());
	return 0;
}
