cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project (sudoku)

file(GLOB SRCS "*.cpp")
add_executable (sudoku ${SRCS})

target_include_directories(sudoku PUBLIC ../nanorange ../include)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(sudoku PUBLIC /std:c++latest /Z7 /permissive-)
else()
    target_compile_options(sudoku PUBLIC -std=c++20)
endif()