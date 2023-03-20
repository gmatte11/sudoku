cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project (sudoku)

#fmt
find_package(fmt CONFIG REQUIRED)

#sources
file(GLOB SRCS "*.cpp")
add_executable (sudoku ${SRCS})

target_include_directories(sudoku PUBLIC ../nanorange ../include)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(sudoku PUBLIC /std:c++latest /Z7 /permissive- /W4 /WX)
else()
    target_compile_options(sudoku PUBLIC -std=c++20)
endif()

target_link_libraries(sudoku PRIVATE fmt::fmt fmt::fmt-header-only)