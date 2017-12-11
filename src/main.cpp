
#include <iostream>
#include <array>
#include <vector>
#include <string.h>


#include "config.hpp"
#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"


// https://github.com/facebook/zstd/issues/521
// https://github.com/samtools/samtools/blob/develop/faidx.c


void usage(char **argv)
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
            std::cout << PACKAGE << " v" << PACKAGE_VERSION << GIT_SHA1_STRING << "\n\n";
            std::cout << "Copyright (C) 2017 Youri Hoogstrate." << "\n";
            std::cout << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n";
            std::cout << "This is free software: you are free to change and redistribute it.\n";
            std::cout << "There is NO WARRANTY, to the extent permitted by law.\n\n";
            std::cout << "The " << PACKAGE << " package is writting by Youri Hoogstrate.\n";

            exit(0);
        } else if (strcmp(argv[1], "cache") == 0) {
            if(argc > 2) {
                std::string fname = "test/cache/test.fa";
                fasta_to_fastafs f = fasta_to_fastafs(&fname);
                f.cache();
                f.write("test.2bit");
                // @ todo progress bar
            } else {
                std::cout << "usage: " << PACKAGE << " cache -n hg38 test.fa\n\n";
                std::cout << "    -n, --name       Name of reference file (required)\n";
                std::cout << "\n";
            }
        } else if (strcmp(argv[1], "view") == 0) {
            if(argc > 2) {
                std::string fname = std::string(argv[argc - 1]);
                fastafs f = fastafs();
                f.load(&fname);
                f.view(60);
            } else {
                std::cout << "usage: " << PACKAGE << " view [<options>] test.fastafs\n\n";
                std::cout << "    -p, --padding    width of nucleotide lines (int > 0; default=60)\n";
                std::cout << "\n";
            }
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

