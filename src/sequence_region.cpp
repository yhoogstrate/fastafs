
#include "sequence_region.hpp"



sequence_region::sequence_region(char * seqstr) :
    seq_name(""), has_defined_end(false), start(0), end(0)
{

    parse((const char *) seqstr);// char* can be converted to cost char*, but not vice versa

}

sequence_region::sequence_region(const char * seqstr) :
    seq_name(""), has_defined_end(false), start(0), end(0)
{

    parse(seqstr);

}


void sequence_region::parse(const char * seqstr)
{
    // the + 1 is the also allow parsing "sequence-of-size-255-...-:123-345"
    size_t string_max_pos = std::min(MAX_SIZE_SEQ_NAME + 1, strlen(seqstr));
    size_t p = 0;
    bool proceed = true;
    for(size_t i = 0; i < string_max_pos && proceed == true; i++) {
        if(seqstr[i] == ':') {
            p = i;
            proceed = false;
        }
    }

    if(p > 0) {
        this->seq_name = std::string(seqstr, 0, p);
    } else if(p == false) {
        // either with string > 255 chars or string smaller than 255 without ':'
        this->seq_name = std::string(seqstr, 0, string_max_pos);
    }

    // chr1:1
    // p = 4
    // strlen = 6
    if(proceed == false and strlen(seqstr) > (p + 1)) {
        // we can parse numbers
        // find position of '-' character

        size_t p2 = 0;
        bool proceed2 = true;

        for(size_t i = p; i < strlen(seqstr) && proceed2 == true; i++) {
            if(seqstr[i] == '-') {
                p2 = (size_t) i;
                proceed2 = false;
            }
        }


        if(proceed2 == true) { // chrA:123
            std::string start = std::string(seqstr, p + 1, p2 - p - 1);

            this->start = std::stoi(start);

            this->has_defined_end = true;
            this->end = this->start;
        } else if(p2 == (p + 1)) {// chrA:-123
            std::string end = std::string(seqstr, p2 + 1, strlen(seqstr) - p2 - 1);

            this->start = 0;
            this->end = std::stoi(end) ;

            this->has_defined_end = true;
        } else if(p2 > (p + 1)) { // chrA:123- | chrA:123-456 | chrA:123-456ERR
            if(p2 + 1 == strlen(seqstr)) { // chrA:123-
                std::string start = std::string(seqstr, p + 1, p2 - p - 1);

                this->start = std::stoi(start);
                this->has_defined_end = false;
            } else { // chrA:123-456 | chrA:123-456ERR
                std::string start = std::string(seqstr, p + 1, p2 - p - 1);
                std::string end = std::string(seqstr, p2 + 1, strlen(seqstr) - p2 - 1);


                this->start = std::stoi(start) ;

                this->has_defined_end = true;
                this->end = std::stoi(end) ;
            }
        }

    }

    if(this->has_defined_end and this->start > this->end) {
        throw std::invalid_argument("Invalid region - start larger than end.");
    }
}
