## Process this file with automake to produce Makefile.in

bin_PROGRAMS = nslneutron
#AM_LDFLAGS=-lm
#AM_CFLAGS = -O3 -g3 -Wall
AM_CXXFLAGS = -O3 -g3 -Wall
CXXFLAGS = -O3 -g3 -Wall

nslneutron_SOURCES = electromagnetics.cpp electromagnetics.h particle.cpp errors.cpp particle.h container.cpp           errors.h               physics.cpp container.h             main.cpp                physics.h cparser.cpp              reporters.cpp cparser.h               nslobject.cpp           reporters.h datasets.h              nslobject.h             vector3.h edmexperiment.cpp       nslobjectfactory.cpp    volume_cylinder.cpp edmexperiment.h         nslobjectfactory.h tools.h tools.cpp random.h random.cpp neutronphysics.cpp neutronphysics.h solver.cpp solver.h variable.h variable.cpp

nslneutron_LDADD = -lm -lboost_program_options -lboost_thread
#nslneutron_CXXFLAGS = -O3 -g3 -Wall
