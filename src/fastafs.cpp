
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <openssl/sha.h>

#include "config.hpp"

#include "twobit_byte.hpp"
#include "fastafs.hpp"



fastafs_seq::fastafs_seq(): n(0)
{
}

void fastafs_seq::view_fasta(unsigned int padding, std::ifstream *fh)
{
#if DEBUG
	if(this->n_starts.size() != this->n_ends.size()) {
		throw std::invalid_argument("unequal number of start and end positions for N regions\n");
	}
#endif //DEBUG
	
	printf(">");
	std::cout << this->name << "\n";
	
	char *byte_tmp = new char [4];
	unsigned int chunk_offset;
	const char *chunk;
	
	bool in_N = false;
	twobit_byte t = twobit_byte();
	unsigned int i_n_start = 0;//@todo make iterator
	unsigned int i_n_end = 0;//@todo make iterator
	unsigned int i_in_seq = 0;
	unsigned int i;
	unsigned int modulo = padding - 1;
	
	//@todo create func this->get_offset_2bit_data();
	fh->seekg ((unsigned int) this->data_position + 4 + 4 + 4 + (this->n_starts.size() * 8), fh->beg);
	for(i = 0; i < this->n; i++) {
	
	
		if(this->n_starts.size() > i_n_start and i == this->n_starts[i_n_start]) {
			in_N = true;
		}
		
		if(in_N) {
			std::cout << "N";
			
			if(i == this->n_ends[i_n_end]) {
				i_n_end++;
				in_N = false;
			}
		} else {
			// load new twobit chunk when needed
			chunk_offset = i_in_seq % 4;
			if(chunk_offset == 0) {
			
				fh->read(byte_tmp, 1);
				t.data = byte_tmp[0];
				chunk = t.get();
			}
			std::cout << chunk[chunk_offset];
			
			i_in_seq++;
		}
		
		if(i % padding == modulo) {
			std::cout << "\n";
		}
	}
	if(i % padding != 0) {
		std::cout << "\n";
	}
	
	delete[] byte_tmp;
}




int fastafs_seq::view_fasta_chunk(unsigned int padding, char *buffer, off_t start_pos_in_fasta, size_t len_to_copy, std::ifstream *fh)
{
	unsigned int i;
	unsigned int written = 0;
	
	// then close line
	if( start_pos_in_fasta == 0 and written < len_to_copy) {
		buffer[written++] = '>';
	}
	
	
	for(i = (unsigned int) start_pos_in_fasta + written - 1; i < (unsigned int) this->name.size() and written < len_to_copy; i++) {
		buffer[written++] = this->name[i];
	}
	
	if(start_pos_in_fasta < (unsigned int) this->name.size() + 2 and written < len_to_copy) {
		buffer[written++] = '\n';
	}
	
	
	char *byte_tmp = new char [4];
	const char *chunk;
	twobit_byte t = twobit_byte();
	unsigned int chunk_offset;
	unsigned int i_n_end = 0;
	unsigned int i_n_start = 0;
	unsigned int num_paddings = (this->n + padding - 1) / padding;
	
	// 1. zoek nucleotide om te beginnen a.d.h.v. start_pos + copy len (minus geschreven header lengte)
	unsigned int i_in_file = (written +  (unsigned int)  start_pos_in_fasta) - ( (unsigned int)  this->name.size() + 2); // how many'th char after ">header\n"
	//printf("\toffsets in file [ACTG N \\n]: (%u)\n", i_in_file);
	
	
	unsigned int removal_pre = fastafs_seq::n_padding(0, i_in_file - 1, padding);// het aantal N's VOOR deze positie
	unsigned int start_nucleotide = i_in_file - removal_pre;
	//printf("\tnucleotides in file [ACTG N]: {-%u} => (%u)\n",removal_pre, start_nucleotide);
	
	
	unsigned int ns_until_start;
	bool in_N = this->get_n_offset(start_nucleotide, &ns_until_start);
	unsigned int i_in_seq = start_nucleotide - ns_until_start;
	//printf("\tACTG nucleotides until start nuc [ACTG]: {%u - %u} = (%u)\n",start_nucleotide, ns_until_start, i_in_seq);
	unsigned int twobit_offset = i_in_seq / 4;
	
	
	// 2. subtract aantal N's van start pos & set is_N: bepaal 2bit & zet file allocatie goed
	
	fh->seekg ((unsigned int) this->data_position + 4 + 4 + 4 + (this->n_starts.size() * 8) + twobit_offset, fh->beg);
	i = start_nucleotide;              // pos in nucleotides ACTG N
	
	// 3. gaan met die loop
	
	chunk_offset = i_in_seq % 4;// load first twobit chunk [when needed]
	if(chunk_offset != 0) {
		fh->read(byte_tmp, 1);
		t.data = byte_tmp[0];
		chunk = t.get();
	}
	
	
	for(; written < len_to_copy; i_in_file++) {
		if((i_in_file % (padding + 1) == padding) or (i_in_file == this->n + num_paddings - 1)) {
			buffer[written++] = '\n';
		} else {
			if(this->n_starts.size() > i_n_start and i == this->n_starts[i_n_start]) {
				in_N = true;
			}
			if(in_N) {
				buffer[written++] = 'N';
				
				if(i == this->n_ends[i_n_end]) {
					i_n_end++;
					in_N = false;
				}
			} else {
				chunk_offset = i_in_seq % 4;
				// load new twobit chunk when needed
				
				if(chunk_offset == 0) {
					fh->read(byte_tmp, 1);
					t.data = byte_tmp[0];
					chunk = t.get();
				}
				
				buffer[written++] = chunk[chunk_offset];
				
				i_in_seq++;
			}
			
			i++;
		}
	}
	
	delete[] byte_tmp;
	
	return written;
}

