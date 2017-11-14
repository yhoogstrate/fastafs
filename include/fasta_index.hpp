
#include <iostream>
#include <vector>



struct fasta_index {
	/* index of the fasta file
	 * (chr name, nucleotides)
	 * ("chr1", 6467)
	 * ("chr2", 5344)
	 */

	std::vector<std::string> sequences;
};
