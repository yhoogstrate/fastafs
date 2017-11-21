
#include <array>



class two_bit_byte
{
	static const char inverse_hash[256][5];
	
	public:
		unsigned char data;
		void set(unsigned char, unsigned char);
		const char *get();
		char *get(unsigned char length);
};
