
#include <array>

#include "config.hpp"

class twobit_byte
{
	static const char inverse_hash[256][5];
	
	public:
		unsigned char data;
		void set(unsigned char, unsigned char);
		const char *get(void);
		char *get(unsigned char);
};
