project('fastafs', 'cpp')

src = ['./src/fasta_to_fourbit_fastafs.cpp', './src/fasta_to_twobit_fastafs.cpp', './src/flags.cpp', './src/fourbit_byte.cpp', './src/twobit_byte.cpp', './src/ucsc2bit.cpp', './src/ucsc2bit_to_fastafs.cpp', './src/fastafs.cpp', './src/fuse.cpp', './src/utils.cpp', './src/database.cpp', './src/lsfastafs.cpp', './src/main.cpp']

incdir = include_directories('include')

fuse = dependency('fuse')
crypto = dependency('libcrypto')
openssl = dependency('openssl')

executable('fastafs', src, 
							include_directories : incdir,
							dependencies: [crypto, openssl, fuse])

