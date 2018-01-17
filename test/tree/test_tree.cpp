#define BOOST_TEST_MODULE fastfs_tree

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

//#include "fasta_to_fastafs.hpp"
//#include "fastafs.hpp"



BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_finding_long_subseqs)
{
    int min_subseq_len = 12;
    
    std::string shortest = "ATCGATCGATCGATCGACGTCGTCGACTAGCTAGCTAGCTAGCTAGTCGCTA";
    std::string longest  = "GCGCGCGCGATTATCGACGTCGTCGACTAGCTAGCGGCTTTCGTAAACCCAATCTTTTT";
    
    std::cout << "    " << shortest << "\n";
    
    unsigned int size_s = shortest.size();
    unsigned int size_l = longest.size();
    
    unsigned int l, s;
    for(l = 0 ; l < size_l; l++) {
        std::cout << longest[l] << "   ";
        for(s = 0; s < size_s; s++) {
            if(longest[l] == shortest[s]) {
                std::cout << '*';
            }
            else {
                std::cout << '-';
            }
        }
        std::cout << "\n";
    }
}




BOOST_AUTO_TEST_SUITE_END()
