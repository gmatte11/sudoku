cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project (sudoku_test)
find_package(Catch2 CONFIG REQUIRED)

file(GLOB SRCS "*.cpp")
add_executable (sudoku_test ${SRCS})

target_include_directories(sudoku_test PUBLIC ../nanorange ../include)
target_link_libraries(sudoku_test PRIVATE Catch2::Catch2)

target_compile_options(sudoku_test PUBLIC /std:c++latest /Z7 /permissive-)