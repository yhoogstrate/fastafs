#include <iostream>
#include <fstream>

#include "config.hpp"

#include "ucsc2bit_to_fastafs.hpp"
#include "utils.hpp"






void ucsc2bit_to_fastafs(std::string ucsc2bit_file, std::string fastafs_file)
{
    const char nt[2] = "T";
    const char nc[2] = "C";
    const char na[2] = "A";
    const char ng[2] = "G";
    const char nn[2] = "N";


    char buffer[16 +  1];

    fastafs fs_new = fastafs("");
    std::vector<ucsc2bit_seq_header *> data;
    std::vector<ucsc2bit_seq_header_conversion_data *> data2;
    unsigned int i, j, n;
    ucsc2bit_seq_header *s;
    ucsc2bit_seq_header_conversion_data *t;

    std::ifstream fh_ucsc2bit (ucsc2bit_file.c_str(), std::ios::in | std::ios::binary);
    std::ofstream fh_fastafs (fastafs_file.c_str(), std::ios::out | std::ios::binary);
    if(fh_ucsc2bit.is_open() and fh_fastafs.is_open()) {
        fh_ucsc2bit.read(buffer, 12);

        n = fourbytes_to_uint_ucsc2bit(buffer, 8);

        // write fastafs header
        fh_fastafs << UCSC2BIT_MAGIC;
        fh_fastafs << UCSC2BIT_VERSION;
        uint_to_fourbytes(buffer, n);
        fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
        fh_fastafs << "\x00\x00\x00\x00"s;


        fh_ucsc2bit.seekg(16);
        for(i = 0 ; i < n; i ++) {
            s = new ucsc2bit_seq_header();
            t = new ucsc2bit_seq_header_conversion_data();

            data.push_back(s);
            data2.push_back(t);

            fh_ucsc2bit.read(buffer, 1);
            s->name_size = buffer[0];

            fh_ucsc2bit.read(buffer, s->name_size);
            s->name = new char[s->name_size + 1];
            strncpy(s->name, buffer, s->name_size);
            s->name[s->name_size] = '\0';

            fh_ucsc2bit.read(buffer, 4);
            s->offset = fourbytes_to_uint_ucsc2bit(buffer, 0);


            SHA1_Init(&t->ctx);

            fh_fastafs.write((char *) &s->name_size, (size_t) 1); // name size
            fh_fastafs.write(s->name, (size_t) s->name_size);// name

            t->header_position = fh_fastafs.tellp();

            //@todo nullbytes instead
            fh_fastafs << "ssssssssssSSSSSSSSSS"s;//sha1 placeholder, overwrite later with gseek etc
            fh_fastafs << "oooo"s;//file offset placeholder, can only be rewritten after m and n blocks are filled
        }


        for(i = 0 ; i < n; i ++) {
            // seq-len
            s = data[i];
            t = data2[i];

            t->file_offset = fh_fastafs.tellp();

            fh_ucsc2bit.read(buffer, 4);
            s->dna_size = fourbytes_to_uint_ucsc2bit(buffer, 0);

            uint_to_fourbytes(buffer, s->dna_size);
            fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);


            // parse N blocks
            fh_ucsc2bit.read(buffer, 4);
            s->n_blocks = fourbytes_to_uint_ucsc2bit(buffer, 0);

            uint_to_fourbytes(buffer, s->n_blocks);
            fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);

            for(j = 0; j < s->n_blocks; j++) {
                fh_ucsc2bit.read(buffer, 4);
                s->n_block_starts.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));

                uint_to_fourbytes(buffer, s->n_block_starts.back());
                fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
            }

            for(j = 0; j < s->n_blocks; j++) {
                fh_ucsc2bit.read(buffer, 4);
                s->n_block_sizes.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));

                uint_to_fourbytes(buffer, s->n_block_starts.back() + s->n_block_sizes.back() - 1);
                fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
            }

            // parse M blocks
            fh_ucsc2bit.read(buffer, 4);
            s->m_blocks = fourbytes_to_uint_ucsc2bit(buffer, 0);

            uint_to_fourbytes(buffer, s->m_blocks);
            fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);

            for(j = 0; j < s->m_blocks; j++) {
                fh_ucsc2bit.read(buffer, 4);
                s->m_block_starts.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));

                uint_to_fourbytes(buffer, s->m_block_starts.back());
                fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
            }

            for(j = 0; j < s->m_blocks; j++) {
                fh_ucsc2bit.read(buffer, 4);
                s->m_block_sizes.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));

                uint_to_fourbytes(buffer, s->m_block_starts.back() + s->m_block_sizes.back() - 1);
                fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);
            }

            // parse and convert sequence
            fh_ucsc2bit.read(buffer, 4);
            twobit_byte t_in = twobit_byte();
            const char *decoded_in;

            twobit_byte t_out = twobit_byte();

            unsigned int k = 0; // iterator in fastafs format
            unsigned int next_n;
            unsigned int n_n = 0;
            unsigned int n_ahead = 0; // number of n's ahead
            if(s->n_blocks > n_n) {
                next_n = s->n_block_starts[n_n];
            } else {
                next_n = s->dna_size + 1; // otherwise -1
            }
            for(j = 0; j < s->dna_size; j++) {
                if(j % 4 == 0) {
                    fh_ucsc2bit.read(buffer, 1);
                    t_in.data = buffer[0];
                    decoded_in = t_in.get();// pointer to the right value?
                }

                // check if N
                if(j == next_n) { // new next block has opened
                    n_ahead = s->n_block_sizes[n_n];
                }
                if(n_ahead > 0) {// we are in an N block on an N base
                    SHA1_Update(&t->ctx, nn, 1);

                    n_ahead -= 1;

                    if(n_ahead == 0) {
                        n_n++;
                        if(s->n_blocks > n_n) {
                            next_n = s->n_block_starts[n_n];
                            n_ahead = 0;
                        }
                    }
                } else { // non-N char
                    switch(decoded_in[j % 4]) {
                    case 't':
                    case 'T':
                    case 'u':
                    case 'U':
                        t_out.set(twobit_byte::iterator_to_offset(k), 0);
                        SHA1_Update(&t->ctx, nt, 1);

                        break;
                    case 'c':
                    case 'C':
                        t_out.set(twobit_byte::iterator_to_offset(k), 1);
                        SHA1_Update(&t->ctx, nc, 1);

                        break;
                    case 'a':
                    case 'A':
                        t_out.set(twobit_byte::iterator_to_offset(k), 2);
                        SHA1_Update(&t->ctx, na, 1);

                        break;
                    case 'g':
                    case 'G':
                        t_out.set(twobit_byte::iterator_to_offset(k), 3);
                        SHA1_Update(&t->ctx, ng, 1);

                        break;
                    }
                    if(k % 4 == 3) {
                        fh_fastafs.write((char *) &(t_out.data), (size_t) 1); // name size
                    }
                    k++;
                }
            }

            if(k % 4 != 0) {
                for(j = k % 4; j < 4; j++) {
                    t_out.set(twobit_byte::iterator_to_offset(j), 0);
                }
                fh_fastafs.write((char *) &(t_out.data), (size_t) 1); // name size

            }

            SHA1_Final(t->sha1_digest, &t->ctx);

            delete[] s->name;
            delete s;
        }

        // re-write headers (file offsets and sha1 sums)
        for(i = 0 ; i < n; i ++) {
            t = data2[i];

            fh_fastafs.seekp(t->header_position, std::ios::beg);

            //fh_fastafs << t->sha1_digest; // this way can be nasty if last bytes are nullbytes
            fh_fastafs.write(reinterpret_cast<char *> (&t->sha1_digest), (size_t) 20);

            uint_to_fourbytes(buffer, (unsigned int) t->file_offset);
            fh_fastafs.write(reinterpret_cast<char *> (&buffer), (size_t) 4);

            delete t;
        }
    }

    fh_fastafs.close();
    fh_ucsc2bit.close();
}

