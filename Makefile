
LIBS = -lboost_program_options -lboost_filesystem -lboost_thread -lboost_system
CXXFLAGS = -g -O2

all: neutron nsltest

test: nsltest
	./nsltest

docs:
	doxygen Doxyfile

neutron: build build/mains build/container.o build/cparser.o build/edmexperiment.o build/electromagnetics.o build/errors.o build/mains/main.o build/neutronphysics.o build/nslobject.o build/nslobjectfactory.o build/particle.o build/physics.o build/random.o build/reporters.o build/solver.o build/tools.o build/variable.o build/volume_cylinder.o
	g++ $(LIBS) $(CXXFLAGS) build/mains/main.o build/*.o -o neutron

nsltest: build build/mains/nsltest.o build/container.o build/cparser.o build/edmexperiment.o build/electromagnetics.o build/errors.o build/neutronphysics.o build/nslobject.o build/nslobjectfactory.o build/particle.o build/physics.o build/random.o build/reporters.o build/solver.o build/tools.o build/variable.o build/volume_cylinder.o
	g++ $(LIBS) $(CXXFLAGS) build/mains/nsltest.o build/*.o -o nsltest

build:
	mkdir -p build

build/mains:
	mkdir -p build/mains

build/%.o: src/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

build/mains/%.o: src/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

clean:
	-rm -f build/*.o
	-rm -f build/mains/*.o
	-rm -f neutron
	-rm -f nsltest

cleandocs:
	-rm -rf doc/html

distclean: clean cleandocs
	-rmdir build/mains
	-rmdir build