cmake_minimum_required (VERSION 3.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 23)

add_library(modules)
file(GLOB SRCS "*.ixx")
target_sources(modules PUBLIC FILE_SET CXX_MODULES FILES ${SRCS})