std::string fastafs_seq::sha1(std::ifstream *fh)
{
	char chunk[4];
	unsigned int i;
	
	SHA_CTX ctx;
	SHA1_Init(&ctx);
	
	uint_to_fourbytes(chunk, this->n);
	SHA1_Update(&ctx, chunk, 4);
	
	for(i = 0; i < this->n_starts.size(); i++) {
		uint_to_fourbytes(chunk, this->n_starts[i]);
		SHA1_Update(&ctx, chunk, 4);
		
		uint_to_fourbytes(chunk, this->n_ends[i]);
		SHA1_Update(&ctx, chunk, 4);
	}
	
	fh->seekg ((unsigned int) this->data_position + 4 + 4 + 4 + (this->n_starts.size() * 8), fh->beg);
	for(i = 0; i < this->n_twobits(); i++) {
		fh->read(chunk, 1);
		SHA1_Update(&ctx, chunk, 1);
	}
	
	
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1_Final(hash, &ctx);
	
	char outputBuffer[41];
	for(i = 0; i < SHA_DIGEST_LENGTH; i++) {
		sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
	}
	outputBuffer[40] = 0;
	return std::string(outputBuffer);
}

unsigned int fastafs_seq::n_twobits()
{
	// if n actg bits is:
	// 0 -> 0
	// 1,2,3 and 4 -> 1
	
	unsigned int n = this->n;
	
	for(unsigned int i = 0; i < this->n_starts.size(); i++) {
		n -= n_ends[i] - this->n_starts[i] + 1;
	}
	
	return (n + 3) / 4;
}


//@brief calculates the number of paddings found in a sequence of length N with
unsigned int fastafs_seq::n_padding(unsigned int offset, unsigned int position_until, unsigned int padding)
{
	// if debug:
	//   if offset > position_until
	//      throw error
	// end if
	
	unsigned int n = (position_until + 1) / (padding + 1);
	
	// minus all the n's that occurred before offset
	if (offset > 0) {
		n -= fastafs_seq::n_padding(0, offset - 1, padding);
	}
	
	return n;
}



//@brief finds the number of N's BEFORE pos, and returns whether POS is N
bool fastafs_seq::get_n_offset(unsigned int pos, unsigned int *num_Ns)
{
	*num_Ns = 0;
	
	for(unsigned int n_block = 0; n_block < this->n_starts.size(); ++n_block) {
		if(this->n_starts[n_block] > pos) {
			return false;
		} else {
			// als einde kleiner is dan pos, tel verschil op
			if(this->n_ends[n_block] < pos) {
				*num_Ns += (this->n_ends[n_block] - this->n_starts[n_block]) + 1;
			}
			// pos is within N block
			else if(this->n_ends[n_block] >= pos) {
				*num_Ns += (pos - this->n_starts[n_block]);// if pos is N and would be included: + 1
				
				return true;
			}
		}
	}
	
	return false;
}



