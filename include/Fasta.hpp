
#include <vector>

class Fasta
{
		std::string *filename = nullptr;
		std::vector<unsigned char> twobit_string;
		
	public:
		Fasta(std::string *fname);
		int cache(void);
};
