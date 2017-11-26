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


add_definitions(-std=c++11)

set(BUILD_DIR      "../bin")
set(BUILD_TEST_DIR "${BUILD_DIR}/test")


add_executable(test_cache          cache/test_cache.cpp ../src/twobit_byte.cpp)

set_target_properties(test_cache
                                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BUILD_TEST_DIR}")