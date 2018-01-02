#include <iostream>
#include <fstream>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"







fasta_to_fastafs_seq::fasta_to_fastafs_seq(void) :
	fastafs_seq(),
	previous_was_N(false),
	N(0)
{
}




void fasta_to_fastafs_seq::add_N()
{
	if(!this->previous_was_N) {
		this->n_starts.push_back(this->n);
	}
	
	this->previous_was_N = true;
	this->n++;
	this->N++;
}




std::map<std::string, unsigned char> mymap = {
	{"0000", 0}, {"0001", 1}, {"0002", 2}, {"0003", 3}, {"0010", 4}, {"0011", 5}, {"0012", 6}, {"0013", 7}, {"0020", 8}, {"0021", 9}, {"0022", 10}, {"0023", 11}, {"0030", 12}, {"0031", 13}, {"0032", 14}, {"0033", 15}, {"0100", 16}, {"0101", 17}, {"0102", 18}, {"0103", 19}, {"0110", 20}, {"0111", 21}, {"0112", 22}, {"0113", 23}, {"0120", 24}, {"0121", 25}, {"0122", 26}, {"0123", 27}, {"0130", 28}, {"0131", 29}, {"0132", 30}, {"0133", 31}, {"0200", 32}, {"0201", 33}, {"0202", 34}, {"0203", 35}, {"0210", 36}, {"0211", 37}, {"0212", 38}, {"0213", 39}, {"0220", 40}, {"0221", 41}, {"0222", 42}, {"0223", 43}, {"0230", 44}, {"0231", 45}, {"0232", 46}, {"0233", 47}, {"0300", 48}, {"0301", 49}, {"0302", 50}, {"0303", 51}, {"0310", 52}, {"0311", 53}, {"0312", 54}, {"0313", 55}, {"0320", 56}, {"0321", 57}, {"0322", 58}, {"0323", 59}, {"0330", 60}, {"0331", 61}, {"0332", 62}, {"0333", 63},
	{"1000", 64}, {"1001", 65}, {"1002", 66}, {"1003", 67}, {"1010", 68}, {"1011", 69}, {"1012", 70}, {"1013", 71}, {"1020", 72}, {"1021", 73}, {"1022", 74}, {"1023", 75}, {"1030", 76}, {"1031", 77}, {"1032", 78}, {"1033", 79}, {"1100", 80}, {"1101", 81}, {"1102", 82}, {"1103", 83}, {"1110", 84}, {"1111", 85}, {"1112", 86}, {"1113", 87}, {"1120", 88}, {"1121", 89}, {"1122", 90}, {"1123", 91}, {"1130", 92}, {"1131", 93}, {"1132", 94}, {"1133", 95}, {"1200", 96}, {"1201", 97}, {"1202", 98}, {"1203", 99}, {"1210", 100}, {"1211", 101}, {"1212", 102}, {"1213", 103}, {"1220", 104}, {"1221", 105}, {"1222", 106}, {"1223", 107}, {"1230", 108}, {"1231", 109}, {"1232", 110}, {"1233", 111}, {"1300", 112}, {"1301", 113}, {"1302", 114}, {"1303", 115}, {"1310", 116}, {"1311", 117}, {"1312", 118}, {"1313", 119}, {"1320", 120}, {"1321", 121}, {"1322", 122}, {"1323", 123}, {"1330", 124}, {"1331", 125}, {"1332", 126}, {"1333", 127},
	{"2000", 128}, {"2001", 129}, {"2002", 130}, {"2003", 131}, {"2010", 132}, {"2011", 133}, {"2012", 134}, {"2013", 135}, {"2020", 136}, {"2021", 137}, {"2022", 138}, {"2023", 139}, {"2030", 140}, {"2031", 141}, {"2032", 142}, {"2033", 143}, {"2100", 144}, {"2101", 145}, {"2102", 146}, {"2103", 147}, {"2110", 148}, {"2111", 149}, {"2112", 150}, {"2113", 151}, {"2120", 152}, {"2121", 153}, {"2122", 154}, {"2123", 155}, {"2130", 156}, {"2131", 157}, {"2132", 158}, {"2133", 159}, {"2200", 160}, {"2201", 161}, {"2202", 162}, {"2203", 163}, {"2210", 164}, {"2211", 165}, {"2212", 166}, {"2213", 167}, {"2220", 168}, {"2221", 169}, {"2222", 170}, {"2223", 171}, {"2230", 172}, {"2231", 173}, {"2232", 174}, {"2233", 175}, {"2300", 176}, {"2301", 177}, {"2302", 178}, {"2303", 179}, {"2310", 180}, {"2311", 181}, {"2312", 182}, {"2313", 183}, {"2320", 184}, {"2321", 185}, {"2322", 186}, {"2323", 187}, {"2330", 188}, {"2331", 189}, {"2332", 190}, {"2333", 191},
	{"3000", 192}, {"3001", 193}, {"3002", 194}, {"3003", 195}, {"3010", 196}, {"3011", 197}, {"3012", 198}, {"3013", 199}, {"3020", 200}, {"3021", 201}, {"3022", 202}, {"3023", 203}, {"3030", 204}, {"3031", 205}, {"3032", 206}, {"3033", 207}, {"3100", 208}, {"3101", 209}, {"3102", 210}, {"3103", 211}, {"3110", 212}, {"3111", 213}, {"3112", 214}, {"3113", 215}, {"3120", 216}, {"3121", 217}, {"3122", 218}, {"3123", 219}, {"3130", 220}, {"3131", 221}, {"3132", 222}, {"3133", 223}, {"3200", 224}, {"3201", 225}, {"3202", 226}, {"3203", 227}, {"3210", 228}, {"3211", 229}, {"3212", 230}, {"3213", 231}, {"3220", 232}, {"3221", 233}, {"3222", 234}, {"3223", 235}, {"3230", 236}, {"3231", 237}, {"3232", 238}, {"3233", 239}, {"3300", 240}, {"3301", 241}, {"3302", 242}, {"3303", 243}, {"3310", 244}, {"3311", 245}, {"3312", 246}, {"3313", 247}, {"3320", 248}, {"3321", 249}, {"3322", 250}, {"3323", 251}, {"3330", 252}, {"3331", 253}, {"3332", 254}, {"3333", 255}
};

