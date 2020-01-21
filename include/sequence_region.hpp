#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>


#include "config.hpp"

#include "utils.hpp"



class sequence_region
{
public:
    sequence_region(char *);
    sequence_region(const char *);

    std::string seq_name;

    bool has_defined_end;

    off_t start;
    off_t end;

private:
    void parse(const char *);
};

