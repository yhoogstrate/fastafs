#include <iostream>
#include <fstream>


#include "config.hpp"

#include "twobit_byte.hpp"

const char twobit_byte::twobit_hash_decode[256][5] = {"TTTT", "TTTC", "TTTA", "TTTG", "TTCT", "TTCC", "TTCA", "TTCG", "TTAT", "TTAC", "TTAA", "TTAG", "TTGT", "TTGC", "TTGA", "TTGG", "TCTT", "TCTC", "TCTA", "TCTG", "TCCT", "TCCC", "TCCA", "TCCG", "TCAT", "TCAC", "TCAA", "TCAG", "TCGT", "TCGC", "TCGA", "TCGG", "TATT", "TATC", "TATA", "TATG", "TACT", "TACC", "TACA", "TACG", "TAAT", "TAAC", "TAAA", "TAAG", "TAGT", "TAGC", "TAGA", "TAGG", "TGTT", "TGTC", "TGTA", "TGTG", "TGCT", "TGCC", "TGCA", "TGCG", "TGAT", "TGAC", "TGAA", "TGAG", "TGGT", "TGGC", "TGGA", "TGGG", "CTTT", "CTTC", "CTTA", "CTTG", "CTCT", "CTCC", "CTCA", "CTCG", "CTAT", "CTAC", "CTAA", "CTAG", "CTGT", "CTGC", "CTGA", "CTGG", "CCTT", "CCTC", "CCTA", "CCTG", "CCCT", "CCCC", "CCCA", "CCCG", "CCAT", "CCAC", "CCAA", "CCAG", "CCGT", "CCGC", "CCGA", "CCGG", "CATT", "CATC", "CATA", "CATG", "CACT", "CACC", "CACA", "CACG", "CAAT", "CAAC", "CAAA", "CAAG", "CAGT", "CAGC", "CAGA", "CAGG", "CGTT", "CGTC", "CGTA", "CGTG", "CGCT", "CGCC", "CGCA", "CGCG", "CGAT", "CGAC", "CGAA", "CGAG", "CGGT", "CGGC", "CGGA", "CGGG", "ATTT", "ATTC", "ATTA", "ATTG", "ATCT", "ATCC", "ATCA", "ATCG", "ATAT", "ATAC", "ATAA", "ATAG", "ATGT", "ATGC", "ATGA", "ATGG", "ACTT", "ACTC", "ACTA", "ACTG", "ACCT", "ACCC", "ACCA", "ACCG", "ACAT", "ACAC", "ACAA", "ACAG", "ACGT", "ACGC", "ACGA", "ACGG", "AATT", "AATC", "AATA", "AATG", "AACT", "AACC", "AACA", "AACG", "AAAT", "AAAC", "AAAA", "AAAG", "AAGT", "AAGC", "AAGA", "AAGG", "AGTT", "AGTC", "AGTA", "AGTG", "AGCT", "AGCC", "AGCA", "AGCG", "AGAT", "AGAC", "AGAA", "AGAG", "AGGT", "AGGC", "AGGA", "AGGG", "GTTT", "GTTC", "GTTA", "GTTG", "GTCT", "GTCC", "GTCA", "GTCG", "GTAT", "GTAC", "GTAA", "GTAG", "GTGT", "GTGC", "GTGA", "GTGG", "GCTT", "GCTC", "GCTA", "GCTG", "GCCT", "GCCC", "GCCA", "GCCG", "GCAT", "GCAC", "GCAA", "GCAG", "GCGT", "GCGC", "GCGA", "GCGG", "GATT", "GATC", "GATA", "GATG", "GACT", "GACC", "GACA", "GACG", "GAAT", "GAAC", "GAAA", "GAAG", "GAGT", "GAGC", "GAGA", "GAGG", "GGTT", "GGTC", "GGTA", "GGTG", "GGCT", "GGCC", "GGCA", "GGCG", "GGAT", "GGAC", "GGAA", "GGAG", "GGGT", "GGGC", "GGGA", "GGGG"};

