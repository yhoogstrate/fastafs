
#include <iostream>
#include <array>
#include <vector>
#include <string.h>


//#include "include/fasta_index.hpp"
//#include "include/fasta.hpp"

#include "config.hpp"
#include "fasta.hpp"


// https://github.com/facebook/zstd/issues/521
// https://github.com/samtools/samtools/blob/develop/faidx.c


void usage(char** argv)
{
    std::cout << "usage: " << PACKAGE << " [--version] [--help]" << std::endl << std::endl
              << "  " << PACKAGE << " cache      adds FASTA file to cache" << std::endl;
}


int main(int argc, char *argv[])
{
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0) {
            usage(argv);
        } else if (strcmp(argv[1], "--version") == 0) {
            std::cout << PACKAGE << " v" << PACKAGE_VERSION << GIT_SHA1_STRING << std::endl;
            exit(0);
        } else if (strcmp(argv[1], "cache") == 0) {

            std::string fname = "test/cache/test.fa";
            fasta f = fasta(&fname);
            printf("[%i]", f.cache());

            std::cout << "usage: fastafs cache -n hg38 test.fa";
        } else {
            std::cerr << "Invalid 1st argument given" << std::endl;
            return 1;
        }
    } else {
        usage(argv);
        return 1;
    }



    return 0;
}

