
#include "SequenceRegion.hpp"



SequenceRegion::SequenceRegion(char * seqstr) :
    seq_name("") , has_range(false), has_end(false), start(0), end(0) {

	parse(seqstr);

}


void SequenceRegion::parse(char * seqstr) {
	printf("123'\n");
	printf("[%s]\n", seqstr);
	printf("---\n");

	printf("strlen: %i\n", strlen(seqstr));

	// the + 1 is the also allow parsing "sequence-of-size-255-...-:123-345"
	size_t string_max_pos = std::min(MAX_SIZE_SEQ_NAME + 1, strlen(seqstr));
	ssize_t p = -1;
	for(size_t i = 0; i < string_max_pos && p == -1; i++) {
		if(seqstr[i] == ':') {
			p = (size_t) i;
		}
	}
	
	printf("p = %i\n", (int) p);
	
	if(p > 0) {
		this->seq_name = std::string(seqstr , 0 , p);
		printf("| %s |\n", this->seq_name.c_str());
	} else if(p == -1) {
		
		// either with string > 255 chars or string smaller than 255 without ':'
		this->seq_name = std::string(seqstr , 0 , string_max_pos);
		printf(": %s :\n", this->seq_name.c_str());
	}
	
	printf("\n");
	// chr1:1
	// p = 4
	// strlen = 6
	if(p != -1 and strlen(seqstr) > (p + 1)) {
		// we can parse numbers
		// find position of '-' character

		ssize_t p2 = -1;

		for(size_t i = p; i < strlen(seqstr) && p2 == -1; i++) {
			if(seqstr[i] == ':') {
				p2 = (size_t) i;
			}
		}

		printf("p2 = %i , p = %i\n", p2, p);
		if(p2 == p) {// chrA:-123
			printf("yes!\n");
			this->start = 0;
			printf(" end = [%s]\n", std::string(seqstr,p + 2,strlen(seqstr)).c_str());
			this->end = std::stoi( std::string(seqstr,p + 2,strlen(seqstr)) ) ;
		}
		else if(p2 > strlen(seqstr) + 2) {
			
		}

		printf("p2 = %i\n", (int) p);
	}

}
