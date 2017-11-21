#include <iostream>
#include <fstream>

#include "config.hpp"

#include "twobit_seq.hpp" // #include "twobit_byte.hpp"



twobit_seq::twobit_seq(void) : twobit_data(nullptr), n(0) {
}

/**
 */
#if DEBUG
twobit_seq::~twobit_seq(void) {
	if(this->twobit_data != nullptr) {
		//throw std::runtime_error("2bit/fasta sequence was opened for insertion but not flushed");
		exit(1);
	}
}
#endif //DEBUG


void twobit_seq::add_nucleotide(unsigned char nucleotide) {
	switch(this->n % 4) {
		case 0:
			this->twobit_data = new twobit_byte();
			this->twobit_data->set(6, nucleotide);
		break;
		case 1:
			this->twobit_data->set(4, nucleotide);
		break;
		case 2:
			this->twobit_data->set(2, nucleotide);
		break;
		case 3:
			this->twobit_data->set(0, nucleotide);
			this->data.push_back(this->twobit_data->data);
			delete this->twobit_data;
			this->twobit_data = new twobit_byte();
		break;
#if DEBUG
		default:
			throw std::invalid_argument("add_nucleotide needs to be 1, 2 or 3\n");
		break;
#endif //DEBUG
	}
	
	this->n ++;
}


/**
 * @brief MAY ONLY BE USED TO INSERT FULL TWOBIT BYTES REPRESENTING FOUR CHARS!
 *  otherwise use add_twobit_sticky_end(twobit_byte& tb, i < 4);
 */
void twobit_seq::add_twobit(twobit_byte& tb) {
	this->data.push_back(tb.data);
	this->n++;
}

//void flush_reading();

void twobit_seq::close_reading() {
	if(this->n % 4 != 0) { // sticky end
		// set remaining 3, 2 or 1 bitpairs to 0
		// insert semi closed twobit
	}
	delete this->twobit_data;
	this->twobit_data = nullptr;
}






void twobit_seq::print(void) {
	printf(">%s (size=%i, compressed=%i)\n", this->name.c_str(), this->n, (unsigned int) this->size());
}

size_t twobit_seq::size(void) {
	return this->data.size();
}

