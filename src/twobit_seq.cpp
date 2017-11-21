#include <iostream>
#include <fstream>

#include "config.hpp"

#include "twobit_seq.hpp"
#include "twobit_byte.hpp"


twobit_seq::twobit_seq(void) : n(0) {
}

void twobit_seq::print(void) {
	printf(">%s (%i)\n", this->name.c_str(), this->n);
}
