
#include <iostream>
#include <array>
#include <vector>
#include <string.h>


#include "config.hpp"
#include "fasta_to_fastafs.hpp"
#include "database.hpp"


// https://github.com/facebook/zstd/issues/521
// https://github.com/samtools/samtools/blob/develop/faidx.c


void usage(char **argv)
{
    std::cout << "usage: " << PACKAGE << " [--version] [--help]" << std::endl << std::endl;
    std::cout <<      "    list       overview of FASTAFS database" << std::endl;
    std::cout <<      "    cache      adds FASTA file to cache" << std::endl;
    std::cout <<      "    view       view FASTAFS as FASTA" << std::endl;
    std::cout <<      "    info       view FASTAFS information" << std::endl;
    std::cout << std::endl;
}


int main(int argc, char *argv[])
{
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 or strcmp(argv[1], "-h") == 0) {
            usage(argv);
        } else if (strcmp(argv[1], "--version") == 0) {
            std::cout << PACKAGE << " v" << PACKAGE_VERSION << GIT_SHA1_STRING << "\n\n";
            std::cout << "Copyright (C) 2017 Youri Hoogstrate." << "\n";
            std::cout << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n";
            std::cout << "This is free software: you are free to change and redistribute it.\n";
            std::cout << "There is NO WARRANTY, to the extent permitted by law.\n\n";
            std::cout << "The " << PACKAGE << " package is written by Youri Hoogstrate.\n";

            exit(0);
        } else if (strcmp(argv[1], "cache") == 0) {
            if(argc > 3) {
                std::string fname = "test/cache/test.fa";
                fasta_to_fastafs f = fasta_to_fastafs(&fname);
                f.cache();
                database d = database();
                std::string fname_out = d.add(argv[2]);
                f.write(fname_out);
                // @ todo progress bar
            } else {
                std::cout << "usage: " << PACKAGE << " cache hg38 /tmp/hg38_test.fa\n\n";
                std::cout << "\n";
            }
        } else if (strcmp(argv[1], "view") == 0) {
            if(argc > 2) {
                bool from_file = false;
                
                for(int i = 2; i < argc - 1; i++) {
                    if (strcmp(argv[i], "-f") == 0) {
                        from_file = true;
                    }
                }
                
                std::string fname;
                if(from_file) {
                    fname = std::string(argv[argc - 1]);
                }
                else {
                    database d = database();
                    fname = d.get(argv[argc - 1]);
                    if(fname.size() == 0) {
                        std::cout << "Invalid FASTAFS requested\n";
                        exit(1);
                    }
                }
                
                fastafs f = fastafs();
                f.load(fname);
                f.view(60);
            }
        } else if (strcmp(argv[1], "info") == 0) {
            if(argc > 2) {
                bool from_file = false;
                
                for(int i = 2; i < argc - 1; i++) {
                    if (strcmp(argv[i], "-f") == 0) {
                        from_file = true;
                    }
                }
                
                std::string fname;
                if(from_file) {
                    fname = std::string(argv[argc - 1]);
                }
                else {
                    database d = database();
                    fname = d.get(argv[argc - 1]);
                    if(fname.size() == 0) {
                        std::cout << "Invalid FASTAFS requested\n";
                        exit(1);
                    }
                }
                
                fastafs f = fastafs();
                f.load(fname);
                f.info();
            } else {
                std::cout << "usage: " << PACKAGE << " info [<options>] fastafs-name-or-id\n\n";
                std::cout << "    -f               use filename instead  of name or ID\n";
                std::cout << "\n";
            }
        } else if (strcmp(argv[1], "list") == 0) {
            database d = database();
            d.list();
        } else {
            std::cerr << PACKAGE << ": '" << argv[1] << "' is not a " << PACKAGE <<" command. See '" << PACKAGE <<" --help':" << std::endl<< std::endl;
            usage(argv);
            return 1;
        }
    } else {
        usage(argv);
        return 1;
    }



    return 0;
}

