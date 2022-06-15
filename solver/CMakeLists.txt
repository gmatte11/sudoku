cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project (sudoku)

file(GLOB SRCS "*.cpp")
add_executable (sudoku ${SRCS})

target_include_directories(sudoku PUBLIC ../nanorange ../include)

if (WIN32)
    target_compile_options(sudoku PUBLIC /std:c++latest /Z7 /permissive- /await)
else()
    target_compile_options(sudoku PUBLIC -std=c++17)
endif()