#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>

#include <openssl/sha.h>

// SSL requests to ENA
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


#include "config.hpp"

#include "twobit_byte.hpp"
#include "fastafs.hpp"
#include "utils.hpp"



fastafs_seq::fastafs_seq(): n(0)
{
}

unsigned int fastafs_seq::fasta_filesize(unsigned int padding)
{
    if(padding == 0) {
        padding = this->n;
    }

    unsigned int n = 1; // >
    n += (unsigned int ) this->name.size() + 1;// "chr1\n"
    n += this->n; // ACTG NNN
    n += (this->n + (padding - 1)) / padding;// number of newlines corresponding to ACTG NNN lines

    return n;
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

    fh->clear(); // because gseek was done before

    delete[] byte_tmp;
}

/*
@todo see if this can be a std::ifstream or some kind of stream type of object?
* padding = number of spaces?
* char buffer =
* start_pos_in_fasta =
* len_to_copy =
* fh = filestream to fastafs file
*/
unsigned int fastafs_seq::view_fasta_chunk(unsigned int padding, char *buffer, off_t start_pos_in_fasta, size_t buffer_size, std::ifstream *fh)
{
    unsigned int written = 0;

    if(written >= buffer_size) { // requesting a buffer of size=0
        fh->clear();
        return written;
    }
    
    unsigned int pos = start_pos_in_fasta;
    unsigned int pos_limit = 0;

    if(padding == 0) {
        padding = this->n;
    }
    
    
    // >
    pos_limit += 1;
    if(pos < pos_limit) {
        buffer[written++] = '>';
        pos++;

        if(written >= buffer_size) {
            fh->clear();
            return written;
        }
    }

    // sequence name
    pos_limit += this->name.size();
    while(pos < pos_limit) {
        //printf("written: name\n");
        buffer[written++] = this->name[this->name.size() - (pos_limit - pos)];
        pos++;

        if(written >= buffer_size) {
            fh->clear();
            return written;
        }
    }

    // \n
    pos_limit += 1;
    if(pos < pos_limit) {
        buffer[written++] = '\n';
        pos++;

        if(written >= buffer_size) {
            fh->clear();
            return written;
        }
    }

    // @todo put all this temporary data into a struct?
    // if fuse accepts something like file-call transactions ~ probably: fuse_file_info::fh is the fh id, test when this value changes, and wach out for seek()s
    std::vector<unsigned int> n_starts_w(this->n_starts);// workable type for this func
    std::vector<unsigned int> n_ends_w(this->n_ends);// workable type for this func
    n_starts_w.push_back(this->n);
    n_ends_w.push_back(this->n);
    unsigned int n_block = n_starts_w.size() - 1; /// last element, iterate back :)
    //for(unsigned int i = 0; i < n_starts_w.size(); i++) {
//        printf("n-block{%i}: [%i, %i]\n",i, n_starts_w[i], n_ends_w[i]);
    //}
    //printf("\n");
    //printf("pos=%i\npos_limit=%i\nheader-line=%i\n",pos, pos_limit, 1 + 1 + this->name.size());
    
    // calculate total number of full nucleotide lines
    unsigned int total_sequence_containing_lines = (this->n + padding - 1);//this->n / padding;
    //printf("total_sequenec_containing_lines = %d \n",total_sequence_containing_lines);
    unsigned int offset_from_sequence_line = pos - pos_limit;
    /*
    calc newlines passed:
    >chr1 \n
        ACTG\n      0 1 2 3 4
        ACTG\n      5 6 7 8 9

        0   0
        1   0
        2   0
        3   0
        4   0
        5   1
        6   1
        7   1
        8   1
        9   1
    */
    unsigned int newlines_passed = (offset_from_sequence_line) / (padding + 1);
    unsigned int nucleotide_pos = offset_from_sequence_line - newlines_passed;// requested nucleotide in file
    //printf("offset from sequence line: %i %u %d\n", offset_from_sequence_line, offset_from_sequence_line, offset_from_sequence_line);
    //printf("nucleotide_pos = %d | %u |  %i \n",nucleotide_pos, nucleotide_pos, nucleotide_pos);
    //printf("nuwlines_passed = %d %u %i \n",newlines_passed,newlines_passed,newlines_passed);
    
    // calculate file position for next twobit
    // when we are in an OPEN n block, we need to go to the first non-N base after, and place the file pointer there
    unsigned int n_passed = 0;
    this->get_n_offset(nucleotide_pos, &n_passed);
    unsigned int file_offset = this->data_position + 4 + 4 + 4 + (this->n_starts.size() * 8);// + (this->m_blocks.size() * 8) ;
    //printf("base file offset: %d\n",file_offset);
    file_offset += (nucleotide_pos - n_passed) / 4;
    //printf("____ file offset: %d\n",file_offset);
    fh->seekg((unsigned int) file_offset, fh->beg);
    
    /*
     0  0  0  0  1  1  1  1 << desired offset from starting point
     A  C  T  G  A  C  T  G
    * 
    
    handigste is om file pointer naar de byte ervoor te zetten
    vervolgens wanneer twobit_offset gelijk is aan nul, lees je de volgende byte
    * nooit out of bound
    
    */
    twobit_byte t = twobit_byte();
    char *byte_tmp = new char [2];
    const char *chunk;
    chunk = twobit_byte::twobit_hash[0];
    //printf("has been set to: [%c|%c|%c|%c|%c]\n",chunk[0],chunk[1],chunk[2],chunk[3],chunk[4]);
    
    unsigned char twobit_offset = (nucleotide_pos - n_passed) % 4;
    //printf("twobit_offset = %i\n", twobit_offset);
    if(twobit_offset != 0) {
        fh->read(byte_tmp, 1);
        
        //if (fh)
          // std::cout << "all characters read successfully.";
         //else
           //std::cout << "error: only " << fh->gcount() << " could be read";
        
        t.data = byte_tmp[0];
        chunk = t.get();
        //printf("[init] Reading new byte [%i] => %d !\n",fh->tellg(), t.data);
    }
    else {
        //printf("twobit offset == 0?\n");
    }
    
    //printf("nucleotide-pos: %i\n", nucleotide_pos);
    //printf("\n");
    //printf("\n");
        
    // using nucleotide position, calculate the n_block iterator and the line iterator
    unsigned int line_i = newlines_passed;//offset_from_sequence_line / padding;
    
    // since the n_blocks are sorted, a split search could technically become faster
    // complexity is now N, which could be reduced to log(n)
    printf("[init] => current nblock=%i   %i < %i   [%i, %i] \n", n_block, nucleotide_pos, n_ends_w[n_block], n_starts_w[n_block], n_ends_w[n_block]);
    while(n_block > 0 and nucleotide_pos <= n_ends_w[n_block - 1]) { // iterate back
        n_block--;
        printf("[iter] => current nblock=%i   %i < %i   [%i, %i] \n", n_block, nucleotide_pos, n_ends_w[n_block], n_starts_w[n_block], n_ends_w[n_block]);
    }
    
    pos_limit += line_i * (padding + 1);
    
    //printf("%d < %d      ( %d / %d)\n",line_i, total_sequence_containing_lines / padding, total_sequence_containing_lines , padding);
    while(line_i < total_sequence_containing_lines / padding) { // only 'complete' lines that are guarenteed 'padding' number of nucleotides long [ this loop starts at one to be unsigned-safe ]
        //printf(" - %d < %d      ( %d / %d) ** in loop writing this number of nucleotides to buffer: %d\n",line_i, total_sequence_containing_lines / padding, total_sequence_containing_lines , padding, std::min(padding, this->n - nucleotide_pos));
    
        pos_limit += std::min(padding, this->n - (line_i * padding));// only last line needs to be smaller ~ calculate from the beginning of line_i
        
        // write nucleotides
        while(pos < pos_limit) {
            //printf("%d < %d [pos < pos_limit]\n",pos, pos_limit);

            if(nucleotide_pos >= n_starts_w[n_block]) {
                buffer[written++] = 'N';
            }
            else {
                if(twobit_offset % 4 == 0) {
                    fh->read(byte_tmp, 1);
                    /*
                     * if DEBUG, ERROR ON fh == false
                    if (fh)
                       std::cout << "all " << fh->gcount() << " characters read successfully.\n";
                     else
                       std::cout << "error: only " << fh->gcount() << " could be read\n";
                       */
                    t.data = byte_tmp[0];
                    //t.data = '\0';
                    chunk = t.get();
                    //printf("[upd8] Reading new byte [%i] => %d / %i / %d",fh->tellg(),  (unsigned int) byte_tmp[0],  byte_tmp[0], byte_tmp[0]);
                    //printf(" [ %c | %c | %c | %c : %c ]\n",chunk[0], chunk[1], chunk[2], chunk[3],chunk[4]);
                }
                
                //buffer[written++] = '?';
                //printf("written: [ACTG] char\n");
                buffer[written++] = chunk[twobit_offset];
                //printf("%c",chunk[twobit_offset]);
                
                twobit_offset++;
                twobit_offset = twobit_offset % 4;
            }
            
            if(nucleotide_pos == n_ends_w[n_block]) {
                n_block++;
            }

            pos++;
            nucleotide_pos++;

            if(written >= buffer_size) {
                fh->clear();
                return written;
            }
        }
        
        // write newline
        pos_limit += 1;


        if(pos < pos_limit) {
            //printf("\n");
            buffer[written++] = '\n';
            pos++;

            if(written >= buffer_size) {
                fh->clear();
                return written;
            }
        }
        
        line_i++;
    }

    /*
    #if DEBUG
    // 
        throw std::runtime_error("Check if this should happen?");
    #endif // DEBUG
    */

    fh->clear();
    return written;

/*
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

    // NO CHECK FOR OUT OF BOUND SO FAR -
    size_t max_buffer_size = 0;
    if(this->fasta_filesize(padding) > start_pos_in_fasta) {
        max_buffer_size = this->fasta_filesize(padding) - start_pos_in_fasta;
    }
    buffer_size = std::min(buffer_size, max_buffer_size);

    for(; written < buffer_size; i_in_file++) {
        if((i_in_file % (padding + 1) == padding) or (i_in_file == this->n + num_paddings - 1)) {
            buffer[written++] = '\n';
        } else {
            // bereken in welk N block we zitten
            if(this->n_starts.size() > 0) {
                // i_n_start moet opgeteld worden zolang:
                //    - i_n_start < this->n_starts.size()
                //    - this->n_starts[i_n_start] < i
                while(i_n_start < (this->n_starts.size() - 1) and this->n_starts[i_n_start] < i) {
                    i_n_start++;
                    //printf(" -> %i > %i and %i == %i\n",this->n_starts.size() , i_n_start, i , this->n_starts[i_n_start]);
                }

                while(i_n_end < (this->n_ends.size() - 1) and this->n_ends[i_n_end] < i) {
                    i_n_end++;
                    //printf(" -> %i > %i and %i == %i\n",this->n_ends.size() , i_n_end, i , this->n_ends[i_n_end]);
                }

            }
            if(this->n_starts.size() > i_n_start and i == this->n_starts[i_n_start]) {
                in_N = true;
            }
            if(in_N) {
                //printf(" -> %i > %i and %i == %i\n",this->n_ends.size() , i_n_end, i , this->n_ends[i_n_end]);


                buffer[written++] = 'N';
                if(i == this->n_ends[i_n_end]) {
                    //i_n_end++;
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

    fh->clear();
    //fh->seekg(0, std::ios::beg);
    return written;
    */
}





