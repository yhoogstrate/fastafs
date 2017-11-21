
#include <vector>


class twobit_seq
{
	public:
		twobit_seq(void);
		unsigned int n;
		
		std::string name;
		std::vector<unsigned char> twobit_string;

		std::vector<unsigned int> n_starts;
		std::vector<unsigned int> n_ends;
		
		void print(void);
};
