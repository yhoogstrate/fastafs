#include <iostream>
#include <fstream>

#include "config.hpp"

#include "ucsc2bit_to_fastafs.hpp"
#include "flags.hpp"
#include "utils.hpp"




/*
 * ucsc2bit_to_fastafs - creates a FASTAFS_FILE with contents identical to an UCSC 2bit file
 *
 * @ucsc2bit_file: file name of input file (must be read accessible)
 * @fastafs_file:  file name of the output FASTAFS file (must be write accessible)
 *
 * @todo2: use filehandles/if|ofstreams instead of filenames
 */
size_t ucsc2bit_to_fastafs(std::string ucsc2bit_file, std::string fastafs_file)
{
    const char nt[2] = "T";
    const char nc[2] = "C";
    const char na[2] = "A";
    const char ng[2] = "G";
    const char nn[2] = "N";

    unsigned char buffer[16 +  1];
    fastafs fs_new = fastafs("");
    uint32_t i, j, n;

    fastafs_flags ffsf;
    ffsf.set_incomplete();

    ucsc2bit_seq_header *s;
    ucsc2bit_seq_header_conversion_data *t;

    std::ifstream fh_ucsc2bit(ucsc2bit_file.c_str(), std::ios::in | std::ios::binary);
    std::ofstream fh_fastafs(fastafs_file.c_str(), std::ios::out | std::ios::binary);
    if(fh_ucsc2bit.is_open() and fh_fastafs.is_open()) {

        // Write header
        fh_fastafs << FASTAFS_MAGIC;
        fh_fastafs << FASTAFS_VERSION;

        // the flag for now, set to INCOMPLETE as writing is in progress || spacer that will be overwritten later
        fh_fastafs << ffsf.get_bits()[0];
        fh_fastafs << ffsf.get_bits()[1];

        fh_fastafs << "\x00\x00\x00\x00"s;// position of metedata ~ unknown YET

        // Read UCSC2bit header (n seq)
        fh_ucsc2bit.read((char*)(&buffer[0]), 12);    //conversion from unsigned char* to char* (https://stackoverflow.com/questions/604431/c-reading-unsigned-char-from-file-stream)
        n = fourbytes_to_uint_ucsc2bit(buffer, 8);
        uint_to_fourbytes(buffer, n);
        std::vector<ucsc2bit_seq_header *> data(n);
        std::vector<ucsc2bit_seq_header_conversion_data *> data2(n);

        // point to header
        fh_ucsc2bit.seekg(16);
        for(i = 0 ; i < n; i ++) {
            s = new ucsc2bit_seq_header();
            t = new ucsc2bit_seq_header_conversion_data();

            data[i] = s;
            data2[i] = t;

            fh_ucsc2bit.read((char*)&buffer[0], 1);
            s->name_size = buffer[0];

            fh_ucsc2bit.read((char*)&buffer[0], s->name_size);
            s->name = new char[s->name_size + 1];
            strncpy(s->name, (char*)&buffer[0], s->name_size);
            s->name[s->name_size] = '\0';

            fh_ucsc2bit.read((char*)&buffer[0], 4);
            s->offset = fourbytes_to_uint_ucsc2bit(buffer, 0);
        }
        for(i = 0 ; i < n; i ++) {

            // seq-len
            s = data[i];
            t = data2[i];
            t->file_offset_dna_in_ucsc2bit = fh_fastafs.tellp();

            fh_ucsc2bit.read((char*)&buffer[0], 4);
            s->dna_size = fourbytes_to_uint_ucsc2bit(buffer, 0);

            // parse N blocks
            fh_ucsc2bit.read((char*)&buffer[0], 4);
            s->n_blocks = fourbytes_to_uint_ucsc2bit(buffer, 0);
            for(j = 0; j < s->n_blocks; j++) {
                fh_ucsc2bit.read((char*)&buffer[0], 4);
                s->n_block_starts.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));
            }
            for(j = 0; j < s->n_blocks; j++) {
                fh_ucsc2bit.read((char*)&buffer[0], 4);
                s->n_block_sizes.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));
                t->N += s->n_block_sizes.back();//ucsc2bit provides lengths
            }

            // parse M blocks
            fh_ucsc2bit.read((char*)&buffer[0], 4);
            s->m_blocks = fourbytes_to_uint_ucsc2bit(buffer, 0);
            for(j = 0; j < s->m_blocks; j++) {
                fh_ucsc2bit.read((char*)&buffer[0], 4);
                s->m_block_starts.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));
            }
            for(j = 0; j < s->m_blocks; j++) {
                fh_ucsc2bit.read((char*)&buffer[0], 4);
                s->m_block_sizes.push_back(fourbytes_to_uint_ucsc2bit(buffer, 0));
            }

            // write number fo compressed nucleotides
            uint_to_fourbytes(buffer, s->dna_size - t->N);
            fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);

            // parse and convert sequence
            fh_ucsc2bit.read((char*)&buffer[0], 4);
            twobit_byte t_in = twobit_byte(ENCODE_HASH_TWOBIT_DNA);
            const char *decoded_in = t_in.encode_hash[0];// unnecessary initialization but otherwise gcc whines
            twobit_byte t_out = twobit_byte(ENCODE_HASH_TWOBIT_DNA);

            uint32_t k = 0; // iterator in fastafs format
            uint32_t next_n;
            uint32_t n_n = 0;
            uint32_t n_ahead = 0; // number of n's ahead
            if(s->n_blocks > n_n) {
                next_n = s->n_block_starts[n_n];
            } else {
                next_n = s->dna_size + 1; // otherwise -1
            }
            for(j = 0; j < s->dna_size; j++) {
                if(j % 4 == 0) {
                    fh_ucsc2bit.read((char*)&buffer[0], 1);
                    t_in.data = buffer[0];
                    decoded_in = t_in.get();// pointer to the right value?
                }

                // check if N
                if(j == next_n) {
                    // new next block has opened
                    n_ahead = s->n_block_sizes[n_n];
                }

                if(n_ahead > 0) {
                    // we are in an N block on an N base
                    EVP_DigestUpdate(t->mdctx, nn, 1); // new
                    //MD5_Update(&t->ctx, nn, 1);
                    n_ahead -= 1;
                    if(n_ahead == 0) {
                        n_n++;

                        if(s->n_blocks > n_n) {
                            next_n = s->n_block_starts[n_n];
                            n_ahead = 0;
                        }
                    }
                } else {
                    // non-N char
                    switch(decoded_in[j % 4]) {
                    case 't':
                    case 'T':
                    case 'u':
                    case 'U':
                        t_out.set(twobit_byte::iterator_to_offset(k), 0);
                        EVP_DigestUpdate(t->mdctx, nt, 1); // new
                        //MD5_Update(&t->ctx, nt, 1);
                        break;
                    case 'c':
                    case 'C':
                        t_out.set(twobit_byte::iterator_to_offset(k), 1);
                        EVP_DigestUpdate(t->mdctx, nc, 1); // new
                        //MD5_Update(&t->ctx, nc, 1);
                        break;
                    case 'a':
                    case 'A':
                        t_out.set(twobit_byte::iterator_to_offset(k), 2);
                        EVP_DigestUpdate(t->mdctx, na, 1); // new
                        //MD5_Update(&t->ctx, na, 1);
                        break;
                    case 'g':
                    case 'G':
                        t_out.set(twobit_byte::iterator_to_offset(k), 3);
                        EVP_DigestUpdate(t->mdctx, ng, 1); // new
                        //MD5_Update(&t->ctx, ng, 1);
                        break;
                    }
                    if(k % 4 == 3) {
                        fh_fastafs.write((char *) & (t_out.data), (size_t) 1); // name size
                    }
                    k++;
                }
            }
            if(k % 4 != 0) {
                for(j = k % 4; j < 4; j++) {
                    t_out.set(twobit_byte::iterator_to_offset(j), 0);
                }
                // @todo hf_fastsfs << t_out.data
                fh_fastafs.write((char *) & (t_out.data), (size_t) 1); // name size
            }

            unsigned int md5_digest_len = EVP_MD_size(EVP_md5());
            EVP_DigestFinal_ex(t->mdctx, t->md5_digest, &md5_digest_len);
            
            EVP_MD_CTX_free(t->mdctx);

            

            // write N blocks
            uint_to_fourbytes(buffer, s->n_blocks);
            fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
            for(j = 0; j < s->n_blocks; j++) {
                uint_to_fourbytes(buffer, s->n_block_starts[j]);
                fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
            }
            for(j = 0; j < s->n_blocks; j++) {
                uint_to_fourbytes(buffer, s->n_block_starts[j] + s->n_block_sizes[j] - 1);
                fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
            }

            // write checksum
            fh_fastafs.write(reinterpret_cast<char *>(&t->md5_digest), (size_t) 16);

            // write M blocks (masked region; upper/lower case)
            uint_to_fourbytes(buffer, s->m_blocks);
            fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
            for(j = 0; j < s->m_blocks; j++) {
                uint_to_fourbytes(buffer, s->m_block_starts[j]);
                fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
            }
            for(j = 0; j < s->m_blocks; j++) {
                uint_to_fourbytes(buffer, s->m_block_starts[j] + s->m_block_sizes[j] - 1);
                fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
            }
        }

        // save startposition of index
        unsigned int index_file_position = (uint32_t) fh_fastafs.tellp();

        // write index/footer
        uint_to_fourbytes(buffer, (uint32_t) data.size());
        fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
        for(i = 0; i < n; i++) {
            s = data[i];
            t = data2[i];

            // set and write flag
            fastafs_sequence_flags fsf;
            fsf.set_linear();
            fsf.set_dna();
            fsf.set_complete();
            fh_fastafs << fsf.get_bits()[0];
            fh_fastafs << fsf.get_bits()[1];

            // name
            fh_fastafs.write((char *) &s->name_size, (size_t) 1); // name size
            fh_fastafs.write(s->name, (size_t) s->name_size);// name

            // location of sequence data in file
            uint_to_fourbytes(buffer, (uint32_t) t->file_offset_dna_in_ucsc2bit);
            fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
            delete[] s->name;
            delete s;
            delete t;
        }

        // write metadata:
        fh_fastafs << "\x00"s;// no metadata tags (YET)

        // update header: set to updated
        fh_fastafs.seekp(8, std::ios::beg);
        ffsf.set_complete();
        fh_fastafs << ffsf.get_bits()[0];
        fh_fastafs << ffsf.get_bits()[1];

        uint_to_fourbytes(buffer, index_file_position);//position of header
        fh_fastafs.write(reinterpret_cast<char *>(&buffer), (size_t) 4);
    }

    fh_ucsc2bit.close();

    fh_fastafs.seekp(0, std::ios::end);


    fastafs f("");
    f.load(fastafs_file);
    uint32_t crc32c = f.get_crc32();

    unsigned char byte_enc[5] = "\x00\x00\x00\x00";
    uint_to_fourbytes(byte_enc, (uint32_t) crc32c);
    //printf("[%i][%i][%i][%i] input!! \n", byte_enc[0], byte_enc[1], byte_enc[2], byte_enc[3]);
    fh_fastafs.write(reinterpret_cast<char *>(&byte_enc), (size_t) 4);



    size_t written = fh_fastafs.tellp();
    fh_fastafs.close();

    return written;
}

