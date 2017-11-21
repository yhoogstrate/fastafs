
#include <array>



class two_bit_byte
{
		//protected:
		//char hash_table[5][4] = {"TTTT","TTTC","TTTA","TTTG","TTCT"};
		//std::array<char[4], 5> buffer = {"TTTT","TTTC","TTTA","TTTG","TTCT"};
		static const char inverse_hash[256][5];
		
	public:
		unsigned char data;
		void set(unsigned char, unsigned char);
		const char *get();
		char *get(unsigned char length);
};
