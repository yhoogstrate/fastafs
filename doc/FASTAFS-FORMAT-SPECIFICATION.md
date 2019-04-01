# FASTAFS Format specification

* This file is used in the pre-compiler and directly influences code and compilation.

## Layout ## 

| Section | Sub-1 | Sub-2 | Description |
| ------ | ------ | ------ | ------ |
| GENERIC-HEADER |        |        |        |
|        | MAGIC |  |         |
|        | VERSION-SPECIFICATION | x00 x00 x00 x01 |         |
|        | FLAG | metadata flag: is file being written, is file incomplete (has R-Regions) |         |
|        | START-POSITION-OF-INDEX | to index | 
| DATA [per sequence] | --- | --- | --- |
|        | N-COMPRESSED-NUCLEOTIDES | 4 x unsigned int | Technical limit is thus 256^4 |
|        | TWOBIT-DATA | char[] | length can be deduced from header |
|        | UNKNOWN-NUCLEOTIDES | 4 x unsigned int | Number of N-entries |
|        | N-STARTS | N x 4 x unsigned int | start positions (0-based) |
|        | N-ENDS | N x 4 x unsigned int | end positions (1-based) |
|        | MASKED-NUCLEOTIDES | 4 x unsigned int | Number of M-entries (for lower case regions) |
|        | M-STARTS | M x 4 x unsigned int | start positions (0-based) |
|        | M-ENDS | M x 4 x unsigned int | end positions (1-based) |
|        | RESERVED-REGIONS | 4 x unsigned int | Number of R-entries (reserved regions ~ incomplete file) |
|        | R-STARTS | N x 4 x unsigned int | start positions (0-based) |
|        | R-ENDS | N x 4 x unsigned int | end positions (1-based) |
| INDEX  | --- | --- |  |
|        | NUMBER-SEQUENCES |  | Number of sequences included |
|   -> per sequence | 
|        | FLAG | 4 bytes ~ uint32_t | storing metadata and type of data |
|        | NAME-LENGTH | unsigned char | length in bytes; name cannot exceed 255 bytes |
|        | NAME-FASTA | char[NAME-LENGTH] | FASTA header; may not include new-lines or '>' |
|        | START-POSITION-IN-BODY |  | Seems tricky; after two very long sequences this may exceed 4 uints |
| METADATA | by definition optional data |
|          | N-METADATA-TAGS | 1 x char |
| METADATA-ENTRY [per entry] |  ~ limits to 'only' 256 distinct types of metadata
|          | FLAG | 
|          | ENTRY | ~ type specific : |
|          | => ORIGINAL PADDING | x 4 x unsigned int |


### GENERIC-HEADER ###

#### Magic ####

FA <- HEX
FS <- ASCII

`x0F x0A x46 x53`

```
   +--------+--------+--------+--------+
   |00001111|00001010|01000110|01010011|
   +--------+--------+--------+--------+
```

### DATA ###

Repeated for every sequence, in order matching SEQUENCE-HEADER


### INDEX ###

The index is put to the end because lots of information is unknown during conversion and requires large amounts of RAM allcoated
 

# TODO's #
Add SEQUENCE-TYPE (1 byte):
  * ACTG / ACUG
  * sequence size:
   - 256^4 (uint)  <- current default and only option
   - 256^8 (samtools (cram) ITF8)

Add CRC check in SEQUENCE-HEADER section

Add DEFAULT-PADDING to GENERIC-HEADER

Add tqdm like progress - if possible
