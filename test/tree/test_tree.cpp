#define BOOST_TEST_MODULE fastfs_tree

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

//#include "fasta_to_fastafs.hpp"
//#include "fastafs.hpp"



BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_finding_long_subseqs)
{
    unsigned int min_subseq_len = 12;
    
    std::string shortest = "ATCGATCGATCGATCGACGTCGTCGACTAGCTAGCTAGCTAGCTAGTCGCTA";
    //                      |....:....|....:....|....:....|....:....|....:....|....:....
    //                                  ***********************
    std::string longest  = "ACGCGCGCGATTATCGACGTCGTCGACTAGCTAGCGGCTTTCGTAAACCCAATCTTTTT";
    
    std::cout << "     " << shortest << "\n";
    
    const unsigned int size_s = shortest.size();
    const unsigned int size_l = longest.size();

    std::vector<unsigned int> previous(size_s, 0);
    std::vector<unsigned int> current(size_s, 0);
    
    //std::vector<unsigned int> *previous = &trace1;
    //std::vector<unsigned int> *current = &trace2;
    
    unsigned int l, s;
    for(l = 0 ; l < size_l; l++) {
        for(s = 0; s < size_s; s++) {
            if(longest[l] == shortest[s]) {
                if(s == 0) {
                    current[0] = 1;
                }
                else {
                    current[s] = previous[s-1] + 1;
                }
            }
            else {
                if(s > 0 and previous[s-1] >= min_subseq_len) {
                    printf("s:[%u, %u] ~ l:[%u,%u]\n",(s-1) - previous[s-1] +1 , s-1, (l-1) - previous[s-1] + 1, l-1);
                    for(unsigned int j = (s-1) - previous[s-1]+1; j <= s-1; j++) {
                        std::cout << shortest[j];
                    }
                    std::cout << "\n";
                    for(unsigned int j = (l-1) - previous[s-1] + 1; j <= l-1; j++) {
                        std::cout << longest[j];
                    }
                    std::cout << "\n";
                    std::cout << "\n";
                    
                }
                current[s] = 0;
            }
        }
        std::swap(previous, current);
        //std::cout << "\n\n";
    }
}




BOOST_AUTO_TEST_SUITE_END()
