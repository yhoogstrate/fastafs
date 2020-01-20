#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>


#include "config.hpp"

#include "utils.hpp"



class SequenceRegion {
	public:
		SequenceRegion(char *);

		std::string seq_name;

		bool has_defined_end;
		
		off_t start;
		off_t end;
	
	private:
		void parse(char *);
};

