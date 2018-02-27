project('fastafs', 'cpp',
    default_options : ['cpp_std=c++11'])

fuse_dep = dependency('fuse')
sha_dep = dependency('libcrypto')

inc = include_directories('include')

subdir('include')
subdir('src')
subdir('test')
