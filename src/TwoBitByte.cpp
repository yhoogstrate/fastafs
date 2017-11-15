#include <iostream>
#include <fstream>

#include "TwoBitByte.hpp"

void TwoBitByte::set(unsigned char bit_offset, unsigned char nucleotide) {
	// byte_offset must be: {0, 2, 4 or 6};
	// nucleotides must be:
	// => T - 00, C - 01, A - 10, G - 11
	// => T - 00, C -  1, A -  2, G -  3

	data |= nucleotide << bit_offset;
};

void TwoBitByte::print() {
	// leuk, testen!
	printf("[%i]\n", data);
}