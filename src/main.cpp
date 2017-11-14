
#include <iostream>
#include <array>
#include <vector>

#include "include/fasta_index.hpp"
#include "include/fasta.hpp"


// https://github.com/facebook/zstd/issues/521
// https://github.com/samtools/samtools/blob/develop/faidx.c

class Fasta {
	fasta data;
	
	int cache(void) {
		return 0;
	}
	
	
};

int main(int argc, char * argv[])
{
	std::cout << "usage: fastafs cache -n hg38 test.fa";
	return 0;
}

