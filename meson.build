project('fastafs', 'cpp',
    default_options : ['cpp_std=c++11'])

config_hpp = configuration_data()

config_hpp.set_quoted('PACKAGE_NAME', 'fastafs')
config_hpp.set_quoted('PACKAGE_VERSION', meson.project_version())
config_hpp.set_quoted('PACKAGE_STRING', 'fastafs-' + meson.project_version())
config_hpp.set_quoted('PACKAGE_BUGREPORT', 'url')
config_hpp.set('PACKAGE', 'PACKAGE_NAME')
config_hpp.set('VERSION', 'PACKAGE_VERSION')



fuse_dep = dependency('fuse')
sha_dep = dependency('libcrypto')

inc = include_directories('include')

subdir('include')
subdir('src')
subdir('test')
