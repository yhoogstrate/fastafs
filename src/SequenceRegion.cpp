
#include "SequenceRegion.hpp"



SequenceRegion::SequenceRegion(char * seqstr) :
    seq_name("") , has_defined_end(false), start(0), end(0) {

    parse(seqstr);

}


void SequenceRegion::parse(char * seqstr) {
    // the + 1 is the also allow parsing "sequence-of-size-255-...-:123-345"
    size_t string_max_pos = std::min(MAX_SIZE_SEQ_NAME + 1, strlen(seqstr));
    ssize_t p = -1;
    for(size_t i = 0; i < string_max_pos && p == -1; i++) {
        if(seqstr[i] == ':') {
            p = (size_t) i;
        }
    }
    
    if(p > 0) {
        this->seq_name = std::string(seqstr , 0 , p);
    } else if(p == -1) {
        
        // either with string > 255 chars or string smaller than 255 without ':'
        this->seq_name = std::string(seqstr , 0 , string_max_pos);
    }
    
    // chr1:1
    // p = 4
    // strlen = 6
    if(p != -1 and strlen(seqstr) > (p + 1)) {
        // we can parse numbers
        // find position of '-' character

        ssize_t p2 = -1;

        for(size_t i = p; i < strlen(seqstr) && p2 == -1; i++) {
            if(seqstr[i] == '-') {
                p2 = (size_t) i;
            }
        }

        
        if(p2 == -1) { // chrA:123
            std::string start = std::string(seqstr,p + 1,p2 - p - 1);

            this->start = std::stoi( start );

            this->has_defined_end = true;
            this->end = this->start;
        } else if(p2 == (p + 1)) {// chrA:-123
            std::string end = std::string(seqstr,p2 + 1,strlen(seqstr) - p2 - 1);

            this->start = 0;
            this->end = std::stoi( end ) ;

            this->has_defined_end = true;
        } else if(p2 > (p + 1)) { // chrA:123- | chrA:123-456 | chrA:123-456ERR
            if(p2 + 1 == strlen(seqstr)) { // chrA:123-
                std::string start = std::string(seqstr,p + 1,p2 - p - 1);
                
                this->start = std::stoi(start);
                this->has_defined_end = false;
            } else { // chrA:123-456 | chrA:123-456ERR
                std::string start = std::string(seqstr,p + 1,p2 - p - 1);
                std::string end = std::string(seqstr,p2 + 1,strlen(seqstr) - p2 - 1);


                this->start = std::stoi( start ) ;

                this->has_defined_end = true;
                this->end = std::stoi( end ) ;
            }
        }

    }

    if(this->has_defined_end and this->start > this->end) {
        throw std::invalid_argument("Invalid region - start larger than end.");
    }
}
