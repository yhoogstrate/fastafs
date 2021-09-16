
#include <fstream>
#include <iostream>
#include <libgen.h>
#include <string.h>

#include <openssl/sha.h>
#include <openssl/md5.h>

#include <sys/stat.h>

#include "zlib.h"

#include "config.hpp"

// as these chars are coming from ifstream.read
uint32_t fourbytes_to_uint(char *chars, unsigned char offset)
{
    uint32_t u = ((unsigned char) chars[0 + offset] << 24) | ((unsigned char) chars[1 + offset] << 16) | ((unsigned char)  chars[2 + offset] << 8) | ((unsigned char) chars[3 + offset]);
    return u;
}


/*
for some reason, ucsc2bit writes down an int in reversed order.

the equivalent of 1 (one) is as follows:
00000001 00000000 00000000 00000000

the equivalent of 128 in their encoding is as follows:
10000000 00000000 00000000 00000000

the equivalent of 129 in their encoding is as follows:
10000000 00000001 00000000 00000000


The function below decodes these 4 charred strings into an uint32_teger
*/
uint32_t fourbytes_to_uint_ucsc2bit(char *chars, unsigned char offset)
{
    uint32_t u = ((unsigned char) chars[3 + offset] << 24) | ((unsigned char) chars[2 + offset] << 16) | ((unsigned char)  chars[1 + offset] << 8) | ((unsigned char) chars[0 + offset]);

    return u;
}



/*
 * chars: pointer to 2 chars
 */
uint16_t twobytes_to_uint(char *chars)
{
    uint16_t u = (uint16_t)(((unsigned char) chars[0] << 8) | ((unsigned char) chars[1]));

    return u;
}


/*
 * chars: pointer to 2 chars
 * n: integer that needs to be converted
 */
void uint_to_twobytes(char *chars, uint16_t n)
{
    chars[0] = (signed char)((n >> 8) & 0xFF);
    chars[1] = (signed char)(n & 0xFF);
}



size_t remove_chars(char *s, int c, size_t l)
{
    size_t j = 0;
    size_t n = l;

    for(size_t i = j = 0; i < n; i++)
        if(s[i] != c) {
            s[j++] = s[i];
        }

    s[j] = '\0';

    return j;
}


void uint_to_fourbytes(char *chars, uint32_t n)
{
    chars[0] = (signed char)((n >> 24) & 0xFF);
    chars[1] = (signed char)((n >> 16) & 0xFF);
    chars[2] = (signed char)((n >> 8) & 0xFF);
    chars[3] = (signed char)(n & 0xFF);
}


// ucsc2bit is somehow stringreverted
void uint_to_fourbytes_ucsc2bit(char *chars, uint32_t n)
{
    chars[3] = (signed char)((n >> 24) & 0xFF);
    chars[2] = (signed char)((n >> 16) & 0xFF);
    chars[1] = (signed char)((n >> 8) & 0xFF);
    chars[0] = (signed char)(n & 0xFF);
}



/**
 * @brief parses mtab and searches for al fastafs mounts
 "/home/users/youri/.local/share/fastafs/test.fastafs -> ["/mnt/fastafs/test/",....]
 */
std::string get_mtab()
{
    std::string out = "";
    return out;
}



char *human_readable_fs(uint32_t bitsize, char *buf)
{
    float size = (float) bitsize;
    uint32_t i = 0;
    const char *units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while((size > 1024) & (i < 9)) {
        size /= 1024;
        i++;
    }
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return buf;
}


void sha1_digest_to_hash(unsigned char *digest, char *hash)
{
    //unsigned char hash[SHA_DIGEST_LENGTH];
    //SHA1_Final(hash, &ctx);
    for(uint32_t i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(hash + (i * 2), "%02x", digest[i]);
    }
    hash[40] = 0;
}

void md5_digest_to_hash(unsigned char *digest, char *hash)
{
    for(uint32_t i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(hash + (i * 2), "%02x", digest[i]);
    }
    hash[32] = 0;
}


std::string std_string_nullbyte_safe(char *ref, size_t pos, size_t len)
{
    std::string s = "";
    for(size_t i = pos; i < len; i++) {
        s.push_back(ref[i]);
    }
    return s;
}


std::string std_string_nullbyte_safe(char *ref, size_t len)
{
    std::string s = "";
    for(size_t i = 0; i < len; i++) {
        s.push_back(ref[i]);
    }
    return s;
}


bool is_fasta_file(char *filename)
{
    char buf[2];
    FILE *fp;

    if((fp = fopen(filename, "rb")) == NULL) {
        //fclose(fp); segfault if NULL
        throw std::runtime_error("Could not read first byte of putative FASTA file.");
        return false;
    }

    if(fread(buf, 1, 2, fp) == 2) {
        fclose(fp);
        return (buf[0] == '>');// return true if first byte equals >
    } else {
        fclose(fp);

        throw std::runtime_error("Could not read sufficient data.");
    }

    return false;
}



