
#include <vector>

#include "twobit_seq.hpp"

class fasta
{


		std::string *filename = nullptr;
		
		std::vector<twobit_seq *> data;
		
	public:
		fasta(std::string *fname);
		~fasta();
		int cache(void);
		void print();
		void write(std::string);
};
