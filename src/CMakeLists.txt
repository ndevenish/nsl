cmake_minimum_required (VERSION 2.6 FATAL_ERROR)


# Check for BOOST - 1.34 was current when this software was written.
# Set the flags that the FindBoost module recommends
#set(Boost_DEBUG ON)
set(Boost_USE_MULTITHREADED      ON)
find_package(Boost 1.34 COMPONENTS thread filesystem system program_options REQUIRED)
# message("Boost_LIBRARIES: " ${Boost_LIBRARIES})

include_directories ( ${Boost_INCLUDE_DIRS} )
link_directories ( ${Boost_LIBRARY_DIRS} )

# Specify our sources

set(NEUTRON_SRC container.cpp cparser.cpp edmexperiment.cpp electromagnetics.cpp errors.cpp main.cpp neutronphysics.cpp nslobject.cpp nslobjectfactory.cpp particle.cpp physics.cpp random.cpp reporters.cpp solver.cpp tools.cpp variable.cpp volume_cylinder.cpp)

add_executable ( neutron ${NEUTRON_SRC})

target_link_libraries ( neutron ${Boost_LIBRARIES})
