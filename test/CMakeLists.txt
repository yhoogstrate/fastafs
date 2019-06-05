cmake_minimum_required(VERSION 2.8)

include_directories(../include)

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


add_executable(test_cache               cache/test_cache.cpp ../src/fasta_to_fastafs.cpp ../src/fastafs.cpp ../src/twobit_byte.cpp ../src/utils.cpp)
add_executable(test_view                view/test_view.cpp ../src/fasta_to_fastafs.cpp ../src/fastafs.cpp ../src/twobit_byte.cpp ../src/utils.cpp)
#add_executable(test_tree                tree/test_tree.cpp)
add_executable(test_fastafs             fastafs/test_fastafs.cpp ../src/fasta_to_fastafs.cpp ../src/fastafs.cpp ../src/twobit_byte.cpp ../src/utils.cpp)
add_executable(test_ucsc2bit            fastafs/test_ucsc2bit.cpp ../src/fasta_to_fastafs.cpp ../src/fastafs.cpp ../src/twobit_byte.cpp ../src/utils.cpp)
add_executable(test_twobit_byte         twobit_byte/test_twobit_byte.cpp ../src/twobit_byte.cpp  ../src/utils.cpp)
add_executable(test_ucsc2bit_to_fastafs ucsc2bit_to_fastafs/test_ucsc2bit_to_fastafs.cpp ../src/fasta_to_fastafs.cpp ../src/ucsc2bit_to_fastafs.cpp ../src/fastafs.cpp ../src/twobit_byte.cpp ../src/utils.cpp)
add_executable(test_utils               utils/test_utils.cpp ../src/utils.cpp)

add_test(test_ucsc2bit_to_fasta  "${BUILD_TEST_DIR}/test_ucsc2bit_to_fasta")


set_target_properties(test_cache
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_view
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
#set_target_properties(test_tree
#                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_fastafs
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_ucsc2bit
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_twobit_byte
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_ucsc2bit_to_fastafs
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
set_target_properties(test_utils
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")