/*
CRAM specification:

M5 (sequence MD5 checksum) field of @SQ sequence record in the BAM header is required and UR (URI
for the sequence fasta optionally gzipped file) field is strongly advised. The rule for calculating MD5 is
 - to remove any non-base symbols (like \n, sequence name or length and spaces) and
 - upper case the rest.

meaning: md5s([ACTGN]+)

// ww -l = 149998
*
* need = 74999


chunk size 1:
fastafs check short  1.49s user 2.79s system 99% cpu 4.282 total
fastafs check short  1.53s user 2.73s system 99% cpu 4.269 total

chunk size 1024:
??
*/
std::string fastafs_seq::sha1(std::ifstream *fh)
{
    const size_t header_offset = this->name.size() + 2;
    //const size_t fasta_size = header_offset + this->n; // not size effectively, as terminating newline is skipped..., but length to be read
    const unsigned long chunksize = 1024*1024;// seems to not matter that much
    char chunk[chunksize + 2];
    chunk[chunksize] = '\0';

    SHA_CTX ctx;
    SHA1_Init(&ctx);

    fh->clear();

    // "(a/b)*b + a%b shall equal a"
    // full iterations = this->n / chunk_size; do this number of iterations looped
    unsigned long n_iterations = (unsigned long) this->n / chunksize;
    signed int remaining_bytes = this->n % chunksize;
    // half iteration remainder = this->n % chunk_size; if this number > 0; do it too

    unsigned long nbases = 0;

    for(unsigned long i = 0; i < n_iterations; i++) {
        this->view_fasta_chunk(0, chunk, header_offset + (i * chunksize), chunksize, fh);
        //printf("[%s] - %i\n", chunk, chunksize);
        SHA1_Update(&ctx, chunk, chunksize);

        nbases += chunksize;
    }

    if(remaining_bytes > 0) {
        this->view_fasta_chunk(0, chunk, header_offset + (n_iterations * chunksize), remaining_bytes, fh);
        SHA1_Update(&ctx, chunk, remaining_bytes);
        nbases += remaining_bytes;

        chunk[remaining_bytes] = '\0';
        //printf("[%s] - %i (last chunk)\n", chunk, remaining_bytes);
    }

    //printf(" (%i * %i) + %i =  %i  = %i\n", n_iterations , chunksize, remaining_bytes , (n_iterations * chunksize) + remaining_bytes , this->n);
    //printf("nbases=%i\n", nbases);

    unsigned char sha1_digest[SHA_DIGEST_LENGTH];
    SHA1_Final(sha1_digest, &ctx);

    fh->clear(); // because gseek was done before

    char sha1_hash[41];
    sha1_digest_to_hash(sha1_digest, sha1_hash);

    return std::string(sha1_hash);
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
            memblock = new char [20+1];//sha1 is 20b
            file.seekg (0, std::ios::beg);
            file.read (memblock, 16);
            memblock[16] = '\0';

            //char twobit_magic[5] = TWOBIT_MAGIC;

            unsigned int i;

            // check magic
            for(i = 0 ; i < 4;  i++) {
                if(memblock[i] != UCSC2BIT_MAGIC[i]) {
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

                // SHA1 digest
                file.read(memblock, 20);
                //s->sha1_digest = memblock;
                //strncpy(*s->sha1_digest, memblock);
                for(int j = 0; j < 20 ; j ++) {
                    s->sha1_digest[j] = memblock[j];
                }

                //char sha1_hash[41] = "";
                //sha1_digest_to_hash(s->sha1_digest, sha1_hash);
                //printf("sha1 from file: [%s]\n", sha1_hash);

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

    else {
        std::cout << "Unable to open file";
    }
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


unsigned int fastafs::view_fasta_chunk(unsigned int padding, char *buffer, size_t buffer_size, off_t file_offset)
{
    unsigned int written = 0;
    unsigned int total_fa_size = 0, i_buffer = 0;
    unsigned int i, seq_true_fasta_size;

    std::ifstream file (this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {

        for(i = 0; i < this->data.size(); i++) {
            //printf("\n\n\nsequence: %d\n", i);
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


off_t fastafs::ucsc2bit_filesize(void)
{
    off_t nn = 4 + 4 + 4 + 4;// header, version, n-seq, rsrvd

    size_t i;// auto iterator?
    for(i = 0; i < this->data.size(); i++) {
        nn += 1; // namesize
        nn += 4; // offset in file

        nn += 4;// dna size
        nn += 4;// n blocks
        nn += this->data[i]->n_starts.size() * (4 * 2); // both start and end

        nn += 4;// n masked regions - so far always 0
        nn += 4;// reserved

        nn += this->data[i]->name.size();

        /*
        nn_actg = this->data[i]->n;
        nn += nn_actg / 4;
        if(nn_actg % 4 > 0){
            nn++;
        }*/

        nn += (this->data[i]->n + 3) / 4; // math.ceil hack
    }

    return nn;
}


//http://genome.ucsc.edu/FAQ/FAQformat.html#format7
//https://www.mathsisfun.com/binary-decimal-hexadecimal-converter.html
unsigned int fastafs::view_ucsc2bit_chunk(char *buffer, size_t buffer_size, off_t file_offset)
{
    unsigned int written = 0;
    unsigned int pos = file_offset; // iterator (position, in bytes) in file
    unsigned int pos_limit = 0; // counter to keep track of when writing needs to stop for given loop

    std::ifstream file (this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        char n_seq[4];

        pos_limit += 4;// skip this loop after writing first four bytes
        while(pos < pos_limit) {
            buffer[written++] = UCSC2BIT_MAGIC[pos];
            pos++;

            if(written >= buffer_size) {
                return written;
            }
        }

        pos_limit += 4;
        while(pos < pos_limit) {
            buffer[written++] = UCSC2BIT_VERSION[pos - 4];
            pos++;

            if(written >= buffer_size) {
                return written;
            }
        }

        // number sequences
        uint_to_fourbytes_ucsc2bit(n_seq, (unsigned int) this->data.size());
        pos_limit += 4;
        while(pos < pos_limit) {
            buffer[written++] = n_seq[pos - 8];
            pos++;

            if(written >= buffer_size) {
                return written;
            }
        }

        // 4 x nullbyte
        pos_limit += 4;
        while(pos < pos_limit) {
            buffer[written++] = '\0';
            pos++;

            if(written >= buffer_size) {
                return written;
            }
        }

        unsigned int header_block_len = 4+4+4+4 + ((unsigned int) this->data.size() * (1 + 4));
        unsigned int header_offset_previous = 0;
        for(unsigned int i = 0; i < this->data.size(); i++) {
            header_block_len += this->data[i]->name.size();
        }

        for(auto sequence = this->data.begin(); sequence != this->data.end(); sequence++) {
            // single byte can be written, as the while loop has returned true
            pos_limit += 1;
            if(pos < pos_limit) {
                buffer[written++] = (unsigned char) (*sequence)->name.size();
                pos++;

                if(written >= buffer_size) {
                    return written;
                }
            }

            // sequence name
            pos_limit += (*sequence)->name.size();
            while(pos < pos_limit) {
                buffer[written++] = (*sequence)->name[(*sequence)->name.size() - (pos_limit - pos)];
                pos++;

                if(written >= buffer_size) {
                    return written;
                }
            }

            // file offset
            unsigned int offset = header_block_len + header_offset_previous;
            uint_to_fourbytes_ucsc2bit(n_seq, offset);
            pos_limit += 4;
            while(pos < pos_limit) {
                buffer[written++] = n_seq[4 - (pos_limit - pos)];
                pos++;

                if(written >= buffer_size) {
                    return written;
                }
            }

            header_offset_previous += 4 + 4 + 4 + 4;
            header_offset_previous += 8 * (*sequence)->n_starts.size();
            header_offset_previous += (*sequence)->n / 4;
            if((*sequence)->n % 4 != 0) {
                header_offset_previous++;
            }
        }

        for(auto sequence = this->data.begin(); sequence != this->data.end(); sequence++) {
            // number nucleotides
            uint_to_fourbytes_ucsc2bit(n_seq, (*sequence)->n);
            pos_limit += 4;
            while(pos < pos_limit) {
                buffer[written++] = n_seq[4 - (pos_limit - pos)];
                pos++;

                if(written >= buffer_size) {
                    return written;
                }
            }

            // number N blocks
            uint_to_fourbytes_ucsc2bit(n_seq, (*sequence)->n_starts.size());
            pos_limit += 4;
            while(pos < pos_limit) {
                buffer[written++] = n_seq[4 - (pos_limit - pos)];
                pos++;

                if(written >= buffer_size) {
                    return written;
                }
            }

            // write n-blocks effectively down!
            for(unsigned int k = 0; k < (*sequence)->n_starts.size(); k++) {
                uint_to_fourbytes_ucsc2bit(n_seq, (*sequence)->n_starts[k]);
                pos_limit += 4;
                while(pos < pos_limit) {
                    buffer[written++] = n_seq[4 - (pos_limit - pos)];
                    pos++;

                    if(written >= buffer_size) {
                        return written;
                    }
                }

                uint_to_fourbytes_ucsc2bit(n_seq, (*sequence)->n_ends[k] - (*sequence)->n_starts[k] + 1);
                pos_limit += 4;
                while(pos < pos_limit) {
                    buffer[written++] = n_seq[4 - (pos_limit - pos)];
                    pos++;

                    if(written >= buffer_size) {
                        return written;
                    }
                }
            }

            // m-blocks + reserved block
            pos_limit += 4 + 4;
            while(pos < pos_limit) {
                buffer[written++] = '\0';
                pos++;

                if(written >= buffer_size) {
                    return written;
                }
            }

            // twobit coded nucleotides (only containing 4 nucleotides each)
            unsigned int full_twobits = (*sequence)->n / 4;
            twobit_byte t;
            pos_limit += full_twobits;
            while(pos < pos_limit) {
                //printf("%i - %i  = %i  ||  %i\n",pos_limit,pos, (full_twobits - (pos_limit - pos)) * 4, j);
                (*sequence)->view_fasta_chunk(0, n_seq, (*sequence)->name.size() + 2 + ((full_twobits - (pos_limit - pos)) * 4), 4, &file);
                t.set(n_seq);
                buffer[written++] = t.data;
                pos++;

                if(written >= buffer_size) {
                    return written;
                }
            }

            // last byte, may also rely on 1,2 or 3 nucleotides and reqiures setting 0's
            if(full_twobits * 4 < (*sequence)->n) {
                n_seq[0] = 'N';
                n_seq[1] = 'N';
                n_seq[2] = 'N';
                n_seq[3] = 'N';

                pos_limit += 1;
                if(pos < pos_limit) {
                    //printf("%i - %i  = %i  ||  %i      ::    %i  == %i \n",pos_limit,pos, full_twobits * 4, j, (*sequence)->n - (full_twobits * 4),  (*sequence)->n - j);

                    (*sequence)->view_fasta_chunk(0, n_seq, (*sequence)->name.size() + 2 + full_twobits * 4, (*sequence)->n - (full_twobits * 4), &file);
                    t.set(n_seq);

                    buffer[written++] = t.data;
                    pos++;

                    if(written >= buffer_size) {
                        return written;
                    }
                }
            }
        }

        file.close();
    } else {
        throw std::runtime_error("could not load fastafs: " + this->filename);
    }

    return written;
}






unsigned int fastafs::fasta_filesize(unsigned int padding)
{
    unsigned int n = 0;

    std::ifstream file (this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        file.close();

        for(unsigned int i = 0; i < this->data.size(); i++) {
            n += this->data[i]->fasta_filesize(padding);
            /*
            n += 1;// '>'
            n += (unsigned int ) this->data[i]->name.size() + 1;// "chr1\n"
            n += this->data[i]->n; // ACTG NNN
            n += (this->data[i]->n + (padding - 1)) / padding;// number of newlines corresponding to ACTG NNN lines
            */
        }

    } else {
        throw std::runtime_error("could not load fastafs: " + this->filename);
    }


    return n;
}


std::string fastafs::get_faidx(unsigned int padding)
{

    std::string contents = "";
    std::string padding_s = std::to_string(padding);
    std::string padding_s2 = std::to_string(padding + 1);// padding + newline

    std::ifstream file (this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        file.close();

        unsigned int offset = 0;
        for(unsigned int i = 0; i < this->data.size(); i++) {
            offset += 1;// '>'
            offset += (unsigned int ) this->data[i]->name.size() + 1;// "chr1\n"

            contents += data[i]->name + "\t" + std::to_string(this->data[i]->n) + "\t" + std::to_string(offset) + "\t" + padding_s + "\t" + padding_s2 + "\n";

            offset += this->data[i]->n; // ACTG NNN
            offset += (this->data[i]->n + (padding - 1)) / padding;// number of newlines corresponding to ACTG NNN lines
        }

        //while(written < buffer_size and written + file_offset < contents.size()) {
        //	buffer[written] = contents[written];
        //	written++;
        //	}

    } else {
        throw std::runtime_error("could not load fastafs: " + this->filename);
    }

    return contents;
}



unsigned int fastafs::view_faidx_chunk(unsigned int padding, char *buffer, size_t buffer_size, off_t file_offset)
{
    std::string contents = this->get_faidx(padding);

    unsigned int written = 0;
    while(written < buffer_size and written + file_offset < (unsigned int) contents.size()) {
        buffer[written] = contents[written];
        written++;
    }


    return written;
}

/*
https://www.ebi.ac.uk/ena/cram/sha1/7716832754e642d068e6fbd8f792821ca5544309
Hello message sent

HTTP/1.1 301 Moved Permanently
Content-Type: text/html
Date: Fri, 15 Feb 2019 11:20:13 GMT
Location: https://www.ebi.ac.uk/ena/cram/sha1/7716832754e642d068e6fbd8f792821ca5544309
Connection: Keep-Alive
Content-Length: 0
*
*
good response:*
* HTTP/1.0 200 OK
Cache-Control: max-age=31536000
X-Cache: MISS from pg-ena-cram-1.ebi.ac.uk
Content-Type: text/plain
Strict-Transport-Security: max-age=0
Date: Fri, 15 Feb 2019 11:32:17 GMT
X-Application-Context: application:8080
X-Cache-Lookup: MISS from pg-ena-cram-1.ebi.ac.uk:8080
Via: 1.0 pg-ena-cram-1.ebi.ac.uk (squid/3.1.23)
Connection: keep-alive
Content-Length: 249250621


bad response:
HTTP/1.0 404 Not Found
X-Cache: MISS from pg-ena-cram-1.ebi.ac.uk
Content-Type: text/html;charset=utf-8
Strict-Transport-Security: max-age=0
Date: Fri, 15 Feb 2019 11:32:58 GMT
X-Application-Context: application:8080
Content-Language: en
X-Cache-Lookup: MISS from pg-ena-cram-1.ebi.ac.uk:8080
Via: 1.0 pg-ena-cram-1.ebi.ac.uk (squid/3.1.23)
Connection: keep-alive
Content-Length: 1047

 */
int fastafs::info(bool ena_verify_checksum)
{
    if(this->filename.size() == 0) {
        throw std::invalid_argument("No filename found");
    }

    char sha1_hash[41] = "";
    sha1_hash[40] = '\0';

    std::ifstream file (this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if(file.is_open()) {
        std::cout << "FASTAFS NAME: " << this->filename << "\n";
        printf("SEQUENCES:    %u\n", (unsigned int) this->data.size());

        for(unsigned int i = 0; i < this->data.size(); i++) {
            sha1_digest_to_hash(this->data[i]->sha1_digest, sha1_hash);

            if(ena_verify_checksum) {
                //wget header of:
                //https://www.ebi.ac.uk/ena/cram/sha1/<sha1>
                //std::cout << "https://www.ebi.ac.uk/ena/cram/sha1/" << sha1_hash << "\n";

                SSL *ssl;
                //int sock_ssl = 0;

                //struct sockadfiledr_in address;
                int sock = 0;
                struct sockaddr_in serv_addr;
                std::string hello2 = "GET /ena/cram/sha1/" + std::string(sha1_hash) + " HTTP/1.1\r\nHost: www.ebi.ac.uk\r\nConnection: Keep-Alive\r\n\r\n";
                //char *hello = &hello2.c_str();
                char buffer[1024] = {0};
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    printf("\n Socket creation error \n");
                    return -1;
                }

                memset(&serv_addr, '0', sizeof(serv_addr));

                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(443);

                // Convert IPv4 and IPv6 addresses from text to binary form
                if(inet_pton(AF_INET, "193.62.193.80", &serv_addr.sin_addr)<=0) {
                    printf("\nInvalid address/ Address not supported \n");
                    return -1;
                }

                if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                    printf("\nConnection Failed \n");
                    return -1;
                }

                SSL_library_init();
                SSLeay_add_ssl_algorithms();
                SSL_load_error_strings();
                const SSL_METHOD *meth = TLS_client_method();// defining version specificity in here often results in deprecation warnings over time
                SSL_CTX *ctx = SSL_CTX_new (meth);
                ssl = SSL_new(ctx);
                if (!ssl) {
                    printf("Error creating SSL.\n");
                    //log_ssl();
                    return -1;
                }

                //int sock_ssl = SSL_get_fd(ssl);
                SSL_set_fd(ssl, sock);
                int err = SSL_connect(ssl);
                if (err <= 0) {
                    printf("Error creating SSL connection.  err=%x\n", err);
                    //log_ssl();
                    fflush(stdout);
                    return -1;
                }

                SSL_write(ssl, hello2.c_str(), hello2.length());

                int NNvalread = SSL_read(ssl, buffer, 32);
                if(NNvalread < 0) {
                    printf("    >%-24s%-12i%s   <connection error>\n", this->data[i]->name.c_str(), this->data[i]->n, sha1_hash);
                } else if(std::string(buffer).find(" 200 ") != (size_t) -1) { // sequence is in ENA
                    printf("    >%-24s%-12i%s   https://www.ebi.ac.uk/ena/cram/sha1/%s\n", this->data[i]->name.c_str(), this->data[i]->n, sha1_hash, sha1_hash);
                } else {
                    printf("    >%-24s%-12i%s   ---\n", this->data[i]->name.c_str(), this->data[i]->n, sha1_hash);
                }
            } else {
                printf("    >%-24s%-12i%s\n", this->data[i]->name.c_str(), this->data[i]->n, sha1_hash);
            }
        }

        file.close();
    }

    return 0;
}


int fastafs::check_integrity()
{
    if(this->filename.size() == 0) {
        throw std::invalid_argument("No filename found");
    }

    int retcode = 0;
    char sha1_hash[41] = "";
    sha1_hash[40] = '\0';
    std::string old_hash;

    std::ifstream file (this->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        for(unsigned int i = 0; i < this->data.size(); i++) {
            sha1_digest_to_hash(this->data[i]->sha1_digest, sha1_hash);
            old_hash = std::string(sha1_hash);
            std::string new_hash = this->data[i]->sha1(&file);

            if(old_hash.compare(new_hash) == 0) {
                printf("OK\t%s\n",this->data[i]->name.c_str());
            } else {
                printf("ERROR\t%s\t%s != %s\n",this->data[i]->name.c_str(), sha1_hash, new_hash.c_str());
                retcode = EIO;
            }
        }
        file.close();
    }

    return retcode;
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
