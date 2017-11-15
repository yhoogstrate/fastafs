
#include "fasta_index.hpp"

struct fasta {
	bool compressed;
	//bool twobit;
    std::vector<unsigned char> twobit_string;
	
	fasta_index index;
};
