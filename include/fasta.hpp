
#include <vector>


class fasta
{
		

		std::string *filename = nullptr;
		std::vector<unsigned char> twobit_string;
		
		//@todo create some kind of combined matrix with std::pair<unsigned int, unsigned int>?
		std::vector<unsigned int> n_starts;
		std::vector<unsigned int> n_ends;
		
	public:
		fasta(std::string *fname);
		int cache(void);
};