void fasta_to_fastafs_seq::add_nucleotide(unsigned char nucleotide)
{
	this->tmp_str.push_back(nucleotide);

	if(tmp_str.size() >= 4) {
		//std::map<std::string, unsigned char>::iterator it = mymap.find(tmp_str);
		
		unsigned char encoded = mymap.find(tmp_str)->second;
		//printf("[%s] -> [%u]\n", tmp_str.c_str(), encoded);
		tmp_str = "";
		
		this->twobits.push_back(encoded);
		//printf("[%s] -> [%u]  {%u}\n", tmp_str.c_str(), encoded, this->twobits.size());
	}
	/*
	*/



/*
	unsigned char offset = 6 -(2* ((this->n - this->N) % 4) ) ;
	
	this->twobit_data.set(offset, nucleotide);
	if(offset == 0) {
		this->twobits.push_back(this->twobit_data.data);
	}
	*/
	
	// proceed as normal
	if(this->previous_was_N) {
		this->n_ends.push_back(this->n - 1);
	}
	
	this->previous_was_N = false;
	this->n++;
}


/**
 * @brief MAY ONLY BE USED TO INSERT FULL TWOBIT BYTES REPRESENTING FOUR CHARS!
 *  otherwise use add_twobit_sticky_end(twobit_byte& tb, i < 4);
 */
void fasta_to_fastafs_seq::add_twobit(twobit_byte &tb)
{
	this->twobits.push_back(tb.data);
	this->n++;
}


void fasta_to_fastafs_seq::close_reading()
{
	unsigned char sticky_end = (this->n - this->N) % 4;
	if(sticky_end != 0) { // sticky end
		signed int i;
		// if i = 1, last three 2bits need to be set to 00
		// 3 - i = 2,
		for(i = 3 - (signed int) sticky_end; i >= 0; i--) {
			this->twobit_data.set((unsigned char) (i * 2), 0);
		}
		
		// insert sticky-ended twobit
		this->twobits.push_back(this->twobit_data.data);
	}
	
	if(this->previous_was_N) {
		this->n_ends.push_back(this->n - 1);
	}
}




