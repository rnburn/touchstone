cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project("touchstone")

find_package(Boost)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/..)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -DNDEBUG")

add_subdirectory(examples)
