cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project (sudoku)
set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_MODULE_STD ON)

#fmt
find_package(fmt CONFIG REQUIRED)

#sources
file(GLOB SRCS "*.cpp")
add_executable (sudoku ${SRCS})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(sudoku PUBLIC /std:c++latest /Z7 /permissive- /W4 /WX)
else()
    target_compile_options(sudoku PUBLIC -std=c++20)
endif()

target_link_libraries(sudoku PRIVATE modules)
target_link_libraries(sudoku PRIVATE fmt::fmt-header-only)