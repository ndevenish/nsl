cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project (NSL)

add_definitions(-g -O2 -Wall)

add_subdirectory(src)

find_package(Doxygen)
if (DOXYGEN_FOUND STREQUAL "NO")
    message(FATAL_ERROR "Doxygen not found. Please get a copy http://www.doxygen.org")
endif (DOXYGEN_FOUND STREQUAL "NO")
