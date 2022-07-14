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
|        | [MAGIC](#magic) | 4 bytes | `x0F x0A x46 x53` |
|        | [FILE FORMAT VERSION](#file-format-version) | [4-byte integer](#four-byte-integer) | `x00 x00 x00 x00` |
|        | [FASTAFS-FLAGS](#fastafs-flags) | 2 bytes | Certain binary flags |
|        | [FILE-POSITION-OF-INDEX](#file-position-of-the-index) | [4-byte integer](#four-byte-integer) | Location in the file (offset in bytes from beginning) where the INDEX is located | 
| DATA | --- | --- | --- |
|   -> per sequence | 
|        | N-COMPRESSED-NUCLEOTIDES | uint32_t as [4-byte integer](#four-byte-integer) | The number of 2bit compressed nucleotides. Technical limit is 256^4 |
|        | [TWOBIT-DATA](#twobit-data) | sequence of 2bit-bytes | length can be deduced from header |
|        | UNKNOWN-NUCLEOTIDES | uint32_t as [4-byte integer](#four-byte-integer) | Number of N-entries |
|        | N-STARTS | N x uint32_t as [4-byte integer](#four-byte-integer) | start positions (0-based) |
|        | N-ENDS | N x uint32_t as [4-byte integer](#four-byte-integer) | end positions (0-based) |
|        | [MD5-CHECKSUM](#md5-checksum) | 16 x byte | MD5 compatible with CRAM, BAM, DICT & ENA |
|        | ** RESERVED-REGIONS | uint32_t as [4-byte integer](#four-byte-integer) | Number of R-entries (reserved regions ~ incomplete file) - not yet implemented and must be enabled by a flag |
|        | ** R-STARTS | N x uint32_t as [4-byte integer](#four-byte-integer) | start positions (0-based) |
|        | ** R-ENDS | N x uint32_t as [4-byte integer](#four-byte-integer) | end positions (1-based) |
|        | MASKED-NUCLEOTIDES | uint32_t as [4-byte integer](#four-byte-integer) | Number of M-entries (for lower case regions) |
|        | M-STARTS | M x uint32_t as [4-byte integer](#four-byte-integer) | start positions (0-based) - default is CAPITAL, within M-blocks is LOWER-case |
|        | M-ENDS | M x uint32_t as [4-byte integer](#four-byte-integer) | end positions (0-based) |
| INDEX  | --- | --- |  |
|        | NUMBER-SEQUENCES | uint32_t as [4-byte integer](#four-byte-integer) | Number of sequences included |
|   -> per sequence | 
|        | [SEQUENCE-FLAGS](#sequence-flags) | 2 bytes | storing metadata and type of data |
|        | NAME-LENGTH | 1 byte as unsigned char | length in bytes; name cannot exceed 255 bytes |
|        | NAME-FASTA | NAME-LENGTH x char | FASTA header; may not include new-lines or '>' |
|        | START-POSITION-IN-BODY of N-COMPR-NUC | uint32_t as [4-byte integer](#four-byte-integer) | Location in the file (offset in bytes from beginning) where the DATA block for this sequence starts |
| METADATA | only first char is required, the rest is always optional |
|          | N-METADATA-TAGS | 1 x char |
| METADATA-ENTRY [per entry] |  ~ limits to 'only' 256 distinct types of metadata
|          | METADATA-TYPE-FLAG | 2 bytes | 
|          | ENTRY | type specific, examples below: |
|          | => ORIGINAL PADDING | uint32_t as [4-byte integer](#four-byte-integer) | The number of nucleotides per line in the original FASTA file |
| CRC32  | Checksum on entire file | 4 bytes | To ensure whole file integrity |

### GENERIC-HEADER ###

#### Magic ####

The file magic (first four bytes used to recognise binary file types) of FASTAFS are `x0F x0A x46 x53`.
They stand for `FA` (as HEX) and `FS` (in ASCII).
The bit representation of these bytes are:

```
    +--------+--------+--------+--------+
    |00001111|00001010|01000110|01010011|
    +--------+--------+--------+--------+
```

#### FILE FORMAT VERSION ####

The version of the file format specification implemented as four byte integer. Currently only one version exists: 

`x00 x00 x00 x00`

The bit representation of these bytes are:

```
    +--------+--------+--------+--------+
    |00000000|00000000|00000000|00000000|
    +--------+--------+--------+--------+
```

#### FASTAFS-FLAGS ####

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


#### File position of the index ####

The index is located at the end of the data. This file offset in bytes from the files start position is indicated as [4-byte integer](#four-byte-integer).


### DATA ###

Repeated for every sequence, in order matching SEQUENCE-HEADER

#### SEQUENCE-FLAGS #### 

The sequence flag allows to describe the following metadata for each sequence:

```
bit 0   combined sequence type
bit 1   combined sequence type
```

| bit-0 | bit-1 | Type | Alphabet |
| ---- | ---- | - | - |
| `0` | `0` | DNA | `ACTG` + `N` |
| `1` | `0` | RNA | `ACUG` + `N` |
| `0` | `1` | IUPEC Nucleotide | `ACGTURYKMSWBDHVN` + `-` |
| `1` | `1` | reserved for protein | to be determined |

```
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

#### TwoBit Data ####

The TwoBit data is encoded in a long array of bytes in which each byte encodes four nucleotides.

The following bits to nucleotide encoding is used:

| bits | Nucleotide |
| ---- | - |
| `00` | T |
| `01` | C |
| `10` | A |
| `11` | G |

Encoded into a byte in the following order:

```
    +----------+------+
    | 00000011 | TTTG |
    +----------+------+
    | 00001100 | TTGT |
    +----------+------+
    | 00110000 | TGTT |
    +----------+------+
    | 11000000 | GTTT |
    +----------+------+
```

#### MD5 checksum ####

Per sequence, an MD5 checksum is stored as it's binary encoded digest.

The MD5 checksum is calculated as described in the CRAM specification (section 11): https://samtools.github.io/hts-specs/CRAMv3.pdf

Only nucleotides are included (no newlines, no whitespaces, no headers), all in uppercase.


### INDEX ###

The index is put to the end because lots of information is unknown during conversion and requires large amounts of RAM allcoated

#### Four Byte Integer ####

A four byte integer is a binary encoded integer value (using 4 bytes).

 - A 0 is encoded as follows (`x00 x00 x00 x00`):

```
    +--------+--------+--------+--------+
    |00000000|00000000|00000000|00000000|
    +--------+--------+--------+--------+
```

 - A 1 is encoded as follows (`x00 x00 x00 x01`):

```
    +--------+--------+--------+--------+
    |00000000|00000000|00000000|00000001|
    +--------+--------+--------+--------+
```

 - A 2 is encoded as follows (`x00 x00 x00 x02`):

```
    +--------+--------+--------+--------+
    |00000000|00000000|00000000|00000010|
    +--------+--------+--------+--------+
```

 - A 3 is encoded as follows (`x00 x00 x00 x03`):

```
    +--------+--------+--------+--------+
    |00000000|00000000|00000000|00000011|
    +--------+--------+--------+--------+
```

 - A 255 is encoded as follows (`x00 x00 x00 xFF`):

```
    +--------+--------+--------+--------+
    |00000000|00000000|00000000|11111111|
    +--------+--------+--------+--------+
```

 - A 256 is encoded as follows (`x00 x00 x01 x00`):

```
    +--------+--------+--------+--------+
    |00000000|00000000|00000001|00000000|
    +--------+--------+--------+--------+
```

 - A 257 is encoded as follows (`x00 x00 x01 x01`):

```
    +--------+--------+--------+--------+
    |00000000|00000000|00000001|00000001|
    +--------+--------+--------+--------+
```

Denote that this implementation is DIFFERENT than for the [UCSC implementation](#four-byte-integer-ucscu-implementation)!


#### Four Byte Integer UCSC implementation ####

A four byte integer is a binary encoded integer value (using 4 bytes). 


 - A 0 is encoded as follows (`x00 x00 x00 x00`):

```
    +--------+--------+--------+--------+
    |00000000|00000000|00000000|00000000|
    +--------+--------+--------+--------+
```

 - A 1 is encoded as follows (`x01 x00 x00 x00`) - DENOTE INVERSED ORDER OF INTEGERS:

```
    +--------+--------+--------+--------+
    |00000001|00000000|00000000|00000000|
    +--------+--------+--------+--------+
```

 - A 2 is encoded as follows (`x02 x00 x00 x00`) - DENOTE INVERSED ORDER OF INTEGERS:

```
    +--------+--------+--------+--------+
    |00000010|00000000|00000000|00000000|
    +--------+--------+--------+--------+
```

 - A 3 is encoded as follows (`x03 x00 x00 x00`) - DENOTE INVERSED ORDER OF INTEGERS:

```
    +--------+--------+--------+--------+
    |00000011|00000000|00000000|00000000|
    +--------+--------+--------+--------+
```

 - A 255 is encoded as follows (`xFF x00 x00 x00`) - DENOTE INVERSED ORDER OF INTEGERS:

```
    +--------+--------+--------+--------+
    |11111111|00000000|00000000|00000000|
    +--------+--------+--------+--------+
```

 - A 256 is encoded as follows (`x00 x01 x00 x00`) - DENOTE INVERSED ORDER OF INTEGERS:

```
    +--------+--------+--------+--------+
    |00000000|00000001|00000000|00000000|
    +--------+--------+--------+--------+
```

 - A 257 is encoded as follows (`x01 x01 x00 x00`) - DENOTE INVERSED ORDER OF INTEGERS:

```
    +--------+--------+--------+--------+
    |00000001|00000001|00000000|00000000|
    +--------+--------+--------+--------+
```


Denote that this implementation is DIFFERENT than for the FASTAFS implementation!
 

# TODO's #
Add SEQUENCE-TYPE (1 byte):
  * ACTG / ACUG
  * sequence size:
    - 256^4 (uint)  <- current default and only option
    - 256^8 (samtools (cram) ITF8)

Add DEFAULT-PADDING to GENERIC-HEADER

Add tqdm like progress - if possible
