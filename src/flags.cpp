

#include <stdlib.h>

#include "flags.hpp"



twobit_flag::twobit_flag()
{
    // ensure all bits are set, this prevents unexpected or undefined behaviour
    this->bits[0] = '\0';
    this->bits[1] = '\0';
}



void twobit_flag::set(char *data)
{
    this->bits[0] = data[0];
    this->bits[1] = data[1];
}



// https://www.learncpp.com/cpp-tutorial/bit-manipulation-with-bitwise-operators-and-bit-masks/
bool twobit_flag::get_flag(unsigned char bit)
{
#if DEBUG
    if(bit >= 16) {
        throw std::runtime_error("twobit_flag::get_flag = out of bound: " + std::to_string(bit) + "\n");
    }
#endif //DEBUG

    return (this->bits[bit / 8] & bitmasks[bit]);
}



// https://www.learncpp.com/cpp-tutorial/bit-manipulation-with-bitwise-operators-and-bit-masks/
void twobit_flag::set_flag(unsigned char bit, bool enable)
{
    if(bit >= 16) {
        throw std::runtime_error("twobit_flag::set_flag = out of bound: " + std::to_string(bit) + "\n");
    }


    if(enable) { //
        //this->bits[bit / 8] |= bitmasks[bit];
        this->bits[bit / 8] = (unsigned char)(this->bits[bit / 8] | bitmasks[bit]);
    } else {
        //this->bits[bit / 8] &= ~bitmasks[bit];
        this->bits[bit / 8] = (unsigned char)(this->bits[bit / 8] & ~bitmasks[bit]);
    }
}


std::array<unsigned char, 2> &twobit_flag::get_bits(void)
{
    return this->bits;
}




bool fastafs_flags::is_complete()
{
    return this->get_flag(FASTAFS_BITFLAG_COMPLETE);
}

void fastafs_flags::set_complete()
{
    this->set_flag(FASTAFS_BITFLAG_COMPLETE, true);
}

void fastafs_flags::set_incomplete()
{
    this->set_flag(FASTAFS_BITFLAG_COMPLETE, false);
}




// alphabet: 'ACTG' + 'N'
bool fastafs_sequence_flags::is_dna()
{
    return (
               this->get_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_1) == false &&
               this->get_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_2) == false);
}

// alphabet: 'ACUG' + 'N'
bool fastafs_sequence_flags::is_rna()
{
    return (
               this->get_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_1) == true &&
               this->get_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_2) == false);
}

// alphabet: 'ACGTURYKMSWBDHVN' + '-'
bool fastafs_sequence_flags::is_iupec_nucleotide()
{
    return (
               this->get_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_1) == false &&
               this->get_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_2) == true);
}

bool fastafs_sequence_flags::is_protein()
{
    return (
               this->get_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_1) == true &&
               this->get_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_2) == true);
}


bool fastafs_sequence_flags::is_complete()
{
    return this->get_flag(FASTAFS_SEQUENCE_BITFLAG_COMPLETE);
}

bool fastafs_sequence_flags::is_circular()
{
    return this->get_flag(FASTAFS_SEQUENCE_BITFLAG_CIRCULAR);
}






void fastafs_sequence_flags::set_dna()
{
    this->set_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_1, false); // 0,0
    this->set_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_2, false);
}

void fastafs_sequence_flags::set_rna()
{
    this->set_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_1, true); // 1,0
    this->set_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_2, false);
}

void fastafs_sequence_flags::set_iupec_nucleotide()
{
    this->set_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_1, false); // 0,1
    this->set_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_2, true);
}

void fastafs_sequence_flags::set_protein()
{
    this->set_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_1, true); // 1,1
    this->set_flag(FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_2, true);
}


void fastafs_sequence_flags::set_complete()
{
    this->set_flag(FASTAFS_SEQUENCE_BITFLAG_COMPLETE, true);
}

void fastafs_sequence_flags::set_incomplete()
{
    this->set_flag(FASTAFS_SEQUENCE_BITFLAG_COMPLETE, false);
}

void fastafs_sequence_flags::set_linear()
{
    this->set_flag(FASTAFS_SEQUENCE_BITFLAG_CIRCULAR, false);
}

void fastafs_sequence_flags::set_circular()
{
    this->set_flag(FASTAFS_SEQUENCE_BITFLAG_CIRCULAR, true);
}


