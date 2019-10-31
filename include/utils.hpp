
uint32_t fourbytes_to_uint(char *, unsigned char);
uint32_t fourbytes_to_uint_ucsc2bit(char *, unsigned char);

// for flags
uint16_t twobytes_to_uint(char *);
void uint_to_twobytes(char *chars, uint16_t n);

void uint_to_fourbytes(char *, uint32_t);
void uint_to_fourbytes_ucsc2bit(char *, uint32_t);

char *human_readable_fs(uint32_t, char *);
void sha1_digest_to_hash(unsigned char *, char *);
void md5_digest_to_hash(unsigned char *, char *);

std::string std_string_nullbyte_safe(char *, size_t, size_t);
std::string std_string_nullbyte_safe(char *, size_t);

bool is_fasta_file(char *filename);

std::string basename_cpp(std::string);
std::string realpath_cpp(std::string);

