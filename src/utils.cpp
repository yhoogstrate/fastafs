

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
