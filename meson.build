project('fastafs', 'cpp',
	version : '1.7.0', default_options : ['warning_level=3', 'cpp_std=c++14'])


add_global_arguments('-O3', language : 'cpp')
add_global_arguments('-D_FILE_OFFSET_BITS=64', language : 'cpp')

src = ['./src/fasta_to_fourbit_fastafs.cpp', './src/fasta_to_twobit_fastafs.cpp', './src/flags.cpp', './src/fourbit_byte.cpp', './src/twobit_byte.cpp', './src/ucsc2bit.cpp', './src/ucsc2bit_to_fastafs.cpp', './src/fastafs.cpp', './src/fuse.cpp', './src/utils.cpp', './src/database.cpp', './src/lsfastafs.cpp', './src/main.cpp']

incdir = include_directories('include')

fuse = dependency('fuse')
crypto = dependency('libcrypto')
openssl = dependency('openssl')

executable('fastafs', src, 
							include_directories : incdir,
							dependencies: [crypto, openssl, fuse])