bool is_ucsc2bit_file(char *filename)
{
    char buf[4 + 1];
    FILE *fp;

    if((fp = fopen(filename, "rb")) == NULL) {
        //fclose(fp); segfault if NULL
        throw std::runtime_error("Could not read first byte of file.");
        return false;
    }

    if(fread(buf, 1, 4, fp) == 4) {
        fclose(fp);
        
        
        return UCSC2BIT_MAGIC.compare(0, 4, buf) == 0;
        //return (
                   //buf[0] == UCSC2BIT_MAGIC[0] and
                   //buf[1] == UCSC2BIT_MAGIC[1] and
                   //buf[2] == UCSC2BIT_MAGIC[2] and
                   //buf[3] == UCSC2BIT_MAGIC[3]
               //);// return true if first byte equals >
    } else {
        fclose(fp);

        throw std::runtime_error("Could not read sufficient data.");
    }

    return false;
}



bool is_zstd_file(const char *filename)
{
    char buf[4 + 1];
    FILE *fp;

    if((fp = fopen(filename, "rb")) == NULL) {
        //fclose(fp); segfault if NULL
        throw std::runtime_error("Could not read first byte of file.");
        return false;
    }

    if(fread(buf, 1, 4, fp) == 4) {
        fclose(fp);

        return ZSTD_MAGIC.compare(0, 4, buf) == 0;

        //return (
                   //buf[0] == ZSTD_MAGIC[0] and
                   //buf[1] == ZSTD_MAGIC[1] and
                   //buf[2] == ZSTD_MAGIC[2] and
                   //buf[3] == ZSTD_MAGIC[3]
               //);// return true if first byte equals >
    } else {
        fclose(fp);

        throw std::runtime_error("Could not read sufficient data.");
    }

    return false;
}



// https://www.systutorials.com/241216/how-to-get-the-directory-path-and-file-name-from-a-absolute-path-in-c-on-linux/
// https://stackoverflow.com/questions/38456127/what-is-the-value-of-cplusplus-for-c17 - THEN use std::filesystem::path(filename).filename();
std::string basename_cpp(std::string fn)
{
    char* ts = strdup(fn.c_str());

    //char* dir = dirname(ts1);
    char* filename = basename(ts);
    //std::string filenamepp = std::string(filename);

    //printf("basename: [%s]\n", filename);
    //std::cout << "basenamepp: |" << filenamepp << "|\n";

    std::string filename_cpp = std::string(filename);
    //delete[] ts;
    //delete[] filename; // deleting these affects the std::string somehow

    return filename_cpp;
}


// https://www.linuxquestions.org/questions/programming-9/how-to-get-the-full-path-of-a-file-in-c-841046/
// https://stackoverflow.com/questions/38456127/what-is-the-value-of-cplusplus-for-c17 - THEN use std::filesystem::canonical(filename)
std::string realpath_cpp(std::string fn)
{
    char buf[1024];
    realpath(fn.c_str(), buf);

    return std::string(buf);
}



// it is important that it does not read beyond 'len'
uint32_t file_crc32(const std::string &fname, off_t start, size_t len)
{
    uLong crc = crc32(0L, Z_NULL, 0);

    std::ifstream fh_fastafs_crc(fname.c_str(), std::ios::out | std::ios::binary);
    if(fh_fastafs_crc.is_open()) {
        fh_fastafs_crc.seekg(start, std::ios::beg);// skip magic number, this must be ok otherwise the toolkit won't use the file anyway

        char buffer[READ_BUFFER_SIZE + 1];
        size_t to_read = len - start;

        for(size_t i = 0; i < to_read / READ_BUFFER_SIZE; i++) {
            if(fh_fastafs_crc.read(buffer, READ_BUFFER_SIZE) &&  fh_fastafs_crc.gcount() == READ_BUFFER_SIZE) {
                crc = crc32(crc, (const Bytef*)& buffer, READ_BUFFER_SIZE);
            } else {
                throw std::invalid_argument("[file_crc32:1] Truncated file (early EOF): " + fname);
            }
        }

        to_read = to_read % READ_BUFFER_SIZE;
        if(to_read > 0) {
            if(fh_fastafs_crc.read(buffer, to_read) && (size_t) fh_fastafs_crc.gcount() == to_read) {
                crc = crc32(crc, (const Bytef*)& buffer, (unsigned int) to_read);
            } else {
                throw std::invalid_argument("[file_crc32:2] Truncated file (early EOF): " + fname);
            }
        }
    } else {
        throw std::invalid_argument("[file_crc32:3] Could not open file: " + fname);
    }

    return (uint32_t) crc;
}


bool file_exist(const char *fileName)
{
    //moe classical but slower implementation
    //std::ifstream infile(fileName);
    //return infile.good();
    
    //following implementation should be faster
    struct stat buffer;
    return (stat (fileName, &buffer) == 0);
}