/*
void fasta_to_fastafs_seq::print(void)
{
    if(this->n_starts.size() != this->n_ends.size()) {
        throw std::invalid_argument("unequal number of start and end positions for N regions\n");
    }

    bool in_N = false;
    twobit_byte t = twobit_byte();
    unsigned int i;

    printf(">%s (size=%i [ACTG: %i, N: %i], compressed ACTG=%i)\n", this->name.c_str(), this->n, this->n - this->N, this->N, (unsigned int) this->size());

    printf("\nN[s]: ");
    for(i = 0; i < this->n_starts.size(); i++) {
        printf("%i\t", this->n_starts[i]);
    }
    printf("\nN[e]: ");
    for(i = 0; i < this->n_ends.size(); i++) {
        printf("%i\t", this->n_ends[i]);
    }
    printf("\n----------------------\n");

    unsigned int i_n_start = 0;//@todo make iterator
    unsigned int i_n_end = 0;//@todo make iterator
    unsigned int i_in_seq = 0;
    unsigned int chunk_offset;
    const char *chunk;

    for(i = 0; i < this->n; i++) {
        if(i % 4 == 0 and i != 0) {
            printf("\n");
        }
        printf("%i\t", i);



        if(this->n_starts.size() > i_n_start and i == this->n_starts[i_n_start]) {
            in_N = true;
        }

        if(in_N) {
            printf("N\n");

            if(i == this->n_ends[i_n_end]) {
                i_n_end++;
                in_N = false;
            }
        } else {
            // load new twobit chunk when needed
            chunk_offset = i_in_seq % 4;
            if(chunk_offset == 0) {
                t.data = this->twobits[i_in_seq / 4];
                chunk = t.get();
            }
            printf("%c\n", chunk[chunk_offset]);

            i_in_seq++;
        }
    }
    printf("\n\n");
}
*/

size_t fasta_to_fastafs_seq::size(void)
{
	return this->twobits.size();
}






// is resource by filename required?
// not yet, maybe other classes or empty?
fasta_to_fastafs::fasta_to_fastafs(std::string name, std::string fname)
{
	this->name = name;
	this->filename = fname;
}




fasta_to_fastafs::~fasta_to_fastafs()
{
	for(unsigned int i = 0; i < this->data.size(); i++) {
		delete this->data[i];
	}
}

int fasta_to_fastafs::cache(void)
{
	fasta_to_fastafs_seq *s = nullptr;
	
	std::string line;
	std::ifstream myfile (this->filename.c_str());
	std::string sequence = "";
	
	
	if (myfile.is_open()) {
		while(getline(myfile, line)) {
			if (line[0] == '>') {
				if(s != nullptr) {
					s->close_reading();
					
					this->data.push_back(s);
					s = nullptr;
				}
				
				line.erase(0, 1);// erases first part, quicker would be pointer from first char
				s = new fasta_to_fastafs_seq();
				s->name = line;
			} else {
			
				for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
					switch(*it) {
						case 't':
						case 'T':
						case 'u':
						case 'U':
							s->add_nucleotide('0');
							break;
						case 'c':
						case 'C':
							s->add_nucleotide('1');
							break;
						case 'a':
						case 'A':
							s->add_nucleotide('2');
							break;
						case 'g':
						case 'G':
							s->add_nucleotide('3');
							break;
						case 'n':
						case 'N':
							s->add_N();
							break;
						default:
							std::cerr << "invalid chars in FASTA file" << std::endl;
							exit(1);
							break;
					}
				}
			}
		}
		myfile.close();
	} else {
		throw std::runtime_error("could not cache fasta file: " + this->filename);
	}
	
	if(s != nullptr) {
		s->close_reading();
		
		this->data.push_back(s);
		s = nullptr;
	}
	return 0;
}


/*
void fasta_to_fastafs::print(void)
{
    for(unsigned int i = 0; i < this->data.size(); i++) {
        this->data[i]->print();
    }
}
*/

unsigned int fasta_to_fastafs::get_index_size()
{
	unsigned int n = (unsigned int) this->data.size() * 5;// one byte to describe each size + 32bits to describe offset
	
	for(unsigned int i = 0; i < this->data.size(); i++) {
		n += (unsigned int) this->data[i]->name.size();
	}
	
	return n;
}

