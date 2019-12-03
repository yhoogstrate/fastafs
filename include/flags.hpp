

#ifndef FLAGS_HPP
#define FLAGS_HPP

#include <array>

constexpr std::array<unsigned char, 16> bitmasks = {
	0b1000'0000, // represents bit 7
	0b0100'0000, // represents bit 6
	0b0010'0000, // represents bit 5
	0b0001'0000, // represents bit 4
	0b0000'1000, // represents bit 3
	0b0000'0100, // represents bit 2 
	0b0000'0010, // represents bit 1
	0b0000'0001, // represents bit 0

	0b1000'0000, // represents bit 7
	0b0100'0000, // represents bit 6
	0b0010'0000, // represents bit 5
	0b0001'0000, // represents bit 4
	0b0000'1000, // represents bit 3
	0b0000'0100, // represents bit 2 
	0b0000'0010, // represents bit 1
	0b0000'0001, // represents bit 0
};


//#include "utils.hpp"


class twobit_flag {
	protected:
		unsigned char bits[2];// 00000000 00000000

		// set by flag
		void set_flag(unsigned char, bool);// counting flag from bit 0(!)
		bool get_flag(unsigned char);

	public:
		void set(char *);
};


class fastafs_flags : public twobit_flag
{
	public:
		bool is_complete();
		bool is_incomplete()
		{
			return !this->is_complete();
		};
		
		void set_complete();
		void set_incomplete();
};



class fastafs_sequence_flags
{
private:
    unsigned char bits[2];// 00000000 00000000

    // set by flag
    void set_flag(unsigned char, bool);// counting flag from bit 0(!)
    bool get_flag(unsigned char);// counting flag position from bit 0


public:
    void set(char*);

    bool is_dna(); // alphabet: 'ACTG' + 'N'
    bool is_rna(); // alphabet: 'ACUG' + 'N'
    bool is_iupec_nucleotide(); // alphabet: 'ACGTURYKMSWBDHVN' + '-'

    bool is_complete();
    bool is_incomplete()
    {
        return !this->is_complete();
    };

    bool is_linear();
    bool is_circular()
    {
        return !this->is_linear();
    };



    // set by entity
    void set_dna();
    void set_rna();
    void set_iupec_nucleotide();

    void set_complete();
    void set_incomplete();

    void set_linear();
    void set_circular();

};




#endif
