project('fastafs', 'cpp',
    version : run_command('bash', '-c' , 'grep PROJECT_VERSION CMakeLists.txt | grep -Po \'".+"\' | grep -Po \'[^"]+\'').stdout().strip(), default_options : ['warning_level=3', 'cpp_std=c++14'])

project_version = run_command('bash', '-c' , 'grep PROJECT_VERSION CMakeLists.txt | grep -Po \'".+"\' | grep -Po \'[^"]+\'').stdout().strip()


add_global_arguments('-O3', language : 'cpp')
add_global_arguments('-D_FILE_OFFSET_BITS=64', language : 'cpp')
add_global_arguments('-static', language : 'cpp')


# make config:
# prefix = get_option('prefix')
# https://mesonbuild.com/Configuration.html#a-full-example
conf_data = configuration_data()
conf_data.set('PROJECT_VERSION', project_version)
conf_data.set('CMAKE_PROJECT_NAME', 'fastafs')
conf_data.set('DEBUG', 'false')
configure_file(input : 'include/config.hpp.in',
               output : 'config.hpp',
               configuration : conf_data)
configuration_inc = include_directories('include')







src = [
'./dependencies/zstd-lib-common/xxhash.cpp',
'./dependencies/zstd-seekable-adapted/zstdseek_compress.cpp', './dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp', './dependencies/zstd-seekable-adapted/zstdseek_utils.cpp',
'./src/chunked_reader.cpp', './src/database.cpp', './src/fastafs.cpp', './src/fasta_to_fastafs.cpp', 'src/byte_decoder.cpp', './src/xbit_byte_encoder.cpp', './src/fivebit_fivebytes.cpp', './src/flags.cpp', './src/fourbit_byte.cpp', './src/fuse.cpp', './src/sequence_region.cpp', './src/twobit_byte.cpp', './src/ucsc2bit.cpp', './src/ucsc2bit_to_fastafs.cpp', './src/utils.cpp', './src/lsfastafs.cpp',
]
 
# src__test_check = [
# './dependencies/zstd-lib-common/xxhash.cpp',
# './dependencies/zstd-seekable-adapted/zstdseek_compress.cpp', './dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp', './dependencies/zstd-seekable-adapted/zstdseek_utils.cpp',
# './src/chunked_reader.cpp', './src/database.cpp', './src/fastafs.cpp', './src/fasta_to_fastafs.cpp', './src/xbit_byte_encoder.cpp', './src/fivebit_fivebytes.cpp', './src/flags.cpp', './src/fourbit_byte.cpp', './src/fuse.cpp', './src/sequence_region.cpp', './src/twobit_byte.cpp', './src/ucsc2bit.cpp', './src/ucsc2bit_to_fastafs.cpp', './src/utils.cpp', './src/lsfastafs.cpp',
# ] 

# src__test_cache = [
# './dependencies/zstd-lib-common/xxhash.cpp',
# './dependencies/zstd-seekable-adapted/zstdseek_compress.cpp', './dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp', './dependencies/zstd-seekable-adapted/zstdseek_utils.cpp',
# './src/chunked_reader.cpp', './src/database.cpp', './src/fastafs.cpp', './src/fasta_to_fastafs.cpp', './src/xbit_byte_encoder.cpp', './src/fivebit_fivebytes.cpp', './src/flags.cpp', './src/fourbit_byte.cpp', './src/fuse.cpp', './src/sequence_region.cpp', './src/twobit_byte.cpp', './src/ucsc2bit.cpp', './src/ucsc2bit_to_fastafs.cpp', './src/utils.cpp', './src/lsfastafs.cpp',
# './test/cache/test_cache.cpp'
# ]

# src__test_chunked_reader = [
# './dependencies/zstd-lib-common/xxhash.cpp',
# './dependencies/zstd-seekable-adapted/zstdseek_compress.cpp', './dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp', './dependencies/zstd-seekable-adapted/zstdseek_utils.cpp',
# './src/chunked_reader.cpp', './src/database.cpp', './src/fastafs.cpp', './src/fasta_to_fastafs.cpp', './src/xbit_byte_encoder.cpp', './src/fivebit_fivebytes.cpp', './src/flags.cpp', './src/fourbit_byte.cpp', './src/fuse.cpp', './src/sequence_region.cpp', './src/twobit_byte.cpp', './src/ucsc2bit.cpp', './src/ucsc2bit_to_fastafs.cpp', './src/utils.cpp', './src/lsfastafs.cpp',
# './test/chunked_reader/test_chunked_reader.cpp'
# ]


incdir = include_directories('dependencies/zstd-seekable-adapted', './dependencies/zstd-lib-common', 'include')


fuse = dependency('fuse')
crypto = dependency('libcrypto')
openssl = dependency('openssl')
zlib = dependency('zlib')
zstd = dependency('libzstd')


libfastafs = library('libfastafs', [src],
							version: project_version,
							include_directories : incdir,
							dependencies: [crypto, openssl, fuse, zlib, zstd])

executable('fastafs', ['./src/main.cpp'],
							link_with : libfastafs,
							include_directories : incdir,
							dependencies: [crypto, openssl, fuse, zlib, zstd])

executable('test_cache', ['./test/check/test_check.cpp'],
							link_with : libfastafs,
							include_directories : incdir,
							dependencies: [crypto, openssl, fuse, zlib, zstd])

executable('test_check', ['./test/cache/test_cache.cpp'],
							link_with : libfastafs,
							include_directories : incdir,
							dependencies: [crypto, openssl, fuse, zlib, zstd])

executable('test_chunked_reader', ['./test/chunked_reader/test_chunked_reader.cpp'],
							link_with : libfastafs,
							include_directories : incdir,
							dependencies: [crypto, openssl, fuse, zlib, zstd])

executable('test_byte_decoder', ['./test/byte_decoder/test_byte_decoder.cpp'],
							link_with : libfastafs,
							include_directories : incdir,
							dependencies: [crypto, openssl, fuse, zlib, zstd])

