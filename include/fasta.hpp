
#include <vector>

class fasta
{
		std::string *filename = nullptr;
		std::vector<unsigned char> twobit_string;
		
	public:
		fasta(std::string *fname);
		int cache(void);
};
