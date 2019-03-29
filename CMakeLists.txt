
cmake_minimum_required(VERSION 2.8)

# ----------------------------- Packaging ------------------------------

project(fastafs)

# Do this once in a while - find different bugs
#set(CMAKE_CXX_COMPILER             "clang++")

set(PROJECT_VERSION "0.0.6")
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

configure_file("include/config.hpp.in" "include/config.hpp")

# ----------------------------------------------------------------------
# ------------------------------ Styling -------------------------------
# run 'make tidy' to style the syntax using astyle

find_program(ASTYLE astyle)

add_custom_command(
				OUTPUT  make_tidy
				COMMAND astyle -n --style=linux include/*.hpp
				COMMAND astyle -n --style=linux src/*.cpp
				COMMAND astyle -n --style=linux test/*/*.cpp
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
include_directories(include)
add_definitions(-std=c++14)

# Boost
find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
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


if(DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -Wall -g -ggdb -Wconversion -D_FILE_OFFSET_BITS=64")# -Werror makes compilation crash when warnings are given (also part of Travis)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -D_FILE_OFFSET_BITS=64")
endif()

add_executable(fastafs
					src/main.cpp
					src/fasta_to_fastafs.cpp
					src/fastafs.cpp
					src/twobit_byte.cpp
					src/database.cpp
					src/utils.cpp
					src/fuse.cpp
				)

set_target_properties(fastafs PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}")

# ----------------------------------------------------------------------
# ------------------------------ Testing -------------------------------

add_subdirectory(test)

enable_testing()

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})					# 'make check' as alias for 'make test'

add_test(test_twobit_byte         "${BUILD_TEST_DIR}/test_twobit_byte")
add_test(test_cache               "${BUILD_TEST_DIR}/test_cache")
add_test(test_view                "${BUILD_TEST_DIR}/test_view")
add_test(test_tree                "${BUILD_TEST_DIR}/test_tree")
add_test(test_fastafs             "${BUILD_TEST_DIR}/test_fastafs")
add_test(test_ucsc2bit            "${BUILD_TEST_DIR}/test_ucsc2bit")
add_test(test_ucsc2bit_to_fastafs "${BUILD_TEST_DIR}/test_ucsc2bit_to_fastafs")


#find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)					# 'ctest -T memcheck'
#INCLUDE(Dart)

# ----------------------------------------------------------------------
# --------------------------- Installation -----------------------------

# The compiled binary, usually to: /usr/local/bin/fastafs
install(TARGETS fastafs DESTINATION "bin")

# ----------------------------------------------------------------------
