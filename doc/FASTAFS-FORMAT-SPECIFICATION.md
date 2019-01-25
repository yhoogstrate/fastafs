# FASTAFS Format specification

* This file is used in the pre-compiler and directly influences code and compilation.

## Layout ## 

| Section | Subsection | Content | Description |
| ------ | ------ | ------ | ------ |
| GENERIC-HEADER |        |        |        |
|        | MAGIC |  x0F x0A x46 x53 |         |
|        | VERSION-SPECIFICATION | x00 x00 x00 x01 |         |
|        | NUMBER-SEQUENCES |  | Number of sequences included |
|        | RESERVED  | x00 x00 x00 x00 | Legacy, but should become CRC check :) |
| SEQUENCE-HEADER|        |        | Repeated for every sequence |
|        | NAME-LENGTH | unsigned char | length in bytes; name cannot exceed 255 bytes |
|        | NAME-FASTA | char[NAME-LENGTH] | FASTA header; may not include new-lines or '>' |
|        | SHA1 | 20 bytes |      | Needs to be calculated according to the CRAM definition: only nucleotides, only upper-case. - https://www.ebi.ac.uk/ena/software/cram-reference-registry |
|        | START-POSITION-IN-FILE |  | Seems tricky; after two very long sequences this may exceed 4 uints |
| BODY   |  |  | Repeated for every sequence, in order matching SEQUENCE-HEADER |
|        | SEQUENCE-LENGTH | 4 x unsigned int | Technical limit is thus 256^4 |
|        | UNKNOWN-NUCLEOTIDES | 4 x unsigned int | Number of N-entries |
|        | N-STARTS | N x 4 x unsigned int | start positions (0-based) |
|        | N-ENDS | N x 4 x unsigned int | end positions (1-based) |
|        | MASKED-NUCLEOTIDES | 4 x unsigned int | Number of M-entries (for lower case regions) |
|        | M-STARTS | M x 4 x unsigned int | start positions (0-based) |
|        | M-ENDS | M x 4 x unsigned int | end positions (1-based) |
|        | TWOBIT-DATA | char[] | length can be deduced from header |

# TODO's
Add SEQUENCE-TYPE (1 byte):
  * ACTG / ACUG
  * sequence size:
   - 256^4 (uint)  <- current default and only option
   - 256^8 (samtools ITF8)

Add CRC check in SEQUENCE-HEADER section

Add DEFAULT-PADDING to GENERIC-HEADER

Add tqdm like progress - if possible
