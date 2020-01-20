#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>


#include "config.hpp"

#include "utils.hpp"



class SequenceRegion {
	std::string seq_name;

	bool has_range;
	bool has_end;
	
	off_t start;
	off_t end;
	
	public:
		SequenceRegion(char *);
	
	private:
		void parse(char *);
};

