


/*
#include <algorithm>
#include <arpa/inet.h>
#include <errno.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
*/

//#include "config.hpp"

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
void twobit_flag::set_flag(unsigned char, bool enable) {
	if(enable) { // 
	}
	else {
	}
}




bool fastafs_flags::is_complete() {
	return this->get_flag(0);
}
