
unsigned int fourbytes_to_uint(char *, unsigned char);
void uint_to_fourbytes(char *, unsigned int);
void uint_to_fourbytes_ucsc2bit(char *, unsigned int);
char *human_readable_fs(unsigned int, char *);
void sha1_digest_to_hash(unsigned char *, char *);

std::string std_string_nullbyte_safe(char *, size_t, size_t);
std::string std_string_nullbyte_safe(char *, size_t);
