
#include <array>


class TwoBitByte
{
	protected:
		unsigned char data;
		//char hash_table[5][4] = {"TTTT","TTTC","TTTA","TTTG","TTCT"};
		//std::array<char[4], 5> buffer = {"TTTT","TTTC","TTTA","TTTG","TTCT"};
		
		char hash_table[(256*4) + 1] = "TTTTTTTCTTTATTTGTTCTTTCCTTCATTCGTTATTTACTTAATTAGTTGTTTGCTTGATTGGTCTTTCTCTCTATCTGTCCTTCCCTCCATCCGTCATTCACTCAATCAGTCGTTCGCTCGATCGGTATTTATCTATATATGTACTTACCTACATACGTAATTAACTAAATAAGTAGTTAGCTAGATAGGTGTTTGTCTGTATGTGTGCTTGCCTGCATGCGTGATTGACTGAATGAGTGGTTGGCTGGATGGGCTTTCTTCCTTACTTGCTCTCTCCCTCACTCGCTATCTACCTAACTAGCTGTCTGCCTGACTGGCCTTCCTCCCTACCTGCCCTCCCCCCCACCCGCCATCCACCCAACCAGCCGTCCGCCCGACCGGCATTCATCCATACATGCACTCACCCACACACGCAATCAACCAAACAAGCAGTCAGCCAGACAGGCGTTCGTCCGTACGTGCGCTCGCCCGCACGCGCGATCGACCGAACGAGCGGTCGGCCGGACGGGATTTATTCATTAATTGATCTATCCATCAATCGATATATACATAAATAGATGTATGCATGAATGGACTTACTCACTAACTGACCTACCCACCAACCGACATACACACAAACAGACGTACGCACGAACGGAATTAATCAATAAATGAACTAACCAACAAACGAAATAAACAAAAAAAGAAGTAAGCAAGAAAGGAGTTAGTCAGTAAGTGAGCTAGCCAGCAAGCGAGATAGACAGAAAGAGAGGTAGGCAGGAAGGGGTTTGTTCGTTAGTTGGTCTGTCCGTCAGTCGGTATGTACGTAAGTAGGTGTGTGCGTGAGTGGGCTTGCTCGCTAGCTGGCCTGCCCGCCAGCCGGCATGCACGCAAGCAGGCGTGCGCGCGAGCGGGATTGATCGATAGATGGACTGACCGACAGACGGAATGAACGAAAGAAGGAGTGAGCGAGAGAGGGGTTGGTCGGTAGGTGGGCTGGCCGGCAGGCGGGATGGACGGAAGGAGGGGTGGGCGGGAGGGG" ; //+ "TTTC","TTTA","TTTG","TTCT"};
	
	public:
		void set(unsigned char, unsigned char);
		void print();
};
