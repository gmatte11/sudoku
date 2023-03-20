cmake_minimum_required (VERSION 3.6)
project(sudoku_master)

set(ENV{VSLANG} 1033)

add_subdirectory(solver)
add_subdirectory(test)
