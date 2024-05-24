
cmake_minimum_required(VERSION 2.8)

# ----------------------------- Packaging ------------------------------

project(fastafs)

# helps debugging:
# Do this once in a while - find different compiler warnings
#set(CMAKE_CXX_COMPILER             "clang++")

set(PROJECT_VERSION "1.11.0")
set(PACKAGE_URL "https://github.com/yhoogstrate/fastafs")
set(PACKAGE_BUGREPORT "${PACKAGE_URL}/issues")

set(BUILD_DIR "bin")
set(BUILD_TEST_DIR "${BUILD_DIR}/test")
file(MAKE_DIRECTORY "${BUILD_DIR}")
file(MAKE_DIRECTORY "${BUILD_TEST_DIR}")

# include the git commit SHA1
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules/")
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
if(NOT "${GIT_SHA1}" STREQUAL "")
	set(GIT_SHA1_STRING "-${GIT_SHA1}")
endif()

# ----------------------------------------------------------------------
# ------------------------------ Pre-comp ------------------------------

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE debug)
	message(WARNING "No CMAKE_BUILD_TYPE was defined. Build type 'debug' was chosen, which will perform slower than using 'cmake -DCMAKE_BUILD_TYPE=release'.")
endif(NOT CMAKE_BUILD_TYPE)

string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE STREQUAL "debug")
	set(DEBUG "true")
else()
	set(DEBUG "false")
endif()

configure_file("include/config.hpp.in" "include/config.hpp")# implies building is done from project root
#configure_file("include/config.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/config.hpp")
#configure_file("include/config.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/config.hpp")
configure_file("include/config.hpp.in" "${BUILD_DIR}/include/config.hpp")


# ----------------------------------------------------------------------
# ------------------------------ Styling -------------------------------
# run 'make tidy' to style the syntax using astyle

find_program(ASTYLE astyle)

add_custom_command(
				OUTPUT  make_tidy
				COMMAND bash tidy.sh
			)

add_custom_target(tidy DEPENDS make_tidy )

# ----------------------------------------------------------------------
# --------------------------- Documentation ----------------------------

find_program(DOXYGEN doxygen)
find_program(DOT dot) # sudo apt install graphviz

configure_file("doc/Doxyfile.in" "doc/Doxyfile")

add_custom_command(
				OUTPUT  make_doc
				COMMAND doxygen doc/Doxyfile
			)

add_custom_target(doc DEPENDS make_doc)

# ----------------------------------------------------------------------
# ---------------------------- Compilation -----------------------------


add_subdirectory(src)
include_directories("${BUILD_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/include")


# ZSTD
include_directories("${CMAKE_SOURCE_DIR}/dependencies/zstd-lib-common")

# ZSTD-SEEKABLE
add_subdirectory("${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted")
include_directories("${CMAKE_SOURCE_DIR}/dependencies/zstd-seekable-adapted")

add_definitions(-std=c++14)