//"TTTT" -> 0, "TTTC" -> 1
/*
const std::map<std::string, unsigned char> twobit_hash_encode = {
	{"TTTT", 0}, {"TTTC", 1}, {"TTTA", 2}, {"TTTG", 3}, {"TTCT", 4}, {"TTCC", 5}, {"TTCA", 6}, {"TTCG", 7}, {"TTAT", 8}, {"TTAC", 9}, {"TTAA", 10}, {"TTAG", 11}, {"TTGT", 12}, {"TTGC", 13}, {"TTGA", 14}, {"TTGG", 15}, {"TCTT", 16}, {"TCTC", 17}, {"TCTA", 18}, {"TCTG", 19}, {"TCCT", 20}, {"TCCC", 21}, {"TCCA", 22}, {"TCCG", 23}, {"TCAT", 24}, {"TCAC", 25}, {"TCAA", 26}, {"TCAG", 27}, {"TCGT", 28}, {"TCGC", 29}, {"TCGA", 30}, {"TCGG", 31}, {"TATT", 32}, {"TATC", 33}, {"TATA", 34}, {"TATG", 35}, {"TACT", 36}, {"TACC", 37}, {"TACA", 38}, {"TACG", 39}, {"TAAT", 40}, {"TAAC", 41}, {"TAAA", 42}, {"TAAG", 43}, {"TAGT", 44}, {"TAGC", 45}, {"TAGA", 46}, {"TAGG", 47}, {"TGTT", 48}, {"TGTC", 49}, {"TGTA", 50}, {"TGTG", 51}, {"TGCT", 52}, {"TGCC", 53}, {"TGCA", 54}, {"TGCG", 55}, {"TGAT", 56}, {"TGAC", 57}, {"TGAA", 58}, {"TGAG", 59}, {"TGGT", 60}, {"TGGC", 61}, {"TGGA", 62}, {"TGGG", 63},
	{"CTTT", 64}, {"CTTC", 65}, {"CTTA", 66}, {"CTTG", 67}, {"CTCT", 68}, {"CTCC", 69}, {"CTCA", 70}, {"CTCG", 71}, {"CTAT", 72}, {"CTAC", 73}, {"CTAA", 74}, {"CTAG", 75}, {"CTGT", 76}, {"CTGC", 77}, {"CTGA", 78}, {"CTGG", 79}, {"CCTT", 80}, {"CCTC", 81}, {"CCTA", 82}, {"CCTG", 83}, {"CCCT", 84}, {"CCCC", 85}, {"CCCA", 86}, {"CCCG", 87}, {"CCAT", 88}, {"CCAC", 89}, {"CCAA", 90}, {"CCAG", 91}, {"CCGT", 92}, {"CCGC", 93}, {"CCGA", 94}, {"CCGG", 95}, {"CATT", 96}, {"CATC", 97}, {"CATA", 98}, {"CATG", 99}, {"CACT", 100}, {"CACC", 101}, {"CACA", 102}, {"CACG", 103}, {"CAAT", 104}, {"CAAC", 105}, {"CAAA", 106}, {"CAAG", 107}, {"CAGT", 108}, {"CAGC", 109}, {"CAGA", 110}, {"CAGG", 111}, {"CGTT", 112}, {"CGTC", 113}, {"CGTA", 114}, {"CGTG", 115}, {"CGCT", 116}, {"CGCC", 117}, {"CGCA", 118}, {"CGCG", 119}, {"CGAT", 120}, {"CGAC", 121}, {"CGAA", 122}, {"CGAG", 123}, {"CGGT", 124}, {"CGGC", 125}, {"CGGA", 126}, {"CGGG", 127},
	{"ATTT", 128}, {"ATTC", 129}, {"ATTA", 130}, {"ATTG", 131}, {"ATCT", 132}, {"ATCC", 133}, {"ATCA", 134}, {"ATCG", 135}, {"ATAT", 136}, {"ATAC", 137}, {"ATAA", 138}, {"ATAG", 139}, {"ATGT", 140}, {"ATGC", 141}, {"ATGA", 142}, {"ATGG", 143}, {"ACTT", 144}, {"ACTC", 145}, {"ACTA", 146}, {"ACTG", 147}, {"ACCT", 148}, {"ACCC", 149}, {"ACCA", 150}, {"ACCG", 151}, {"ACAT", 152}, {"ACAC", 153}, {"ACAA", 154}, {"ACAG", 155}, {"ACGT", 156}, {"ACGC", 157}, {"ACGA", 158}, {"ACGG", 159}, {"AATT", 160}, {"AATC", 161}, {"AATA", 162}, {"AATG", 163}, {"AACT", 164}, {"AACC", 165}, {"AACA", 166}, {"AACG", 167}, {"AAAT", 168}, {"AAAC", 169}, {"AAAA", 170}, {"AAAG", 171}, {"AAGT", 172}, {"AAGC", 173}, {"AAGA", 174}, {"AAGG", 175}, {"AGTT", 176}, {"AGTC", 177}, {"AGTA", 178}, {"AGTG", 179}, {"AGCT", 180}, {"AGCC", 181}, {"AGCA", 182}, {"AGCG", 183}, {"AGAT", 184}, {"AGAC", 185}, {"AGAA", 186}, {"AGAG", 187}, {"AGGT", 188}, {"AGGC", 189}, {"AGGA", 190}, {"AGGG", 191},
	{"GTTT", 192}, {"GTTC", 193}, {"GTTA", 194}, {"GTTG", 195}, {"GTCT", 196}, {"GTCC", 197}, {"GTCA", 198}, {"GTCG", 199}, {"GTAT", 200}, {"GTAC", 201}, {"GTAA", 202}, {"GTAG", 203}, {"GTGT", 204}, {"GTGC", 205}, {"GTGA", 206}, {"GTGG", 207}, {"GCTT", 208}, {"GCTC", 209}, {"GCTA", 210}, {"GCTG", 211}, {"GCCT", 212}, {"GCCC", 213}, {"GCCA", 214}, {"GCCG", 215}, {"GCAT", 216}, {"GCAC", 217}, {"GCAA", 218}, {"GCAG", 219}, {"GCGT", 220}, {"GCGC", 221}, {"GCGA", 222}, {"GCGG", 223}, {"GATT", 224}, {"GATC", 225}, {"GATA", 226}, {"GATG", 227}, {"GACT", 228}, {"GACC", 229}, {"GACA", 230}, {"GACG", 231}, {"GAAT", 232}, {"GAAC", 233}, {"GAAA", 234}, {"GAAG", 235}, {"GAGT", 236}, {"GAGC", 237}, {"GAGA", 238}, {"GAGG", 239}, {"GGTT", 240}, {"GGTC", 241}, {"GGTA", 242}, {"GGTG", 243}, {"GGCT", 244}, {"GGCC", 245}, {"GGCA", 246}, {"GGCG", 247}, {"GGAT", 248}, {"GGAC", 249}, {"GGAA", 250}, {"GGAG", 251}, {"GGGT", 252}, {"GGGC", 253}, {"GGGA", 254}, {"GGGG", 255}
};
*//*
const std::map<std::string, unsigned char> twobit_hash_encode = {
	{"0000", 0}, {"0001", 1}, {"0002", 2}, {"0003", 3}, {"0010", 4}, {"0011", 5}, {"0012", 6}, {"0013", 7}, {"0020", 8}, {"0021", 9}, {"0022", 10}, {"0023", 11}, {"0030", 12}, {"0031", 13}, {"0032", 14}, {"0033", 15}, {"0100", 16}, {"0101", 17}, {"0102", 18}, {"0103", 19}, {"0110", 20}, {"0111", 21}, {"0112", 22}, {"0113", 23}, {"0120", 24}, {"0121", 25}, {"0122", 26}, {"0123", 27}, {"0130", 28}, {"0131", 29}, {"0132", 30}, {"0133", 31}, {"0200", 32}, {"0201", 33}, {"0202", 34}, {"0203", 35}, {"0210", 36}, {"0211", 37}, {"0212", 38}, {"0213", 39}, {"0220", 40}, {"0221", 41}, {"0222", 42}, {"0223", 43}, {"0230", 44}, {"0231", 45}, {"0232", 46}, {"0233", 47}, {"0300", 48}, {"0301", 49}, {"0302", 50}, {"0303", 51}, {"0310", 52}, {"0311", 53}, {"0312", 54}, {"0313", 55}, {"0320", 56}, {"0321", 57}, {"0322", 58}, {"0323", 59}, {"0330", 60}, {"0331", 61}, {"0332", 62}, {"0333", 63},
	{"1000", 64}, {"1001", 65}, {"1002", 66}, {"1003", 67}, {"1010", 68}, {"1011", 69}, {"1012", 70}, {"1013", 71}, {"1020", 72}, {"1021", 73}, {"1022", 74}, {"1023", 75}, {"1030", 76}, {"1031", 77}, {"1032", 78}, {"1033", 79}, {"1100", 80}, {"1101", 81}, {"1102", 82}, {"1103", 83}, {"1110", 84}, {"1111", 85}, {"1112", 86}, {"1113", 87}, {"1120", 88}, {"1121", 89}, {"1122", 90}, {"1123", 91}, {"1130", 92}, {"1131", 93}, {"1132", 94}, {"1133", 95}, {"1200", 96}, {"1201", 97}, {"1202", 98}, {"1203", 99}, {"1210", 100}, {"1211", 101}, {"1212", 102}, {"1213", 103}, {"1220", 104}, {"1221", 105}, {"1222", 106}, {"1223", 107}, {"1230", 108}, {"1231", 109}, {"1232", 110}, {"1233", 111}, {"1300", 112}, {"1301", 113}, {"1302", 114}, {"1303", 115}, {"1310", 116}, {"1311", 117}, {"1312", 118}, {"1313", 119}, {"1320", 120}, {"1321", 121}, {"1322", 122}, {"1323", 123}, {"1330", 124}, {"1331", 125}, {"1332", 126}, {"1333", 127},
	{"2000", 128}, {"2001", 129}, {"2002", 130}, {"2003", 131}, {"2010", 132}, {"2011", 133}, {"2012", 134}, {"2013", 135}, {"2020", 136}, {"2021", 137}, {"2022", 138}, {"2023", 139}, {"2030", 140}, {"2031", 141}, {"2032", 142}, {"2033", 143}, {"2100", 144}, {"2101", 145}, {"2102", 146}, {"2103", 147}, {"2110", 148}, {"2111", 149}, {"2112", 150}, {"2113", 151}, {"2120", 152}, {"2121", 153}, {"2122", 154}, {"2123", 155}, {"2130", 156}, {"2131", 157}, {"2132", 158}, {"2133", 159}, {"2200", 160}, {"2201", 161}, {"2202", 162}, {"2203", 163}, {"2210", 164}, {"2211", 165}, {"2212", 166}, {"2213", 167}, {"2220", 168}, {"2221", 169}, {"2222", 170}, {"2223", 171}, {"2230", 172}, {"2231", 173}, {"2232", 174}, {"2233", 175}, {"2300", 176}, {"2301", 177}, {"2302", 178}, {"2303", 179}, {"2310", 180}, {"2311", 181}, {"2312", 182}, {"2313", 183}, {"2320", 184}, {"2321", 185}, {"2322", 186}, {"2323", 187}, {"2330", 188}, {"2331", 189}, {"2332", 190}, {"2333", 191},
	{"3000", 192}, {"3001", 193}, {"3002", 194}, {"3003", 195}, {"3010", 196}, {"3011", 197}, {"3012", 198}, {"3013", 199}, {"3020", 200}, {"3021", 201}, {"3022", 202}, {"3023", 203}, {"3030", 204}, {"3031", 205}, {"3032", 206}, {"3033", 207}, {"3100", 208}, {"3101", 209}, {"3102", 210}, {"3103", 211}, {"3110", 212}, {"3111", 213}, {"3112", 214}, {"3113", 215}, {"3120", 216}, {"3121", 217}, {"3122", 218}, {"3123", 219}, {"3130", 220}, {"3131", 221}, {"3132", 222}, {"3133", 223}, {"3200", 224}, {"3201", 225}, {"3202", 226}, {"3203", 227}, {"3210", 228}, {"3211", 229}, {"3212", 230}, {"3213", 231}, {"3220", 232}, {"3221", 233}, {"3222", 234}, {"3223", 235}, {"3230", 236}, {"3231", 237}, {"3232", 238}, {"3233", 239}, {"3300", 240}, {"3301", 241}, {"3302", 242}, {"3303", 243}, {"3310", 244}, {"3311", 245}, {"3312", 246}, {"3313", 247}, {"3320", 248}, {"3321", 249}, {"3322", 250}, {"3323", 251}, {"3330", 252}, {"3331", 253}, {"3332", 254}, {"3333", 255}
};

*/
void twobit_byte::set(unsigned char bit_offset, unsigned char nucleotide)
{
	// bit_offset must be: {0, 2, 4 or 6};
	// nucleotides must be:
	// => T - 00, C - 01, A - 10, G - 11
	// => T - 00, C -  1, A -  2, G -  3
	
#if DEBUG
	if(bit_offset != 0 and bit_offset != 2 and bit_offset != 4 and bit_offset != 6) {
		throw std::invalid_argument("twobit_byte(bit_offset, ..) must be 0, 2, 4 or 6\n");
	}
#endif //DEBUG
	//set   bit(s): INPUT |= 1 << N;
	//unset bit(s): INPUT &= ~(1 << N);
	switch(nucleotide) {
		case 0://NUCLEOTIDE_T (00)
			this->data = (unsigned char) (this->data & ~(3 << bit_offset));
			break;
		case 1://NUCLEOTIDE_C (01)
			this->data = (unsigned char) (this->data & ~(2 << bit_offset));
			this->data = (unsigned char) (this->data | (1 << bit_offset));
			break;
		case 2://NUCLEOTIDE_A (10)
			this->data = (unsigned char) (this->data & ~(1 << bit_offset));
			this->data = (unsigned char) (this->data | (2 << bit_offset));
			break;
		case 3://NUCLEOTIDE_G (11)
			this->data = (unsigned char) (this->data | (nucleotide << bit_offset));
			break;
#if DEBUG
		default:
			throw std::invalid_argument("twobit_byte::set(,nucleotide) invalid value\n");
			break;
#endif //DEBUG
	}
};



/**
 * @brief fully decodes a twobit byte, not referencing to a hash but allocating a new char*, slower than twobit_byte::get(void) but capable of determining very ends
**/
char *twobit_byte::get(unsigned char length)
{
	char *seq = new char[length + 1];
	
	for(unsigned char i = 0; i < length; i++ ) {// length = 4: i = 0, 1, 2, 3
		seq[i] = twobit_byte::twobit_hash_decode[this->data][i];
	}
	
	seq[length] = '\0';
	return seq;
}



const char *twobit_byte::get()
{
	return twobit_byte::twobit_hash_decode[this->data];
}
