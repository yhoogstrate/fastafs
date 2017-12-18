
#include <iostream>

#include "config.hpp"

unsigned int fourbytes_to_uint(char *chars, unsigned char offset)
{

    unsigned int u = ((unsigned char) chars[0 + offset] << 24) | ((unsigned char) chars[1 + offset] << 16) | ((unsigned char)  chars[2 + offset] << 8) | ((unsigned char) chars[3 + offset]);

    return u;
}


void uint_to_fourbytes(char *chars, unsigned int n)
{
    chars[0] = (unsigned char) ((n >> 24) & 0xFF);
    chars[1] = (unsigned char) ((n >> 16) & 0xFF);
    chars[2] = (unsigned char) ((n >> 8) & 0xFF);
    chars[3] = (unsigned char) (n & 0xFF);
}



/**
 * @brief parses mtab and searches for al fastafs mounts
 "/home/users/youri/.local/share/fastafs/test.fastafs -> ["/mnt/fastafs/test/",....]
 */
std::string get_mtab() {
    
    
    
    std::string out = "";
    return out;
}



char* human_readable_fs(unsigned int size, char *buf) {
    unsigned int i = 0;
    const char* units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1024) {
        size /= 1024;
        i++;
    }
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return buf;
}
