#!/usr/bin/env pythn

##shortest = "TAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCTAACCCTAACCCTAACCCTAACCCTAACCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCTAACCCTAACCCTAAACCCTAAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCAACCCCAACCCCAACCCCAACCCCAACCCCAACCCTAACCCCTAACCCTAACCCTAACCCTACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCTAACCCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCTAACCCTAACCCTAACCCTAACCCTCGCGGTACCCTCAGCCGGCCCGCCCGCCCGGGTCTGACCTGAGGAGAACTGTGCTCCGCCTTCAGAGTACCACCGAAATCTGTGCAGAGGACAACGCAGCTCCGCCCTCGCGGTGCTCTCCGGGTCTGTGCTGAGGAGAACGCAACTCCGCCGTTGCAAAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGACACATGCTAGCGCGTCGGGGTGGAGGCGTGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGACACATGCTACCGCGTCCAGGGGTGGAGGCGTGGCGCAGGCGCAGAGAGGCGCACCGCGCCGGCGCAGGCGCAGAGACACATGCTAGCGCGTCCAGGGGTGGAGGCGTGGCGCAGGCGCAGAGACGCAAGCCTACGGGCGGGGGTTGGGGGGGCGTGTGTTGCAGGAGCAAAGTCGCACGGCGCCGGGCTGGGGCGGGGGGAGGGTGGCGCCGTGCACGCGCAGAAACTCACGTCACGGTGGCGCGGCGCAGAGACGGGTAGAACCTCAGTAATCCGAAAAGCCGGGATCGACCGCCCCTTGCTTGCAGCCGGGCACTACAGGACCCGCTTGCTCACGGTGCTGTGCCAGGGCGCCCCCTGCTGGCGACTAGGGCAACTGCAGGGCTCTCTTGCTTAGAGTGGTGGCCAGCGCCCCCTGCTGGCGCCGGGGCACTGCAGGGCCCTCTTGCTTACTGTATAGTGGTGGCACGCCGCCTGCTGGCAGCTAGGGACATTGCAGGGTCCTCTTGCTCAAGGTGTAGTGGCAGCACGCCCACCTGCTGGCAGCTGGGGACACTGCCGGGCCCTCTTGCTCCAACAGTACTGGCGGATTATAGGGAAACACCCGGAGCATATGCTGTTTGGTCTCAGTAGACTCCTAAATATGGGATTCCTGGGTTTAAAAGTAAAAAATAAATATGTTTAATTTGTGAACTGATTACCATCAGAATTGTACTGTTCTGTATCCCACCAGCAATGTCTAGGAATGCCTGTTTCTCCACAAAGTGTTTACTTTTGGATTTTTGCCAGTCTAACAGGTGAAGCCCTGGAGATTCTTATTAGTGATTTGGGCTGGGGCCTGGCCATGTGTATTTTTTTAAATTTCCACTGATGATTTTGCTGCATGGCCGGTGTTGAGAATGACTGCGCAAATTTGCCGGATTTCCTTTGCTGTTCCTGCATGTAGTTTAAACGAGATTGCCAGCACCGGGTATCATTCACCATTTTTCTTTTCGTTAACTTGCCGTCAGCCTTTTCTTTGACCTCTTCTTTCTGTTCATGTGTATTTGCTGTCTCTTAGCCCAGACTTCCCGTGTCCTTTCCACCGGGCCTTTGAGAGGTCACAGGGTCTTGATGCTGTGGTCTTCATCTGCAGGTGTCTGACTTCCAGCAACTGCTGGCCTGTGCCAGGGTGCAAGCTGAGCACTGGAGTGGAGTTTTCCTGTGGAGAGGAGCCATGCCTAGAGTGGGATGGGCCATTGTTCATCTTCTGGCCCCTGTTGTCTGCATGTAACTTAATACCACAACCAGGCATAGGGGAAAGATTGGAGGAAAGATGAGTGAGAGCATCAACTTCTCTCACAACCTAGGCCAGTAAGTAGTGCTTGTGCTCATCTCCTTGGCTGTGATACGTGGCCGGCCCTCGCTCCAGCAGCTGGACCCCTACCTGCCGTCTGCTGCCATCGGAGCCCAAAGCCGGGCTGTGACTGCTCAGACCAGCCGGCTGGAGGGAGGGGCTCAGCAGGTCTGGCTTTGGCCCTGGGAGAGCAGGTGGAAGATCAGGCAGGCCATCGCTGCCACAGAACCCAGTGGATTGGCCTAGGTGGGATCTCTGAGCTCAACAAGCCCTCTCTGGGTGGTAGGTGCAGAGACGGGAGGGGCAGAGCCGCAGGCACAGCCAAGAGGGCTGAAGAAATGGTAGAACGGAGCAGCTGGTGATGTGTGGGCCCACCGGCCCCAGGCTCCTGTCTCCCCCCAGGTGTGTGGTGATGCCAGGCATGCCCTTCCCCAGCATCAGGTCTCCAGAGCTGCAGAAGACGACGGCCGACTTGGATCACACTCTTGTGAGTGTCCCCAGTGTTGCAGAGGTGAGAGGAGAGTAGACAGTGAGTGGGAGTGGCGTCGCCCCTAGGGCTCTACGGGGCCGGCGTCTCCTGTCTCCTGGAGAGGCTTCGATGCCCCTCCACACCCTCTTGATCTTCCCTGTGATGTCATCTGGAGCCCTGCTGCTTGCGGTGGCCTATAAAGCCTCCTAGTCTGGCTCCAAGGCCTGGCAGAGTCTTTCCCAGGGAAAGCTACAAGCAGCAAACAGTCTGCATGGGTCATCCCCTTCACTCCCAGCTCAGAGCCCAGGCCAGGGGCCCCCAAGAAAGGCTCTGGTGGAGAACCTGTGCATGAAGGCTGTCAACCAGTCCATAGGCAAGCCTGGCTGCCTCCAGCTGGGTCGACAGACAGGGGCTGGAGAAGGGGAGAAGAGGAAAGTGAGGTTGCCTGCCCTGTCTCCTACCTGAGGCTGAGGAAGGAGAAGGGGATGCACTGTTGGGGAGGCAGCTGTAACTCAAAGCCTTAGCCTCTGTTCCCACGAAGGCAGGGCCATCAGGCACCAAAGGGATTCTGCCAGCATAGTGCTCCTGGACCAGTGATACACCCGGCACCCTGTCCTGGACACGCTGTTGGCCTGGATCTGAGCCCTGGTGGAGGTCAAAGCCACCTTTGGTTCTGCCATTGCTGCTGTGTGGAAGTTCACTCCTGCCTTTTCCTTTCCCTAGAGCCTCCACCACCCCGAGATCACATTTCTCACTGCCTTTTGTCTGCCCAGTTTCACCAGAAGTAGGCCTCTTCCTGACAGGCAGCTGCACCACTGCCTGGCGCTGTGCCCTTCCTTTGCTCTGCCCGCTGGAGACGGTGTTTGTCATGGGCCTGGTCTGCAGGGATCCTGCTACAAAGGTGAAACCCAGGAGAGTGTGGAGTCCAGAGTGTTGCCAGGACCCAGGCACAGGCATTAGTGCCCGTTGGAGAAAACAGGGGAATCCCGAAGAAATGGTGGGTCCTGGCCATCCGTGAGATCTTCCCAGGGCAGCTCCCCTCTGTGGAATCCAATCTGTCTTCCATCCTGC"
##longest = "GTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCATCGATCGATCGACTACATCGATCGATCGATCGATCGATCGATCGTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCTAACCCTAACCCTAACCCTAACCCTAACCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCTAACCCTAACCCTAAACCCTAAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCAACCCCAACCCCAACCCCAACCCCAACCCCAACCCTAACCCCTAACCCTAACCCTAACCCTACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCTAACCCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCTAACCCCTAACCCTAACCCTAACCCTAACCCTCGCGGTACCCTCAGCCGGCCCGCCCGCCCGGGTCTGACCTGAGGAGAACTGTGCTCCGCCTTCAGAGTACCACCGAAATCTGTGCAGAGGACAACGCAGCTCCGCCCTCGCGGTGCTCTCCGGGTCTGTGCTGAGGAGAACGCAACTCCGCCGTTGCAAAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGACACATGCTAGCGCGTCGGGGTGGAGGCGTGGCGCAGGCGCAGAGAGGCGCGCCGCGCCGGCGCAGGCGCAGAGACACATGCTACCGCGTCCAGGGGTGGAGGCGTGGCGCAGGCGCAGAGAGGCGCACCGCGCCGGCGCAGGCGCAGAGACACATGCTAGCGCGTCCAGGGGTGGAGGCGTGGCGCAGGCGCAGAGACGCAAGCCTACGGGCGGGGGTTGGGGGGGCGTGTGTTGCAGGAGCAAAGTCGCACGGCGCCGGGCTGGGGCGGGGGGAGGGTGGCGCCGTGCACGCGCAGAAACTCACGTCACGGTGGCGCGGCGCAGAGACGGGTAGAACCTCAGTAATCCGAAAAGCCGGGATCGACCGCCCCTTGCTTGCAGCCGGGCACTACAGGACCCGCTTGCTCACGGTGCTGTGCCAGGGCGCCCCCTGCTGGCGACTAGGGCAACTGCAGGGCTCTCTTGCTTAGAGTGGTGGCCAGCGCCCCCTGCTGGCGCCGGGGCACTGCAGGGCCCTCTTGCTTACTGTATAGTGGTGGCACGCCGCCTGCTGGCAGCTAGGGACATTGCAGGGTCCTCTTGCTCAAGGTGTAGTGGCAGCACGCCCACCTGCTGGCAGCTGGGGACACTGCCGGGCCCTCTTGCTCCAACAGTACTGGCGGATTATAGGGAAACACCCGGAGCATATGCTGTTTGGTCTCAGTAGACTCCTAAATATGGGATTCCTGGGTTTAAAAGTAAAAAATAAATATGTTTAATTTGTGAACTGATTACCATCAGAATTGTACTGTTCTGTATCCCACCAGCAATGTCTAGGAATGCCTGTTTCTCCACAAAGTGTTTACTTTTGGATTTTTGCCAGTCTAACAGGTGAAGCCCTGGAGATTCTTATTAGTGATTTGGGCTGGGGCCTGGCCATGTGTATTTTTTTAAATTTCCACTGATGATTTTGCTGCATGGCCGGTGTTGAGAATGACTGCGCAAATTTGCCGGATTTCCTTTGCTGTTCCTGCATGTAGTTTAAACGAGATTGCCAGCACCGGGTATCATTCACCATTTTTCTTTTCGTTAACTTGCCGTCAGCCTTTTCTTTGACCTCTTCTTTCTGTTCATGTGTATTTGCTGTCTCTTAGCCCAGACTTCCCGTGTCCTTTCCACCGGGCCTTTGAGAGGTCACAGGGTCTTGATGCTGTGGTCTTCATCTGCAGGTGTCTGACTTCCAGCAACTGCTGGCCTGTGCCAGGGTGCAAGCTGAGCACTGGAGTGGAGTTTTCCTGTGGAGAGGAGCCATGCCTAGAGTGGGATGGGCCATTGTTCATCTTCTGGCCCCTGTTGTCTGCATGTAACTTAATACCACAACCAGGCATAGGGGAAAGATTGGAGGAAAGATGAGTGAGAGCATCAACTTCTCTCACAACCTAGGCCAGTAAGTAGTGCTTGTGCTCATCTCCTTGGCTGTGATACGTGGCCGGCCCTCGCTCCAGCAGCTGGACCCCTACCTGCCGTCTGCTGCCATCGGAGCCCAAAGCCGGGCTGTGACTGCTCAGACCAGCCGGCTGGAGGGAGGGGCTCAGCAGGTCTGGCTTTGGCCCTGGGAGAGCAGGTGGAAGATCAGGCAGGCCATCGCTGCCACAGAACCCAGTGGATTGGCCTAGGTGGGATCTCTGAGCTCAACAAGCCCTCTCTGGGTGGTAGGTGCAGAGACGGGAGGGGCAGAGCCGCAGGCACAGCCAAGAGGGCTGAAGAAATGGTAGAACGGAGCAGCTGGTGATGTGTGGGCCCACCGGCCCCAGGCTCCTGTCTCCCCCCAGGTGTGTGGTGATGCCAGGCATGCCCTTCCCCAGCATCAGGTCTCCAGAGCTGCAGAAGACGACGGCCGACTTGGATCACACTCTTGTGAGTGTCCCCAGTGTTGCAGAGGTGAGAGGAGAGTAGACAGTGAGTGGGAGTGGCGTCGCCCCTAGGGCTCTACGGGGCCGGCGTCTCCTGTCTCCTGGAGAGGCTTCGATGCCCCTCCACACCCTCTTGATCTTCCCTGTGATGTCATCTGGAGCCCTGCTGCTTGCGGTGGCCTATAAAGCCTCCTAGTCTGGCTCCAAGGCCTGGCAGAGTCTTTCCCAGGGAAAGCTACAAGCAGCAAACAGTCTGCATGGGTCATCCCCTTCACTCCCAGCTCAGAGCCCAGGCCAGGGGCCCCCAAGAAAGGCTCTGGTGGAGAACCTGTGCATGAAGGCTGTCAACCAGTCCATAGGCAAGCCTGGCTGCCTCCAGCTGGGTCGACAGACAGGGGCTGGAGAAGGGGAGAAGAGGAAAGTGAGGTTGCCTGCCCTGTCTCCTACCTGAGGCTGAGGAAGGAGAAGGGGATGCACTGTTGGGGAGGCAGCTGTAACTCAAAGCCTTAGCCTCTGTTCCCACGAAGGCAGGGCCATCAGGCACCAAAGGGATTCTGCCAGCATAGTGCTCCTGGACCAGTGATACACCCGGCACCCTGTCCTGGACACGCTGTTGGCCTGGATCTGAGCCCTGGTGGAGGTCAAAGCCACCTTTGGTTCTGCCATTGCTGCTGTGTGGAAGTTATCGACTAGCTACGACTGAGCTAGCATGCATGCTACGATCGACTGATCGACACTCCTGCCTTTTCCTTTCCCTAGAGCCTCCACCACCCCGAGATCACATTTCTCACTGCCTTTTGTCTGCCCAGTTTCACCAGAAGTAGGCCTCTTCCTGACAGGCAGCTGCACCACTGCCTGGCGCTGTGCCCTTCCTTTGCTCTGCCCGCTGGAGACGGTGTTTGTCATGGGCCTGGTCTGCAGGGATCCTGCTACAAAGGTGAAACCCAGGAGAGTGTGGAGTCCAGAGTGTTGCCAGGACCCAGGCACAGGCATTAGTGCCCGTTGGAGAAAACAGGGGAATCCCGAAGAAATGGTGGGTCCTGGCCATCCGTGAGATCTTCCCAGGGCAGCTCCCCTCTGTGGAATCCAATCTGTCTTCCATCCTGC"
shortest = "GCCTGCTAGCTGCTGCTAGCTAGCTAGCAACGCCGTATCGATCGTCGTGCTAGTAG"
longest = "ATCGATCGATCGATCGACTGCCTGCTAGCTGCTGCTAGCTAGCTAGCA GGCGCGCTAGCATGCATCGATCAGCGCCGTATCGATCGTCGTGCTAGTAGCGATCGA"
##shortest = "AGCCC"
##longest = "aaaAGCCC"




