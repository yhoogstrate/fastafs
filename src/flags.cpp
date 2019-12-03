

#include <stdlib.h>

#include "flags.hpp"




void twobit_flag::set(char *data)
{
    this->bits[0] = data[0];
    this->bits[1] = data[1];
}



// https://www.learncpp.com/cpp-tutorial/bit-manipulation-with-bitwise-operators-and-bit-masks/
bool twobit_flag::get_flag(unsigned char bit) {
#if DEBUG
	if(bit >= 16) {
		throw std::runtime_error("twobit_flag::get_flag = out of bound: " + std::to_string(bit) + "\n");
	}
#endif //DEBUG

	return (this->bits[bit / 8] & bitmasks[bit]);
}



// https://www.learncpp.com/cpp-tutorial/bit-manipulation-with-bitwise-operators-and-bit-masks/
void twobit_flag::set_flag(unsigned char bit, bool enable) {
	if(bit >= 16) {
		throw std::runtime_error("twobit_flag::set_flag = out of bound: " + std::to_string(bit) + "\n");
	}


	if(enable) { // 
		 //this->bits[bit / 8] |= bitmasks[bit];
		 this->bits[bit / 8] = (unsigned char) (this->bits[bit / 8] | bitmasks[bit]);
	}
	else {
		 //this->bits[bit / 8] &= ~bitmasks[bit];
		 this->bits[bit / 8] = (unsigned char) (this->bits[bit / 8] & ~bitmasks[bit]);
	}
}




bool fastafs_flags::is_complete() {
	return this->get_flag(0);
}

void fastafs_flags::set_complete() {
	this->set_flag(0, true);
}

void fastafs_flags::set_incomplete() {
	this->set_flag(0, false);
}