# Boost
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
if(NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found or requirement not satisfied. See building instructions.")
else()
  message(STATUS "Found Boost: ${Boost_INCLUDE_DIR}, ${Boost_LIBRARIES}")
  add_definitions(-DBOOST_ALL_DYN_LINK)
  include_directories(${Boost_INCLUDE_DIRS})
endif()


link_libraries(ssl)
link_libraries(crypto)
link_libraries(fuse)
link_libraries(z)# zlib; -lz; for crc32 checks on whole file integrity
link_libraries(zstd)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# -DXXH_NAMESPACE=ZST_
if(DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pg -ggdb -Wconversion -D_FILE_OFFSET_BITS=64 -g -DBOOST_TEST_TOOLS_UNDER_DEBUGGER -DBOOST_TEST_TOOLS_DEBUGGABLE")# -Werror makes compilation crash when warnings are given (also part of Travis)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -D_FILE_OFFSET_BITS=64")
endif()


# ----------------------------------------------------------------------
# ----------------------------- Build lib ------------------------------


add_library(libfastafs SHARED
					src/fasta_to_fastafs.cpp
					src/ucsc2bit_to_fastafs.cpp
					src/flags.cpp
					src/fastafs.cpp
					src/ucsc2bit.cpp
					
                    src/byte_decoder.cpp
                    
					src/xbit_byte_encoder.cpp
					src/twobit_byte.cpp
					src/fourbit_byte.cpp
					src/fivebit_fivebytes.cpp
					
					src/database.cpp
					src/utils.cpp
					src/sequence_region.cpp
					src/fuse.cpp
					src/lsfastafs.cpp
					src/chunked_reader.cpp


					dependencies/zstd-lib-common/xxhash.c
					dependencies/zstd-seekable-adapted/zstdseek_utils.cpp
					dependencies/zstd-seekable-adapted/zstdseek_compress.cpp
					dependencies/zstd-seekable-adapted/zstdseek_decompress.cpp
)
target_include_directories(libfastafs PUBLIC include)
target_include_directories(libfastafs PUBLIC dependencies/zstd-lib-common)
target_include_directories(libfastafs PUBLIC dependencies/zstd-seekable-adapted)


set_target_properties(libfastafs PROPERTIES LIBRARY_OUTPUT_DIRECTORY "lib")
set_target_properties(libfastafs PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(libfastafs PROPERTIES SOVERSION 1)
set_target_properties(libfastafs PROPERTIES OUTPUT_NAME fastafs)


##set_target_properties(libfastafs PROPERTIES HEADER_OUTPUT_DIRECTORY "include")
## great, this doesn't go automagically with an entire dir
set_target_properties(libfastafs PROPERTIES PUBLIC_HEADER "include/config.hpp;include/chunked_reader.hpp;include/database.hpp;include/fastafs.hpp;include/fasta_to_fastafs.hpp;include/flags.hpp;include/fourbit_byte.hpp;include/fuse.hpp;include/lsfastafs.hpp;include/sequence_region.hpp;include/twobit_byte.hpp;include/ucsc2bit.hpp;include/ucsc2bit_to_fastafs.hpp;include/utils.hpp")
set_target_properties(libfastafs PROPERTIES PUBLIC_HEADER_DIRECTORY include)
##set_target_properties(libfastafs PROPERTIES PUBLIC_HEADER_OUTPUT_DIRECTORY "include")

# ----------------------------------------------------------------------
# ----------------------------- Build exe ------------------------------


add_executable(fastafs src/main.cpp)
set_target_properties(fastafs PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}")
target_link_libraries(fastafs libfastafs)

# mount-only binary, without all the other stuff 'mount.fastafs' [for fstab]
add_executable(mount.fastafs src/main_mount.cpp)
set_target_properties(mount.fastafs PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}")
target_link_libraries(mount.fastafs libfastafs)



# ----------------------------------------------------------------------
# ------------------------------ Testing -------------------------------

#add_subdirectory(test)

set(BUILD_TEST_DIR "${BUILD_DIR}/test")
enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})					# 'make check' as alias for 'make test'


add_executable(test_cache test/cache/test_cache.cpp)
set_target_properties(test_cache PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_cache libfastafs)
add_test(test_cache ${BUILD_TEST_DIR}/test_cache)

add_executable(test_check test/check/test_check.cpp)
set_target_properties(test_check PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_check libfastafs)
add_test(test_check ${BUILD_TEST_DIR}/test_check)

add_executable(test_chunked_reader test/chunked_reader/test_chunked_reader.cpp )
add_test(test_chunked_reader ${BUILD_TEST_DIR}/test_chunked_reader)
set_target_properties(test_chunked_reader PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_chunked_reader libfastafs)

add_executable(test_database test/database/test_database.cpp)
set_target_properties(test_database PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_database libfastafs)
add_test(test_database ${BUILD_TEST_DIR}/test_database)

