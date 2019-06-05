# FASTAFS Format specification

The file format consists of four parts:
 - Header, for initiation and format recognition
 - (Compressed) Sequence data
 - Index, metadata required to read sequence data
 - (Optional) metadata

Although it is uncommon, the FASTAFS format denotes the index (a typical header) after the data.
The reason for this location is that most metadata can only be known after converting the entire FASTA file to FASTAFS.
For instance, the number of nucleotides encoded within 2bit data, the number of N-characters or the checksum.
If this metadata would be written in the header located before the sequence data, either:
 - A lot of memory has to be allocated to the heap
 - The FASTA files have to be read twice (once for parsing, once for 2bit conversion)
 - Separate tmp files need to be written to disk and merged afterwards (may cost loads of IO traffic)

## Layout ## 

| Section | Sub | Size | Description |
| ------ | ------ | ------ | ------ |
| GENERIC-HEADER |        |        |        |
|        | MAGIC |        |         |
|        | VERSION-SPECIFICATION | x00 x00 x00 x01 |         |
|        | FASTAFS-FLAG | 2 bytes | metadata flag: is file being written, is file incomplete (has R-Regions) |         |
|        | START-POSITION-OF-INDEX | to index | 
| DATA [per sequence] | --- | --- | --- |
|        | N-COMPRESSED-NUCLEOTIDES | uint32_t | Technical limit is thus 256^4 |
|        | TWOBIT-DATA | char[] | length can be deduced from header |
|        | UNKNOWN-NUCLEOTIDES | uint32_t | Number of N-entries |
|        | N-STARTS | N x uint32_t | start positions (0-based) |
|        | N-ENDS | N x uint32_t | end positions (0-based) |
|        | 1* CHECKSUM | 16 x byte | MD5 |
|        | 2* RESERVED-REGIONS | uint32_t | Number of R-entries (reserved regions ~ incomplete file) |
|        | 2* R-STARTS | N x uint32_t | start positions (0-based) |
|        | 2* R-ENDS | N x uint32_t | end positions (1-based) |
|        | MASKED-NUCLEOTIDES | uint32_t | Number of M-entries (for lower case regions) |
|        | M-STARTS | M x uint32_t | start positions (0-based) - default is CAPITAL, m-blocks are LOWER-case |
|        | M-ENDS | M x uint32_t | end positions (1-based) |
| INDEX  | --- | --- |  |
|        | NUMBER-SEQUENCES | uint32_t | Number of sequences included |
|   -> per sequence | 
|        | SEQUENCE-FLAG | 2 bytes ~ uchar | storing metadata and type of data |
|        | NAME-LENGTH | unsigned char | length in bytes; name cannot exceed 255 bytes |
|        | NAME-FASTA | char[NAME-LENGTH] | FASTA header; may not include new-lines or '>' |
|        | START-POSITION-IN-BODY of N-COMPR-NUC |  | Seems tricky; after two very long sequences this may exceed 4 uints |
| METADATA | by definition optional data |
|          | N-METADATA-TAGS | 1 x char |
| METADATA-ENTRY [per entry] |  ~ limits to 'only' 256 distinct types of metadata
|          | METADATA-TYPE-FLAG | 2 bytes | 
|          | ENTRY | ~ type specific : |
|          | => ORIGINAL PADDING | x uint32_t |


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

#### FASTAFS-FLAG ####

``` 
bit 0   file-complete
bit 1   reserved [most likely for 1 = 64-bit fastafs]
bit 2   reserved
bit 3   reserved
bit 4   reserved
bit 5   reserved
bit 6   reserved
bit 7   reserved
---
bit 8   reserved
bit 9   reserved
bit 10  reserved
bit 11  reserved
bit 12  reserved
bit 13  reserved
bit 14  reserved
bit 15  reserved
```

File-complete set to 1 means that the file writing has completed.
If this value is set to 0, the file is either being written or corrupt (because interrupted write process)

### DATA ###

Repeated for every sequence, in order matching SEQUENCE-HEADER

#### SEQUENCE-FLAG #### 

```
bit 0   is-rna      [1 = yes, 0 = DNA]
bit 1   reserved    [reserved, library type 2 -> protein]
bit 2   reserved    [reserved, library type 2 -> protein]
bit 3   is-complete [1: checksum is present, 0: some regions are reserved but not yet 'downloaded']
bit 4   is-circular 
bit 5   reserved    [reserved for vertical duplicate? located in other file]
bit 6   reserved
bit 7   reserved
---
bit 8   reserved
bit 9   reserved
bit 10  reserved
bit 11  reserved
bit 12  reserved
bit 13  reserved
bit 14  reserved
bit 15  reserved
```

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
