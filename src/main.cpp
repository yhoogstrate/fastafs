
#include <iostream>
#include <array>
#include <vector>




// https://github.com/facebook/zstd/issues/521

struct fasta_index {
	std::vector<std::string> sequences;
};

struct fasta {
	bool compressed;
	bool twobit;

	fasta_index index;
};

class Fasta {
	int cache(void) {
		return 0;
	}
	
	
};

int main()
{
	std::cout << "usage: fastafs index -n hg38 test.fa";
	return 0;
}

