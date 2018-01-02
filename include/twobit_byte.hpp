
#include <array>
#include <map>

#include "config.hpp"

class twobit_byte
{
	public:
		unsigned char data;
		void set(unsigned char, unsigned char);
		const char *get(void);
		char *get(unsigned char);


		static const char twobit_hash_decode[256][5];
		//static const std::map<std::string, unsigned char> twobit_hash_encode;
};