size_s = shortest.size()
size_l = longest.size()

previous = [0] * size_s
current = [0] * size_s

std::vector<> traceback; # can be implemented by much sparser structure since most values are empty
traceback.resize(size_s + 1, (size_l + 1, 0))
#traceback[l][s] = size-of-match ; 
traceback2 = {
#traceback2[{1,2}] = 1


def find_matching_subseqs(shortest, longest):
    l, s = 0, 0
    for l = 0 ; l < size_l; l++:
        for s = 0; s < size_s; s++:
            if longest[l] == shortest[s]:
                if s == 0:
                    current[s] = 1; # als deze 1 is, betekend het dat het matchen 1 positie terug is gestart - s-1 bestaat nog niet
                }
                # de elif hieronder kan slimmer, door nog een losse for-for te maken (nu moet er in de hele loop extra ge-ift worden
                elif l == size_l - 1 and current[s]+1 >= min_subseq_len) # in de laatste rij betekend een mismatch dat beide suffixen matchen
                {
                    printf("s:[%u, %u] ~ l:[%u,%u]  (%u)\n",(s-1) - previous[s-1] +1 , s-1, (l-1) - previous[s-1] + 1, l-1, len            )
                    # assert( (s-1) - ((s-1) - previous[s-1] +1)   ==     (l-1) - ((l-1) - previous[s-1] + 1)   ); # len of both subseqs must be identical
                    traceback[s][l] = previous[s-1] # len
                    traceback2[(s, l)] = previous[s-1] # len
                }
                else:
                    current[s] = previous[s-1] + 1
                }
            }
            else:
                if s > 0 and previous[s-1] >= min_subseq_len:
                    #len = previous[s-1]
                    printf("s:[%u, %u] ~ l:[%u,%u]  (%u)\n",(s-1) - previous[s-1] +1 , s-1, (l-1) - previous[s-1] + 1, l-1, len            )
                    #assert( (s-1) - ((s-1) - previous[s-1] +1)   ==     (l-1) - ((l-1) - previous[s-1] + 1)   ); # len of both subseqs must be identical
                    traceback[s][l] = previous[s-1]
                    traceback2[{s, l}] = previous[s-1]
                    
                    #intervals.push_back(    Interval<std::size_t, int>(s, l, intervals.size() + 1)    )
                    
                    """
                    for j = (s-1) - previous[s-1]+1; j <= s-1; j++:
                        std::cout << shortest[l]
                    }
                    std::cout << "\n"
                    for j = (l-1) - previous[s-1] + 1; j <= l-1; j++:
                        std::cout << longest[l]
                    }
                    std::cout << "\n"
                    std::cout << "\n"
                    """

                current[s] = 0

        #std::swap(previous, current)
        previous, current = current, previous
        #std::cout << "\n\n"
    }




