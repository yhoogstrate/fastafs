

#ifndef FLAGS_HPP
#define FLAGS_HPP

#include <array>


const static unsigned char FASTAFS_BITFLAG_COMPLETE = 0;

const static unsigned char FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_1 = 0;
const static unsigned char FASTAFS_SEQUENCE_BITFLAG_SEQUENCE_TYPE_2 = 1;
// const static unsigned char FASTAFS_SEQUENCE_BITFLAG_???? = 2 ; // is reserved
const static unsigned char FASTAFS_SEQUENCE_BITFLAG_COMPLETE = 3;
const static unsigned char FASTAFS_SEQUENCE_BITFLAG_CIRCULAR = 4;




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



class twobit_flag
{
private:
    std::array<unsigned char, 2> bits; // 00000000 00000000

protected:
    twobit_flag();

    void set_flag(unsigned char, bool);// counting flag from bit 0(!)
    bool get_flag(unsigned char);

public:
    void set(char *);
    std::array<unsigned char, 2> &get_bits(void); // get bit 0 or bit 1 - needed for exporting flags to file(s)
};


class fastafs_flags : public twobit_flag
{
public:
    bool is_complete();
    bool is_incomplete();

    void set_complete();
    void set_incomplete();
};



class fastafs_sequence_flags : public twobit_flag
{

public:
    bool is_dna(); // alphabet: 'ACTG' + 'N'
    bool is_rna(); // alphabet: 'ACUG' + 'N'
    bool is_iupec_nucleotide(); // alphabet: 'ACGTURYKMSWBDHVN' + '-'
    bool is_protein(); // alphabet: 'ABCDEFGHIJKLMNOPQRSTUVWYZX*-'

    bool is_complete();
    bool is_incomplete(); // is not complete

    bool is_circular();
    bool is_linear(); // is not circular

    bool is_twobit();
    bool is_fourbit();


    // set by entity
    void set_dna();
    void set_rna();
    void set_iupec_nucleotide();
    void set_protein();

    void set_complete();
    void set_incomplete();

    void set_linear();
    void set_circular();

};




#endif
