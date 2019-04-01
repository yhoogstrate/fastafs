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

| Section | Sub-1 | Sub-2 | Description |
| ------ | ------ | ------ | ------ |
| GENERIC-HEADER |        |        |        |
|        | MAGIC |  |         |
|        | VERSION-SPECIFICATION | x00 x00 x00 x01 |         |
|        | FASTAFS-FLAG | metadata flag: is file being written, is file incomplete (has R-Regions) |         |
|        | START-POSITION-OF-INDEX | to index | 
| DATA [per sequence] | --- | --- | --- |
|        | N-COMPRESSED-NUCLEOTIDES | 4 x unsigned int | Technical limit is thus 256^4 |
|        | TWOBIT-DATA | char[] | length can be deduced from header |
|        | UNKNOWN-NUCLEOTIDES | 4 x unsigned int | Number of N-entries |
|        | N-STARTS | N x 4 x unsigned int | start positions (0-based) |
|        | N-ENDS | N x 4 x unsigned int | end positions (1-based) |
|        | 1* CHECKSUM | 20 x byte | SHA1 |
|        | 2* RESERVED-REGIONS | 4 x unsigned int | Number of R-entries (reserved regions ~ incomplete file) |
|        | 2* R-STARTS | N x 4 x unsigned int | start positions (0-based) |
|        | 2* R-ENDS | N x 4 x unsigned int | end positions (1-based) |
|        | MASKED-NUCLEOTIDES | 4 x unsigned int | Number of M-entries (for lower case regions) |
|        | M-STARTS | M x 4 x unsigned int | start positions (0-based) |
|        | M-ENDS | M x 4 x unsigned int | end positions (1-based) |
| INDEX  | --- | --- |  |
|        | NUMBER-SEQUENCES |  | Number of sequences included |
|   -> per sequence | 
|        | SEQUENCE-FLAG | 1 bytes ~ uchar | storing metadata and type of data |
|        | NAME-LENGTH | unsigned char | length in bytes; name cannot exceed 255 bytes |
|        | NAME-FASTA | char[NAME-LENGTH] | FASTA header; may not include new-lines or '>' |
|        | START-POSITION-IN-BODY |  | Seems tricky; after two very long sequences this may exceed 4 uints |
|        | sequence SHA1 checksum | 20? bytes? | 20x \00 if reserved regions are set
| METADATA | by definition optional data |
|          | N-METADATA-TAGS | 1 x char |
| METADATA-ENTRY [per entry] |  ~ limits to 'only' 256 distinct types of metadata
|          | METADATA-TYPE-FLAG | 
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
bit 5   reserved
bit 6   reserved
bit 7   reserved
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
