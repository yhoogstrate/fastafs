cmake_minimum_required(VERSION 2.8)

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted")
include_directories("${CMAKE_SOURCE_DIR}/dependencies/zstd/lib/common")
include_directories("${CMAKE_SOURCE_DIR}/dependencies/zstd/lib")


find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
if(NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found or requirement not satisfied. See building instructions.")
else()
  message(STATUS "Found Boost: ${Boost_INCLUDE_DIR}, ${Boost_LIBRARIES}")
  add_definitions(-DBOOST_ALL_DYN_LINK)
  include_directories(${Boost_INCLUDE_DIRS})
endif()


add_definitions(-std=c++14)

set(BUILD_DIR      "../bin")
set(BUILD_TEST_DIR "${BUILD_DIR}/test")


add_executable(test_twobit_byte         "twobit_byte/test_twobit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/twobit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/chunked_reader.cpp"

                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-lib-common/xxhash.c"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_compress.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp")


add_executable(test_fourbit_byte        "fourbit_byte/test_fourbit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fourbit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp"

                                        "${CMAKE_SOURCE_DIR}/src/chunked_reader.cpp"

                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-lib-common/xxhash.c"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_compress.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp")


add_executable(test_fivebit_fivebytes   "fivebit_fivebytes/test_fivebit_fivebytes.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fivebit_fivebytes.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp"

                                        "${CMAKE_SOURCE_DIR}/src/chunked_reader.cpp"

                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-lib-common/xxhash.c"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_compress.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp")


add_executable(test_cache               "cache/test_cache.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fasta_to_fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/flags.cpp"

                                        "${CMAKE_SOURCE_DIR}/src/fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/twobit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fourbit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fivebit_fivebytes.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/sequence_region.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/chunked_reader.cpp"

                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-lib-common/xxhash.c"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_compress.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp")


add_executable(test_view                "view/test_view.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fasta_to_fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/flags.cpp"

                                        "${CMAKE_SOURCE_DIR}/src/fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/twobit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fourbit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fivebit_fivebytes.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/sequence_region.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/chunked_reader.cpp"

                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-lib-common/xxhash.c"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_compress.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp")


add_executable(test_flags               "flags/test_flags.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/flags.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp")


add_executable(test_fastafs             "fastafs/test_fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fasta_to_fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/flags.cpp"

                                        "${CMAKE_SOURCE_DIR}/src/fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/twobit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fourbit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fivebit_fivebytes.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/sequence_region.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/chunked_reader.cpp"

                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-lib-common/xxhash.c"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_compress.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp")


add_executable(test_check               "check/test_check.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fasta_to_fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/flags.cpp"

                                        "${CMAKE_SOURCE_DIR}/src/fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/twobit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fourbit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fivebit_fivebytes.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/sequence_region.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/chunked_reader.cpp"

                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-lib-common/xxhash.c"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_compress.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp")


add_executable(test_fastafs_as_ucsc2bit "fastafs/test_ucsc2bit.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fasta_to_fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/flags.cpp"

                                        "${CMAKE_SOURCE_DIR}/src/fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/twobit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fourbit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fivebit_fivebytes.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/sequence_region.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/chunked_reader.cpp"

                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-lib-common/xxhash.c"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_compress.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp")


add_executable(test_ucsc2bit_to_fastafs "ucsc2bit_to_fastafs/test_ucsc2bit_to_fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fasta_to_fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/ucsc2bit_to_fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/flags.cpp"

                                        "${CMAKE_SOURCE_DIR}/src/fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/twobit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fourbit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fivebit_fivebytes.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/sequence_region.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/chunked_reader.cpp"

                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-lib-common/xxhash.c"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_compress.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp")


add_executable(test_ucsc2bit_as_fasta   "ucsc2bit/test_ucsc2bit_as_fasta.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fasta_to_fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/flags.cpp"

                                        "${CMAKE_SOURCE_DIR}/src/fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/ucsc2bit.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/twobit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fourbit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fivebit_fivebytes.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/sequence_region.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/chunked_reader.cpp"

                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-lib-common/xxhash.c"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_compress.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp")


add_executable(test_sequenceregion      "sequenceregion/test_sequenceregion.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/sequence_region.cpp")


add_executable(test_utils               "utils/test_utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp")


add_executable(test_chunked_reader      "chunked_reader/test_chunked_reader.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fasta_to_fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/flags.cpp"

                                        "${CMAKE_SOURCE_DIR}/src/fastafs.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/twobit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fourbit_byte.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/fivebit_fivebytes.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/src/sequence_region.cpp"

                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-lib-common/xxhash.c"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_utils.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_compress.cpp"
                                        "${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp"
                                        
                                        "${CMAKE_SOURCE_DIR}/src/chunked_reader.cpp"                                        )
#add_executable(test_tree                tree/test_tree.cpp)




set_target_properties(test_cache
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_view
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_flags
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_fastafs
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_check
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_fastafs_as_ucsc2bit
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_twobit_byte
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_fourbit_byte
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_fivebit_fivebytes
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_ucsc2bit_to_fastafs
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_ucsc2bit_as_fasta
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_sequenceregion
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_utils
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_chunked_reader
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
#set_target_properties(test_tree
#                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")