"""
# traceback - or just take the longest, remove anything overlapping the longest, and proceed with the next?
#             -> suboptimal, we can add scores to jumps that represent bytes too
# one jump will cost lets say 40 bytes
jump_penalty = 0; # 
diag, i
height,cut

#printf("%u*%u*2=%u\n",size_s , size_l, size_s * size_l * 2)
#exit(1)

route[size_s][size_l][2]


score0, score1, score2, score3, scoremax

for diag = 1 ; diag < size_l + size_s - 1; diag++:
    height = std::min(diag + 1, size_s)
    if diag < size_l:
        cut = 0
    }
    else:
        cut = diag - size_l + 1
    }
    
    for i = cut;  i < height; i++:
        l = diag - i
        s = i
        
        #printf("diag[%u, %u] -> [%u,%u]\n", diag, i , s , l)
        
        if s == 0:
            traceback[s][l] = 0
            route[s][l][0] = s
            route[s][l][1] = l - 1
            #printf("   boundary 0,-1   ")
        }
        elif l == 0:
            traceback[s][l] = 0
            route[s][l][0] = s - 1
            route[s][l][1] = l
            #printf("   boundary -1,0   ")
        }
        else:
            if traceback[s][l] != 0:
                score0 = traceback[s][l] + traceback[s - traceback[s][l]][l - traceback[s][l]] + jump_penalty
                #printf("     [%u] + [%u] + [0] = %u", traceback[s][l], traceback[s - traceback[s][l]][l - traceback[s][l]], score0)
            }
            else:
                score0 = 0
            }
            
            score1 = traceback[s - 1][l - 1]
            score2 = traceback[s - 1][l]
            score3 = traceback[s][l - 1]
            scoremax  = std::max(std::max(score0, score1) , std::max(score2, score3))
            
            #printf("   max( %u   , %u  , %u)   ", score1, score2, score3)

            if scoremax == score0 and score0 > 0:
                route[s][l][0] = s - traceback[s][l]
                route[s][l][1] = l - traceback[s][l]
                #printf("    match (%u,%u)   ",route[s][l][0], route[s][l][1])
            }
            elif (scoremax == score1:
                route[s][l][0] = s - 1
                route[s][l][1] = l - 1
                #printf("    mm -1,-1   ")
            }
            elif (scoremax == score2:
                route[s][l][0] = s - 1
                route[s][l][1] = l
                #printf("    mm -1,0   ")
            }
            else:
                route[s][l][0] = s
                route[s][l][1] = l - 1
                #printf("    mm 0,-1   ");                
            }
            traceback[s][l] = scoremax
        }
        

        #printf("   = %u\n", traceback[s][l])
        
    }
    #printf("\n")
    /*
    if diag == 2:
        exit(1)
    }
    */
}
# do [0][0] separate

#exit(1)

# traceback
s_tmp, l_tmp, len

#s2* jump
std::vector< s2 > output

s = size_s - 1
l = size_l - 1
while(s != 0 and l != 0:
    s_tmp = route[s][l][0]
    l_tmp = route[s][l][1]
    
    if l - l_tmp > 1 or s - s_tmp > 1)
    {
        len = l - l_tmp
        printf("[%u,%u] -> [%u,%u] *** shared jump ***\n", s, l, route[s][l][0], route[s][l][1])
        #jump = {s_tmp, l_tmp, len}
        output.push_back({s_tmp, l_tmp, len})
    }
    else:
        printf("[%u,%u] -> [%u,%u]\n", s, l, route[s][l][0], route[s][l][1])
    }
    
    l = l_tmp
    s = s_tmp; # otherwise s influences obtaining l
}

printf("[%u] shared regions were found \n", output.size())
"""