add_executable(test_fastafs_as_ucsc2bit test/fastafs/test_ucsc2bit.cpp)
add_test(test_fastafs_as_ucsc2bit ${BUILD_TEST_DIR}/test_fastafs_as_ucsc2bit)
set_target_properties(test_fastafs_as_ucsc2bit PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_fastafs_as_ucsc2bit libfastafs)

add_executable(test_fastafs test/fastafs/test_fastafs.cpp)
add_test(test_fastafs ${BUILD_TEST_DIR}/test_fastafs)
set_target_properties(test_fastafs PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_fastafs libfastafs)

add_executable(test_byte_decoder test/byte_decoder/test_byte_decoder.cpp)
add_test(test_byte_decoder ${BUILD_TEST_DIR}/test_byte_decoder)
set_target_properties(test_byte_decoder PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_byte_decoder libfastafs)

add_executable(test_fivebit_byte test/fivebit_fivebytes/test_fivebit_fivebytes.cpp)
add_test(test_fivebit_byte ${BUILD_TEST_DIR}/test_fivebit_byte)
set_target_properties(test_fivebit_byte PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_fivebit_byte libfastafs)

add_executable(test_flags test/flags/test_flags.cpp)
add_test(test_flags ${BUILD_TEST_DIR}/test_flags)
set_target_properties(test_flags PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_flags libfastafs)

add_executable(test_fourbit_byte test/fourbit_byte/test_fourbit_byte.cpp)
add_test(test_fourbit_byte ${BUILD_TEST_DIR}/test_fourbit_byte)
set_target_properties(test_fourbit_byte PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_fourbit_byte libfastafs)

add_executable(test_sequenceregion test/sequenceregion/test_sequenceregion.cpp)
add_test(test_sequenceregion ${BUILD_TEST_DIR}/test_sequenceregion)
set_target_properties(test_sequenceregion PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_sequenceregion libfastafs)

add_executable(test_twobit_byte test/twobit_byte/test_twobit_byte.cpp)
add_test(test_twobit_byte ${BUILD_TEST_DIR}/test_twobit_byte)
set_target_properties(test_twobit_byte PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_twobit_byte libfastafs)

add_executable(test_ucsc2bit_as_fasta test/ucsc2bit/test_ucsc2bit_as_fasta.cpp)
add_test(test_ucsc2bit_as_fasta ${BUILD_TEST_DIR}/test_ucsc2bit_as_fasta)
set_target_properties(test_ucsc2bit_as_fasta PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_ucsc2bit_as_fasta libfastafs)

add_executable(test_ucsc2bit_to_fastafs test/ucsc2bit_to_fastafs/test_ucsc2bit_to_fastafs.cpp)
add_test(test_ucsc2bit_to_fastafs ${BUILD_TEST_DIR}/test_ucsc2bit_to_fastafs)
set_target_properties(test_ucsc2bit_to_fastafs PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_ucsc2bit_to_fastafs libfastafs)

add_executable(test_utils test/utils/test_utils.cpp)
add_test(test_utils ${BUILD_TEST_DIR}/test_utils)
set_target_properties(test_utils PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_utils libfastafs)

add_executable(test_view test/view/test_view.cpp)
add_test(test_view ${BUILD_TEST_DIR}/test_view)
set_target_properties(test_view PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_TEST_DIR})
target_link_libraries(test_view libfastafs)


#add_test(test_functional          "test/test_functional.py")


find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)					# 'ctest -T memcheck'
INCLUDE(Dart)

# ----------------------------------------------------------------------
# --------------------------- Installation -----------------------------

# The compiled binary, usually to: /usr/local/bin/fastafs
install(TARGETS fastafs DESTINATION "bin")
install(TARGETS mount.fastafs DESTINATION "bin")

# don't build during debug at least
install(TARGETS libfastafs LIBRARY DESTINATION "lib" PUBLIC_HEADER DESTINATION "include/libfastafs")

# ----------------------------------------------------------------------