unsigned int fasta_to_fastafs::get_sequence_offset(unsigned int sequence)
{
	unsigned int n = 4 + 4 + 4 + 4 + this->get_index_size();
	
	for(unsigned int i = 0; i < sequence; i++) {
		n += 4; // dna_size
		n += 4; // n_block_count
		n += (unsigned int) this->data[i]->n_starts.size() * 4 * 2;//nBlockStarts + nBlockSizes
		n += 4;//maskBlockCount
		n += 0;//maskBlockStarts
		n += 0;//maskBlockSizes
		//n += 4;//reserved
		n += (unsigned int) this->data[i]->size();//packedDna
		
	}
	
	return n;
}



// closely similar to: http://genome.ucsc.edu/FAQ/FAQformat.html#format7
void fasta_to_fastafs::write(std::string filename)
{
	std::fstream twobit_out_stream (filename.c_str(), std::ios :: out | std::ios :: binary);
	
	unsigned int four_bytes;
	unsigned char byte;
	
	char ch1[] = TWOBIT_MAGIC;
	twobit_out_stream.write(reinterpret_cast<char *> (&ch1), (size_t) 4);
	
	char ch2[] = TWOBIT_VERSION;
	twobit_out_stream.write(reinterpret_cast<char *> (&ch2), (size_t) 4);
	
	//four_bytes = (unsigned int) this->data.size();
	//unsigned int n = (unsigned int) this->data.size();
	
	char ch3[4];
	uint_to_fourbytes(ch3, (unsigned int) this->data.size());
	twobit_out_stream.write(reinterpret_cast<char *> (&ch3), (size_t) 4);
	
	char ch4[] = "\0\0\0\0";
	twobit_out_stream.write(reinterpret_cast<char *> (&ch4), (size_t) 4);
	
	// write indices
	for(unsigned int i = 0; i < this->data.size(); i++) {
		byte = (unsigned char) this->data[i]->name.size();
		twobit_out_stream.write((char *) &byte, (size_t) 1);
		
		for(unsigned int j = 0; j < this->data[i]->name.size(); j++) {
			byte = (unsigned char) this->data[i]->name[j];
			twobit_out_stream.write((char *) &byte, (size_t) 1);
		}
		
		uint_to_fourbytes(ch3, this->get_sequence_offset(i));
		twobit_out_stream.write(reinterpret_cast<char *> (&ch3), (size_t) 4);
	}
	
	// write data
	for(unsigned int i = 0; i < this->data.size(); i++) {
		//s->n
		uint_to_fourbytes(ch3, this->data[i]->n);
		twobit_out_stream.write(reinterpret_cast<char *> (&ch3), (size_t) 4);
		
		//s-N
		uint_to_fourbytes(ch3, (unsigned int )this->data[i]->n_starts.size());
		twobit_out_stream.write(reinterpret_cast<char *> (&ch3), (size_t) 4);
		
		//s->n_starts
		for(unsigned int j = 0; j < this->data[i]->n_starts.size(); ++j) {
			uint_to_fourbytes(ch3, this->data[i]->n_starts[j]);
			twobit_out_stream.write(reinterpret_cast<char *> (&ch3), (size_t) 4);
		}
		
		//s->n_ends
		for(unsigned int j = 0; j < this->data[i]->n_ends.size(); ++j) {
			uint_to_fourbytes(ch3, this->data[i]->n_ends[j]);
			twobit_out_stream.write(reinterpret_cast<char *> (&ch3), (size_t) 4);
		}
		
		//s->M
		four_bytes = (unsigned int) 0;
		twobit_out_stream.write( reinterpret_cast<char *>(&four_bytes), 4 );
		
		// @todo this->data[i].write_twobit_data(fstream);
		for(unsigned int j = 0; j < this->data[i]->twobits.size(); ++j) {
			twobit_out_stream.write((char *) &this->data[i]->twobits[j], (size_t) 1);
		}
		
		/*
		// this is slower for some reason
		for(std::vector<unsigned char>::iterator j = this->data[i]->twobits.begin(); j != this->data[i]->twobits.end(); ++j) {
			twobit_out_stream.write((char *)&(*j), (size_t) 1);
		}
		*/
	}
	
	twobit_out_stream.close();
}
