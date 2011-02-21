
LIBS = -lboost_program_options -lboost_filesystem -lboost_thread -lboost_system
CXXFLAGS = -g -O2

all: neutron

neutron: build build/container.o build/cparser.o build/edmexperiment.o build/electromagnetics.o build/errors.o build/main.o build/neutronphysics.o build/nslobject.o build/nslobjectfactory.o build/particle.o build/physics.o build/random.o build/reporters.o build/solver.o build/tools.o build/variable.o build/volume_cylinder.o
	g++ $(LIBS) $(CXXFLAGS) build/*.o -o neutron

build:
	mkdir -p build

build/%.o: src/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f build/*.o
	rm -f neutron

distclean: clean
	rmdir build