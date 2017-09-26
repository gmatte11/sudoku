cmake_minimum_required (VERSION 2.6)
project (sudoku)

set (SRCS src/main.cpp src/solver.cpp)

include_directories(include range-v3/include)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -fconcepts -Irange-v3/include")

add_executable (sudoku ${SRCS})