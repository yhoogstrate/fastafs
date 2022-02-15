


#ifndef SEQUENCE_REGION_HPP
#define SEQUENCE_REGION_HPP


#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>


#include "config.hpp"

#include "utils.hpp"



class sequence_region
{
private:

    bool defined_end;//whether the requested region has a defined end position (chr1:1-) has; (chr1:1-2) has not

    off_t start;
    off_t end;

    void parse(const char *);   

    
public:
    sequence_region(char *);
    sequence_region(const char *);

    std::string seq_name;
    //const std::string& get_seq_name() {return seq_name; };
    bool has_defined_end(void) const {return defined_end; };
    off_t get_start_position(void) const {return start; };
    off_t get_end_position(void) const {return end; };
};



#endif


