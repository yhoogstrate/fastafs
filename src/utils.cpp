

unsigned int fourbytes_to_uint(char *chars, unsigned char offset)
{

    unsigned int u = ((unsigned char) chars[0 + offset] << 24) | ((unsigned char) chars[1 + offset] << 16) | ((unsigned char)  chars[2 + offset] << 8) | ((unsigned char) chars[3 + offset]);

    return u;
}

