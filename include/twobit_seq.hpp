#include <vector>

#include "config.hpp"

#include "twobit_byte.hpp"

class twobit_seq
{
	private:
		std::vector<unsigned char> data;
		twobit_byte *twobit_data;
		
		bool previous_was_N;

	public:
		twobit_seq(void);
#if DEBUG
		~twobit_seq(void);
#endif //DEBUG
		
		unsigned int n;// effective size in nt
		unsigned int N;// effective size of unnknown nulceotides (N's) in nt
		size_t size(void);// size of compressed data, may be longer than 4*this->n
		
		std::string name;
		
		std::vector<unsigned int> n_starts;
		std::vector<unsigned int> n_ends;
		
		void add_N();
		void add_nucleotide(unsigned char);
		void add_twobit(twobit_byte&);
		void flush_reading();
		void close_reading();
		
		void print(void);
};
