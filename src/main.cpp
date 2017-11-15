
#include <iostream>
#include <array>
#include <vector>

//#include "include/fasta_index.hpp"
//#include "include/fasta.hpp"

#include "Fasta.hpp"


// https://github.com/facebook/zstd/issues/521
// https://github.com/samtools/samtools/blob/develop/faidx.c




int main(int argc, char *argv[])
{
	std::string fname = "test/cache/test.fa";
	Fasta f = Fasta(&fname);
	printf("[%i]", f.cache());
	
	std::cout << "usage: fastafs cache -n hg38 test.fa";
	return 0;
}

