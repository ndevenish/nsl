cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

# Check for BOOST - 1.34 was current when this software was written.
set(Boost_USE_MULTITHREADED      ON)
find_package(Boost 1.34 COMPONENTS thread filesystem system program_options REQUIRED)

# Set up the inclusion of Boost
include_directories ( ${Boost_INCLUDE_DIRS} )
link_directories ( ${Boost_LIBRARY_DIRS} )

# Specify our sources
set(NEUTRON_SRC container.cpp cparser.cpp edmexperiment.cpp electromagnetics.cpp errors.cpp neutronphysics.cpp nslobject.cpp nslobjectfactory.cpp particle.cpp physics.cpp random.cpp reporters.cpp solver.cpp tools.cpp variable.cpp volume_cylinder.cpp)

# Set up the targets
add_executable ( neutron main.cpp )
add_executable ( nsltest nsltest.cpp )
add_library(nslneutron ${NEUTRON_SRC})

# Add the library interdependency
target_link_libraries ( neutron ${Boost_LIBRARIES} nslneutron )
target_link_libraries ( nsltest ${Boost_LIBRARIES} nslneutron )
