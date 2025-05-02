cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project (sudoku_test)
set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_MODULE_STD ON)

#catch2
find_package(Catch2 CONFIG REQUIRED)

#sources
file(GLOB SRCS "*.cpp")
add_executable (sudoku_test ${SRCS})

target_link_libraries(sudoku_test PRIVATE modules)
target_link_libraries(sudoku_test PRIVATE Catch2::Catch2WithMain)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(sudoku_test PUBLIC /std:c++latest /Z7 /permissive-)
else()
    target_compile_options(sudoku_test PUBLIC -std=c++20)
endif()