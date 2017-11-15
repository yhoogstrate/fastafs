#include <iostream>
#include <fstream>

#include "TwoBitByte.hpp"

void TwoBitByte::set(unsigned char bit_offset, unsigned char nucleotide)
{
	// byte_offset must be: {0, 2, 4 or 6};
	// nucleotides must be:
	// => T - 00, C - 01, A - 10, G - 11
	// => T - 00, C -  1, A -  2, G -  3
	
	this->data |= nucleotide << bit_offset;
};

void TwoBitByte::print()
{
	// leuk, testen!
	char c1 = this->hash_table[this->data * 4];
	char c2 = this->hash_table[(this->data * 4) + 1] ;
	char c3 = this->hash_table[(this->data * 4) + 2] ;
	char c4 = this->hash_table[(this->data * 4) + 3] ;
	printf("[%i] -> %c%c%c%c\n", this->data * 4, c1 ,c2 ,c3 ,c4);
	
	
	/**/
}