fastafs::fastafs(std::string arg_name) :
	name(arg_name)
{

}

fastafs::~fastafs()
{
	for(unsigned int i = 0; i < this->data.size(); i++) {
		delete this->data[i];
	}
}

void fastafs::load(std::string afilename)
{

	std::streampos size;
	char *memblock;
	
	std::ifstream file (afilename, std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		this->filename = afilename;
		
		size = file.tellg();
		if(size < 16) {
			file.close();
			throw std::invalid_argument("Corrupt file: " + filename);
		} else {
			// magic
			// version
			//
			memblock = new char [17];
			file.seekg (0, std::ios::beg);
			file.read (memblock, 16);
			memblock[16] = '\0';
			
			char twobit_magic[5] = TWOBIT_MAGIC;
			
			
			unsigned int i;
			
			
			// check magic
			for(i = 0 ; i < 4;  i++) {
				if(memblock[i] != twobit_magic[i]) {
					throw std::invalid_argument("Corrupt file: " + filename);
				}
			}
			for(i = 0 + 4 ; i < 0 + 4 + 4;  i++) {
				if(memblock[i] != '\0' or memblock[i + 8] != '\0') {
					throw std::invalid_argument("Corrupt file: " + filename);
				}
			}
			
			unsigned int n_seq = fourbytes_to_uint(memblock, 8);
			
			
			unsigned char j;
			fastafs_seq *s;
			for(i = 0; i < n_seq; i ++ ) {
				s = new fastafs_seq;
				file.read (memblock, 1);
				
				char name[memblock[0] + 1];
				
				file.read (name, memblock[0]);
				name[(unsigned char) memblock[0]] = '\0';
				s->name = std::string(name);
				
				
				
				file.read(memblock, 4);
				s->data_position = fourbytes_to_uint(memblock, 0);
				
				this->data.push_back(s);
				
			}
			
			
			
			for(i = 0; i < n_seq; i ++ ) {
				s = this->data[i];
				
				file.seekg ((unsigned int) s->data_position, file.beg);
				
				//s->n
				file.read(memblock, 4);
				s->n = fourbytes_to_uint(memblock, 0);
				
				file.read(memblock, 4);
				unsigned int N_regions = fourbytes_to_uint(memblock, 0);
				
				
				for(j = 0 ; N_regions > j  ; j ++) {
					file.read(memblock, 4);
					s->n_starts.push_back( fourbytes_to_uint(memblock, 0));
				}
				for(j = 0 ; j < N_regions ; j ++) {
					file.read(memblock, 4);
					s->n_ends.push_back(fourbytes_to_uint(memblock, 0));
				}
				
				//file.read(memblock, 4);
				//unsigned int maskblock = fourbytes_to_uint(memblock, 0);
				
			}
			
			
			file.close();
			
			
			delete[] memblock;
		}
	}
	
	else { std::cout << "Unable to open file"; }
}



void fastafs::view_fasta(unsigned int padding)
{
	if(this->filename.size() == 0) {
		throw std::invalid_argument("No filename found");
	}
	
	std::ifstream file (this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		for(unsigned int i = 0; i < this->data.size(); i++) {
			this->data[i]->view_fasta(padding, &file);
		}
		file.close();
	}
}


