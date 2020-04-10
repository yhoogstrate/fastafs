
cmake_minimum_required(VERSION 2.8)

# ----------------------------- Packaging ------------------------------

project(fastafs)

# Do this once in a while - find different bugs
#set(CMAKE_CXX_COMPILER             "clang++")

set(PROJECT_VERSION "1.7.5")
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

#find_program(ASTYLE doxygen)

#configure_file("doc/Doxyfile.in" "doc/Doxyfile")

#add_custom_command(
				#OUTPUT  make_doc
				#COMMAND doxygen doc/Doxyfile
			#)

#add_custom_target(doc DEPENDS make_doc)

# ----------------------------------------------------------------------
# ---------------------------- Compilation -----------------------------


add_subdirectory(src)
#include_directories(${BUILD_DIR})
include_directories("${BUILD_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/include")

# ZSTD
include_directories("${CMAKE_SOURCE_DIR}/dependencies/zstd/lib/common")
include_directories("${CMAKE_SOURCE_DIR}/dependencies/zstd/lib")
link_libraries("${CMAKE_SOURCE_DIR}/dependencies/zstd/lib/libzstd.a")

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


if(DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -g -ggdb -Wconversion -D_FILE_OFFSET_BITS=64")# -Werror makes compilation crash when warnings are given (also part of Travis)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -D_FILE_OFFSET_BITS=64")
endif()

add_executable(fastafs
					src/main.cpp
					src/fasta_to_fastafs.cpp
					src/ucsc2bit_to_fastafs.cpp
					src/flags.cpp
					src/fastafs.cpp
					src/ucsc2bit.cpp
					src/twobit_byte.cpp
					src/fourbit_byte.cpp
					src/database.cpp
					src/utils.cpp
					src/sequence_region.cpp
					src/fuse.cpp
					src/lsfastafs.cpp
					
					dependencies/zstd-seekable-adapted/seekable_utils.cpp
					dependencies/zstd-seekable-adapted/seekable_compression.cpp
					dependencies/zstd-seekable-adapted/zstdseek_compress.cpp
				)
set_target_properties(fastafs PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}")

# mount-only binary, without all the other stuff 'mount.fastafs' [for fstab]
add_executable(mount.fastafs
					src/main_mount.cpp
					src/fasta_to_fastafs.cpp
					src/ucsc2bit_to_fastafs.cpp
					src/flags.cpp
					src/fastafs.cpp
					src/ucsc2bit.cpp
					src/twobit_byte.cpp
					src/fourbit_byte.cpp
					src/database.cpp
					src/utils.cpp
					src/sequence_region.cpp
					src/fuse.cpp
					src/lsfastafs.cpp
				)
set_target_properties(mount.fastafs PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}")

add_library(libfastafs SHARED
					src/fasta_to_fastafs.cpp
					src/ucsc2bit_to_fastafs.cpp
					src/flags.cpp
					src/fastafs.cpp
					src/ucsc2bit.cpp
					src/twobit_byte.cpp
					src/fourbit_byte.cpp
					src/database.cpp
					src/utils.cpp
					src/sequence_region.cpp
					src/fuse.cpp
					src/lsfastafs.cpp
)
target_include_directories(libfastafs PUBLIC include)
target_sources(libfastafs PUBLIC include/fastafs.hpp)

set_target_properties(libfastafs PROPERTIES LIBRARY_OUTPUT_DIRECTORY "lib")
set_target_properties(libfastafs PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(libfastafs PROPERTIES SOVERSION 1)
set_target_properties(libfastafs PROPERTIES OUTPUT_NAME fastafs)

##set_target_properties(libfastafs PROPERTIES HEADER_OUTPUT_DIRECTORY "include")
## great, this doesn't go automagically with an entire dir
set_target_properties(libfastafs PROPERTIES PUBLIC_HEADER "include/config.hpp;include/database.hpp;include/fastafs.hpp;include/fasta_to_fastafs.hpp;include/flags.hpp;include/fourbit_byte.hpp;include/fuse.hpp;include/lsfastafs.hpp;include/sequence_region.hpp;include/twobit_byte.hpp;include/ucsc2bit.hpp;include/ucsc2bit_to_fastafs.hpp;include/utils.hpp")
##set_target_properties(libfastafs PROPERTIES PUBLIC_HEADER_DIRECTORY include)
##set_target_properties(libfastafs PROPERTIES PUBLIC_HEADER_OUTPUT_DIRECTORY "include")

# ----------------------------------------------------------------------
# ------------------------------ Testing -------------------------------

add_subdirectory(test)

enable_testing()

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})					# 'make check' as alias for 'make test'

add_test(test_twobit_byte         "${BUILD_TEST_DIR}/test_twobit_byte") # ACTG(N) | ACUG(N)
add_test(test_fourbit_byte        "${BUILD_TEST_DIR}/test_fourbit_byte") # ACGTURYKMSWBDHVN(-)
add_test(test_cache               "${BUILD_TEST_DIR}/test_cache")
add_test(test_view                "${BUILD_TEST_DIR}/test_view")
add_test(test_flags               "${BUILD_TEST_DIR}/test_flags")
add_test(test_fastafs             "${BUILD_TEST_DIR}/test_fastafs")
add_test(test_check               "${BUILD_TEST_DIR}/test_check") # file integrity checks
add_test(test_fastafs_as_ucsc2bit "${BUILD_TEST_DIR}/test_fastafs_as_ucsc2bit")
add_test(test_ucsc2bit_to_fastafs "${BUILD_TEST_DIR}/test_ucsc2bit_to_fastafs")
add_test(test_ucsc2bit_as_fasta   "${BUILD_TEST_DIR}/test_ucsc2bit_as_fasta")
add_test(test_sequenceregion      "${BUILD_TEST_DIR}/test_sequenceregion")
add_test(test_utils               "${BUILD_TEST_DIR}/test_utils")
add_test(test_functional          "test/test_functional.py")
#add_test(test_tree                "${BUILD_TEST_DIR}/test_tree")

#find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)					# 'ctest -T memcheck'
#INCLUDE(Dart)

# ----------------------------------------------------------------------
# --------------------------- Installation -----------------------------

# The compiled binary, usually to: /usr/local/bin/fastafs
install(TARGETS fastafs DESTINATION "bin")
install(TARGETS mount.fastafs DESTINATION "bin")

# don't build during debug at least
install(TARGETS libfastafs LIBRARY DESTINATION "lib" PUBLIC_HEADER DESTINATION "include/libfastafs")

# ----------------------------------------------------------------------
