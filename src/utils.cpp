
#include <iostream>

#include <openssl/sha.h>

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

    uint16_t u = ((unsigned char)  chars[0] << 8) | ((unsigned char) chars[1]);
    return u;
}


/*
 * chars: pointer to 2 chars
 * n: integer that needs to be converted
 */
void uint_to_twobytes(char *chars, uint16_t n)
{
    chars[0] = (signed char) ((n >> 8) & 0xFF);
    chars[1] = (signed char) (n & 0xFF);
}





void uint_to_fourbytes(char *chars, uint32_t n)
{
    chars[0] = (signed char) ((n >> 24) & 0xFF);
    chars[1] = (signed char) ((n >> 16) & 0xFF);
    chars[2] = (signed char) ((n >> 8) & 0xFF);
    chars[3] = (signed char) (n & 0xFF);
}


// ucsc2bit is somehow stringreverted
void uint_to_fourbytes_ucsc2bit(char *chars, uint32_t n)
{
    chars[3] = (signed char) ((n >> 24) & 0xFF);
    chars[2] = (signed char) ((n >> 16) & 0xFF);
    chars[1] = (signed char) ((n >> 8) & 0xFF);
    chars[0] = (signed char) (n & 0xFF);
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
    while (size > 1024) {
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

    //return void;
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

    if ((fp = fopen(filename, "rb")) == NULL) {
        throw std::runtime_error("Could not read first byte of putative FASTA file.");

        return false;
    }

    if (fread(buf, 1, 2, fp) == 2) {
        return (buf[0] == '>');// return true if first byte equals >
    }

    fclose(fp);
}