int fastafs::view_fasta_chunk(unsigned int padding, char *buffer, size_t buffer_size, off_t file_offset)
{
	unsigned int written = 0;
	unsigned int total_fa_size = 0, i_buffer = 0;
	unsigned int i, seq_true_fasta_size;
	
	std::ifstream file (this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open()) {
	
		for(i = 0; i < this->data.size(); i++) {
			// lines below need to be calculated by member function of the sequences themselves
			seq_true_fasta_size = 1;// '>'
			seq_true_fasta_size += (unsigned int ) this->data[i]->name.size() + 1;// "chr1\n"
			seq_true_fasta_size += this->data[i]->n; // ACTG NNN
			seq_true_fasta_size += (this->data[i]->n + (padding - 1)) / padding;// number of newlines corresponding to ACTG NNN lines
			
			// determine whether and how much there needs to be read between: total_fa_size <=> total_fa_size + seq_true_fasta_size
			if((file_offset + i_buffer) >= total_fa_size and file_offset < (total_fa_size + seq_true_fasta_size)) {
				written += this->data[i]->view_fasta_chunk(
							   padding,
							   &buffer[i_buffer],
							   file_offset + i_buffer - total_fa_size,
							   std::min((unsigned int) buffer_size - i_buffer, seq_true_fasta_size - ( (unsigned int) file_offset + i_buffer - total_fa_size ) ),
							   &file
						   );

				while(file_offset + i_buffer < (total_fa_size + seq_true_fasta_size) and i_buffer < buffer_size) {
					i_buffer++;
				}
			}
			
			// update for next iteration
			total_fa_size += seq_true_fasta_size;
		}
		
		file.close();
	} else {
		throw std::runtime_error("could not load fastafs: " + this->filename);
	}
	
	return written;
}


unsigned int fastafs::fasta_filesize(unsigned int padding) {
	unsigned int n = 0;

	std::ifstream file (this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		file.close();
	
		for(unsigned int i = 0; i < this->data.size(); i++) {
			n = 1;// '>'
			n += (unsigned int ) this->data[i]->name.size() + 1;// "chr1\n"
			n += this->data[i]->n; // ACTG NNN
			n += (this->data[i]->n + (padding - 1)) / padding;// number of newlines corresponding to ACTG NNN lines
		}
		
	} else {
		throw std::runtime_error("could not load fastafs: " + this->filename);
	}
	

	return n;
}


int fastafs::view_faidx_chunk(unsigned int padding, char *buffer, size_t buffer_size, off_t file_offset)
{
	/*
		one	66	5	30	31
		two	28	98	14	15
	 */
	std::string contents = "";
	unsigned int written = 0;
	unsigned int i;
	std::string padding_s = std::to_string(padding);
	std::string padding_s2 = std::to_string(padding + 1);// padding + newline

	std::ifstream file (this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if(file.is_open()) {
		file.close();
		
		unsigned int offset = 0;
		for(i = 0; i < this->data.size(); i++) {
			offset += 1;// '>'
			offset += (unsigned int ) this->data[i]->name.size() + 1;// "chr1\n"
			
			contents += data[i]->name + "\t" + std::to_string(this->data[i]->n) + "\t" + std::to_string(offset) + "\t" + padding_s + "\t" + padding_s2 + "\n";
			
			offset += this->data[i]->n; // ACTG NNN
			offset += (this->data[i]->n + (padding - 1)) / padding;// number of newlines corresponding to ACTG NNN lines
		}
		
		/*
		std::cout << " ------ \n";
		std::cout << contents ;
		std::cout << " ------ \n";
		*/
		
		// buffer = 100
		// file_offset = 10
		// contents.size = 50
		while(written < buffer_size and written + file_offset < contents.size()) {
			buffer[written] = contents[written];
			written++;
		}
		
	} else {
		throw std::runtime_error("could not load fastafs: " + this->filename);
	}
	
	return written;
}


void fastafs::info()
{
	if(this->filename.size() == 0) {
		throw std::invalid_argument("No filename found");
	}
	
	std::ifstream file (this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		std::cout << "FASTAFS NAME: " << this->filename << "\n";
		printf("SEQUENCES:    %u\n", (unsigned int) this->data.size());
		
		for(unsigned int i = 0; i < this->data.size(); i++) {
			//this->data[i]->view(padding, &file);
			printf("    >%-24s%-12i%s\n", this->data[i]->name.c_str(), this->data[i]->n, this->data[i]->sha1(&file).c_str());
		}
		file.close();
	}
}


unsigned int fastafs::n()
{
	unsigned int n = 0;
	for(unsigned i = 0; i < this->data.size(); i++) {
		n += this->data[i]->n;
	}
	
	return n;
}



std::string fastafs::basename()
{
	return "";
}
