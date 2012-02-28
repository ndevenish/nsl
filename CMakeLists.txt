cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project (NSL)

add_definitions(-g -O2 -Wall)

add_subdirectory(src)

find_package(Doxygen)

# If we have Doxygen, then build the documentation
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ALL ${DOXYGEN_EXECUTABLE}
                    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile 
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation with Doxygen" VERBATIM
                    )
else (DOXYGEN_FOUND)
  MESSAGE(WARNING "Doxygen not found. Can not build docs.")
endif(DOXYGEN_FOUND)
