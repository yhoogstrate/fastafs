
#include "include/fasta.hpp"

class Fasta {
    std::string *filename = nullptr;
	fasta data;
	
	public:
        Fasta(std::string *fname);
		int cache(void);
};
