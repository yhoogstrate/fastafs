

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
protected:
    std::array<unsigned char, 2> bits; // 00000000 00000000

    twobit_flag();

    void set_flag(unsigned char, bool);// counting flag from bit 0(!)
    bool get_flag(unsigned char) const;

public:
    void set(unsigned char *);
    std::array<unsigned char, 2> &get_bits(void);
    const std::array<unsigned char, 2> &get_bits(void) const;
};


class fastafs_flags : public twobit_flag
{
public:
    bool is_complete() const   { return (bits[0] & 0x80) != 0; }
    bool is_incomplete() const { return !is_complete(); }

    void set_complete();
    void set_incomplete();
};



class fastafs_sequence_flags : public twobit_flag
{
public:
    // bit 7 = TYPE_1 (0x80), bit 6 = TYPE_2 (0x40)
    bool is_dna() const              { return (bits[0] & 0xC0) == 0x00; } // alphabet: 'ACTG' + 'N'
    bool is_rna() const              { return (bits[0] & 0xC0) == 0x80; } // alphabet: 'ACUG' + 'N'
    bool is_iupac_nucleotide() const { return (bits[0] & 0xC0) == 0x40; } // alphabet: 'ACGTURYKMSWBDHVN' + '-'
    bool is_protein() const          { return (bits[0] & 0xC0) == 0xC0; } // alphabet: 'ABCDEFGHIJKLMNOPQRSTUVWYZX*-'

    bool is_complete() const   { return (bits[0] & 0x10) != 0; }
    bool is_incomplete() const { return !is_complete(); }

    bool is_circular() const { return (bits[0] & 0x08) != 0; }
    bool is_linear() const   { return !is_circular(); }

    bool is_twobit() const  { return (bits[0] & 0x40) == 0; }
    bool is_fourbit() const { return (bits[0] & 0xC0) == 0x40; }


    // set by entity
    void set_dna();
    void set_rna();
    void set_iupac_nucleotide();
    void set_protein();

    void set_complete();
    void set_incomplete();

    void set_linear();
    void set_circular();

};




#endif
