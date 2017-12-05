#include <iostream>
#include <fstream>

#include "config.hpp"

#include "fasta.hpp"


// is resource by filename required?
// not yet, maybe other classes or empty?
fasta::fasta(std::string *fname)
{
	this->filename = fname;
}


fasta::~fasta() {
	for(unsigned int i = 0; i < this->data.size();i++){
		delete this->data[i];
	}
}

int fasta::cache(void)
{
	std::cout << "parsing " << *this->filename << std::endl;
	
	twobit_seq *s = nullptr;
	
	std::string line;
	std::ifstream myfile (*this->filename);
	std::string sequence = "";
	
	if (myfile.is_open()) {
	
		while(getline (myfile, line)) {
			if (line[0] == '>') {
				line.erase(0, 1);// erases first part, quicker would be pointer from first char
				
				if(s != nullptr) {
					s->close_reading();
					
					this->data.push_back(s);
					s = nullptr;
				}
				
				s = new twobit_seq();
				s->name = line;
			} else {
			
				for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
					switch(*it) {
						case 't':
						case 'T':
						case 'u':
						case 'U':
							s->add_nucleotide(NUCLEOTIDE_T);
							break;
						case 'c':
						case 'C':
							s->add_nucleotide(NUCLEOTIDE_C);
							break;
						case 'a':
						case 'A':
							s->add_nucleotide(NUCLEOTIDE_A);
							break;
						case 'g':
						case 'G':
							s->add_nucleotide(NUCLEOTIDE_G);
							break;
						case 'n':
						case 'N':
							s->add_N();
							break;
						default:
							std::cerr << "invalid chars in FASTA file" << std::endl;
							exit(1);
							break;
					}
				}
			}
		}
		myfile.close();
	}
	
	
	if(s != nullptr) {
		s->close_reading();
		
		this->data.push_back(s);
		s = nullptr;

	}
	return 0;
}

void fasta::print(void) {
	for(unsigned int i = 0; i < this->data.size();i++){
		this->data[i]->print();
	}
}


// http://genome.ucsc.edu/FAQ/FAQformat.html#format7 2bit format explained
/*
A .2bit file stores multiple DNA sequences (up to 4 Gb total) in a compact randomly-accessible format. The file contains masking information as well as the DNA itself.
The file begins with a 16-byte header containing the following fields:
 - sequenceCount - the number of sequences in the file.
 - reserved - always zero for now

All fields are 32 bits unless noted. If the signature value is not as given, the reader program should byte-swap the signature and check if the swapped version matches. If so, all multiple-byte entities in the file will have to be byte-swapped. This enables these binary files to be used unchanged on different architectures.
The header is followed by a file index, which contains one entry for each sequence. Each index entry contains three fields:
nameSize - a byte containing the length of the name field
name - the sequence name itself, of variable length depending on nameSize
offset - the 32-bit offset of the sequence data relative to the start of the file
The index is followed by the sequence records, which contain nine fields:
dnaSize - number of bases of DNA in the sequence
nBlockCount - the number of blocks of Ns in the file (representing unknown sequence)
nBlockStarts - an array of length nBlockCount of 32 bit integers indicating the starting position of a block of Ns
nBlockSizes - an array of length nBlockCount of 32 bit integers indicating the length of a block of Ns
maskBlockCount - the number of masked (lower-case) blocks
maskBlockStarts - an array of length maskBlockCount of 32 bit integers indicating the starting position of a masked block
maskBlockSizes - an array of length maskBlockCount of 32 bit integers indicating the length of a masked block
reserved - always zero for now
packedDna - the DNA packed to two bits per base, represented as so: T - 00, C - 01, A - 10, G - 11. The first base is in the most significant 2-bit byte; the last base is in the least significant 2 bits. For example, the sequence TCAG is represented as 00011011.
For a complete definition of all fields in the twoBit format, see this description in the source code.
 */
void fasta::write(std::string filename) {

	std::fstream twobit_out_stream (filename.c_str(),std::ios :: out | std::ios :: binary);
	
	char ch1[] = TWOBIT_MAGIC;
	char ch2[] = TWOBIT_VERSION;
	char ch3[] = "\0\0\0\0";

	twobit_out_stream.write(reinterpret_cast<char *> (&ch1),(size_t) 4);
	twobit_out_stream.write(reinterpret_cast<char *> (&ch2),(size_t) 4);
	
	unsigned int i = (unsigned int) this->data.size();
	twobit_out_stream.write( reinterpret_cast<char*>(&i), sizeof(i) );
	
	twobit_out_stream.write(reinterpret_cast<char *> (&ch3),(size_t) 4);
	
	twobit_out_stream.close();

}