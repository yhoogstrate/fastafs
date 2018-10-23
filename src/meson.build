
libfastafs = shared_library(
    'fastafs', # library name
    'database.cpp', 'fastafs.cpp', 'fasta_to_fastafs.cpp', 'fuse.cpp', 'twobit_byte.cpp', 'utils.cpp', # source files to be compiled
    include_directories : inc,
    dependencies: [fuse_dep, sha_dep],
    install : true
    )



fastafs = executable(
    'fastafs',
    'main.cpp',
    include_directories: inc,
    dependencies: [fuse_dep, sha_dep],
    link_with: libfastafs,
    install : true
    )
