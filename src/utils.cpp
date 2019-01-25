
#include <iostream>

#include "config.hpp"

// as these chars are coming from ifstream.read
unsigned int fourbytes_to_uint(char *chars, unsigned char offset)
{

    unsigned int u = ((unsigned char) chars[0 + offset] << 24) | ((unsigned char) chars[1 + offset] << 16) | ((unsigned char)  chars[2 + offset] << 8) | ((unsigned char) chars[3 + offset]);

    return u;
}


void uint_to_fourbytes(char *chars, unsigned int n)
{
    chars[0] = (signed char) ((n >> 24) & 0xFF);
    chars[1] = (signed char) ((n >> 16) & 0xFF);
    chars[2] = (signed char) ((n >> 8) & 0xFF);
    chars[3] = (signed char) (n & 0xFF);
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



char *human_readable_fs(unsigned int bitsize, char *buf)
{
    float size = (float) bitsize;
    unsigned int i = 0;
    const char *units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1024) {
        size /= 1024;
        i++;
    }
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return buf;
}
