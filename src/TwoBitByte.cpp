#include <iostream>
#include <fstream>

#include "TwoBitByte.hpp"

const char TwoBitByte::hash_table[(256*4) + 1] = "TTTTTTTCTTTATTTGTTCTTTCCTTCATTCGTTATTTACTTAATTAGTTGTTTGCTTGATTGGTCTTTCTCTCTATCTGTCCTTCCCTCCATCCGTCATTCACTCAATCAGTCGTTCGCTCGATCGGTATTTATCTATATATGTACTTACCTACATACGTAATTAACTAAATAAGTAGTTAGCTAGATAGGTGTTTGTCTGTATGTGTGCTTGCCTGCATGCGTGATTGACTGAATGAGTGGTTGGCTGGATGGGCTTTCTTCCTTACTTGCTCTCTCCCTCACTCGCTATCTACCTAACTAGCTGTCTGCCTGACTGGCCTTCCTCCCTACCTGCCCTCCCCCCCACCCGCCATCCACCCAACCAGCCGTCCGCCCGACCGGCATTCATCCATACATGCACTCACCCACACACGCAATCAACCAAACAAGCAGTCAGCCAGACAGGCGTTCGTCCGTACGTGCGCTCGCCCGCACGCGCGATCGACCGAACGAGCGGTCGGCCGGACGGGATTTATTCATTAATTGATCTATCCATCAATCGATATATACATAAATAGATGTATGCATGAATGGACTTACTCACTAACTGACCTACCCACCAACCGACATACACACAAACAGACGTACGCACGAACGGAATTAATCAATAAATGAACTAACCAACAAACGAAATAAACAAAAAAAGAAGTAAGCAAGAAAGGAGTTAGTCAGTAAGTGAGCTAGCCAGCAAGCGAGATAGACAGAAAGAGAGGTAGGCAGGAAGGGGTTTGTTCGTTAGTTGGTCTGTCCGTCAGTCGGTATGTACGTAAGTAGGTGTGTGCGTGAGTGGGCTTGCTCGCTAGCTGGCCTGCCCGCCAGCCGGCATGCACGCAAGCAGGCGTGCGCGCGAGCGGGATTGATCGATAGATGGACTGACCGACAGACGGAATGAACGAAAGAAGGAGTGAGCGAGAGAGGGGTTGGTCGGTAGGTGGGCTGGCCGGCAGGCGGGATGGACGGAAGGAGGGGTGGGCGGGAGGGG" ;

void TwoBitByte::set(unsigned char bit_offset, unsigned char nucleotide)
{
	// byte_offset must be: {0, 2, 4 or 6};
	// nucleotides must be:
	// => T - 00, C - 01, A - 10, G - 11
	// => T - 00, C -  1, A -  2, G -  3
	
	this->data |= (unsigned char) (nucleotide << bit_offset);
};

void TwoBitByte::print()
{
	// leuk, testen!
	char c1 = TwoBitByte::hash_table[this->data * 4];
	char c2 = TwoBitByte::hash_table[(this->data * 4) + 1] ;
	char c3 = TwoBitByte::hash_table[(this->data * 4) + 2] ;
	char c4 = TwoBitByte::hash_table[(this->data * 4) + 3] ;
	printf("[%i] -> %c%c%c%c\n", this->data , c1 ,c2 ,c3 ,c4);
	
}
