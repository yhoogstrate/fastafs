
## Technical specs

### On-disk file format

```
┌─────────────────────────────────────────────────────────────┐
│  HEADER (14 bytes)                                          │
│    magic (4B) | version (4B) | flags (2B) | index_pos (4B)  │
├─────────────────────────────────────────────────────────────┤
│  DATA SECTION  (repeated per sequence)                      │
│    ┌────────────────────────────────────────────┐           │
│    │ n_nucleotides (4B)                         │           │
│    │ compressed sequence data    ← 2/4/5-bit    │           │
│    │ N-blocks: starts[] + ends[] ← unknown bases│           │
│    │ MD5 checksum (16B)                         │           │
│    │ M-blocks: starts[] + ends[] ← soft masking │           │
│    └────────────────────────────────────────────┘           │
├─────────────────────────────────────────────────────────────┤
│  INDEX SECTION  (at byte position index_pos)                │
│    n_sequences (4B)                                         │
│    per sequence: flags (2B) | name | data_offset (4B)       │
└─────────────────────────────────────────────────────────────┘
```

### Loading an archive: `fastafs::load()`

```
fastafs::load(filename)
        │
        ▼
  open chunked_reader
  (transparent: raw or zstd-seekable)
        │
        ▼
  read header (14 bytes)
   → verify magic bytes
   → retrieve index_pos
        │
        ▼
  seek to INDEX SECTION
   → for each sequence:
       name, flags, data_offset
       → create fastafs_seq object
         (sequence data itself is NOT loaded)
        │
        ▼
  fastafs object ready
  (holds vector<fastafs_seq>)
```

### Decoding a sequence

```
view_fasta() / view_sequence_region("chr1:100-200")
        │
        ▼
  look up fastafs_seq by name
        │
        ▼
  determine encoding type from flags:
  ┌──────────┬──────────────────┬────────────────────────┐
  │  Type    │  Encoding        │  Class                 │
  ├──────────┼──────────────────┼────────────────────────┤
  │  DNA     │  2-bit / nuc     │  twobit_byte_dna       │
  │  RNA     │  2-bit / nuc     │  twobit_byte_rna       │
  │  IUPEC   │  4-bit / nuc     │  fourbit_byte          │
  │  Protein │  5-bit / sym     │  fivebit_fivebytes     │
  └──────────┴──────────────────┴────────────────────────┘
        │
        ▼
  view_fasta_chunk_generalized<T>()
```

### Core decode loop (per nucleotide)

```
for each nucleotide position p:
        │
        ├─► Is p inside an N-block?
        │      yes →  output 'N'
        │      no  ↓
        │
        ├─► Calculate byte offset in compressed data:
        │     effective_pos = p - (skipped N positions)
        │     byte_offset   = effective_pos / nucleotides_per_byte
        │     bit_offset    = effective_pos % nucleotides_per_byte
        │
        ├─► Seek via chunked_reader to:
        │     data_position + 4 + byte_offset
        │
        ├─► Load byte into T object (twobit / fourbit / fivebit)
        │
        ├─► Decode via static lookup table:
        │     ENCODE_HASH[byte_value][bit_offset]  →  character
        │
        ├─► Is p inside an M-block (soft masking)?
        │      yes →  convert to lowercase (ASCII + 32)
        │
        └─► Write to output buffer
              (insert '\n' at FASTA_LINE_LENGTH)
```

### Bit encoding per sequence type

```
2-bit DNA  (4 nucleotides per byte):
  byte: [GG][AA][CC][TT]   bits 7-6, 5-4, 3-2, 1-0
  00=T  01=C  10=A  11=G

4-bit IUPEC  (2 nucleotides per byte):
  byte: [nnnn][nnnn]        high nibble, low nibble
  0000=A  0001=C  0010=G  ...  1111=N

5-bit protein  (8 symbols per 5 bytes = 40 bits):
  [00000|111|11][222|22333][33444|445][55556|6666][77777|???]
  alphabet: "ABCDEFGHIJKLMNOPQRSTUVWYZX*-"
```

### I/O layer: `chunked_reader`

```
chunked_reader
      │
      ├── ContextUncompressed
      │       └── direct fread() + fseek()
      │
      └── ContextZstdSeekable
              └── decompresses only requested frames (1 MB/frame)
                  → random access without full decompression

  internal buffer: 256 KB (release) / 4 KB (debug)
  seek() → refill buffer → read() returns requested bytes
```

### Full decode data flow

```
.fastafs file on disk
        │
        ▼
chunked_reader  (buffered I/O, optional zstd decompression)
        │
        ▼
fastafs::load()  →  fastafs_seq[] (index kept in memory)
        │
        ▼
view_fasta_chunk_generalized<T>()
        │
        ├── N-blocks check  (stored separately, no compressed data consumed)
        ├── byte offset calculation  (O(1) direct seek)
        ├── T::decode  (lookup table, O(1) per byte)
        └── M-blocks check  (lowercase soft masking)
        │
        ▼
FASTA output buffer  →  FUSE layer / stdout
```

The design optimises for **O(1) random access** (direct byte-offset calculation from nucleotide position), **minimal memory use** (streaming output, no full sequence loaded), and **encoding flexibility** via templates for 2-bit, 4-bit, and 5-bit encodings.

