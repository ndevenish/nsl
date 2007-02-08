/* 
 * Copyright (c) 2006-2012 Nicholas Devenish
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 */

#include <string>
#include <iostream>
#include <fstream>

#include <sstream>

#include "random.h"
#include "nslobject.h"
#include "nslobjectfactory.h"

#include "cparser.h"

#include "edmexperiment.h"
#include "particle.h"
#include "container.h"
#include "electromagnetics.h"
#include "reporters.h"
#include "solver.h"

#include <stdio.h>

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>


using std::cout;
using std::endl;
using std::string;
using std::runtime_error;

using boost::filesystem::path;

namespace po = boost::program_options;

int main ( int argc, char *argv[] )
{	
	po::options_description desc("Allowed Options");
	po::positional_options_description p;
	p.add("input-file", -1);

	desc.add_options()
		("help,h", "Produces help message")
		("tree,t", "Prints an object tree before starting")
		("no-run,d", "Does a dry run - script file will be parsed and prepared, but not run.")
		("input-file,f", po::value<string>(), "The nsl script file to load and run");
	;
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run() , vm);
	po::notify(vm);

	//Print the options if the user demanded so!
	if (vm.count("help")) {
		cout << desc << endl;
		return 0;
	}

	
	try {

		nslobject rootobject;
		
		// the actual classes we want
		nslobjectfactory::add("particle", new particle::Factory);
		nslobjectfactory::add("edmexperiment", new edmexperiment::Factory);
		nslobjectfactory::add("container", new container::Factory);
		nslobjectfactory::add("volume_cylinder", new volume_cylinder::Factory);
		nslobjectfactory::add("bfield", new bfield::Factory);
		nslobjectfactory::add("forcefield", new forcefield::Factory);
		nslobjectfactory::add("linear_zgradient", new linear_zgradient::Factory);
		nslobjectfactory::add("efield", new efield::Factory);
		nslobjectfactory::add("dipole_zmagnetic", new dipole_zmagnetic::Factory);
		nslobjectfactory::add("midpointsolver", new midpointsolver::Factory);
		
		// Reporters!
		nslobjectfactory::add("reporter", new reporter::Factory);
		nslobjectfactory::add("impactreporter", new impactreporter::Factory);
		nslobjectfactory::add("phasereporter", new phasereporter::Factory);
		nslobjectfactory::add("edmreporter", new edmreporter::Factory);
		nslobjectfactory::add("intervalreporter", new intervalreporter::Factory);
		nslobjectfactory::add("polreporter", new polreporter::Factory);
		nslobjectfactory::add("poldistreporter", new poldistreporter::Factory);
		nslobjectfactory::add("posreporter", new posreporter::Factory);
		nslobjectfactory::add("alphareporter", new alphareporter::Factory);
		
		// Build the tree
		path infile;
		if (!vm.count("input-file"))
		{
			cout << "No input script specified, defaulting to testexperiment.nsl" << endl;
			//rootobject.parse("./scripts/testexperiment.nsl");
			infile = "./scripts/testexperiment.nsl";
		} else {
			cout << "Running script " << vm["input-file"].as<string>() << endl;
			//rootobject.parse(vm["input-file"].as<string>());
			infile = vm["input-file"].as<string>();
		}
		
		if (!exists(infile))
			throw runtime_error("Specified infile does not exist");
		
		rootobject.parse(infile.native_file_string());

		if (vm.count("tree")) {
			cout << "Read in structure:" << endl;
			rootobject.tree();
		}
//		return 0;

		// hold off on regeneration for now
		/*cout << endl << "Regeneration:" << endl;*/
		//rootobject.regenerate(cout);

		// initialise the random number generator
		nsl::seedrand();
		
		rootobject.prepare();

		// Allow the user to specify an option not to run
		if (!vm.count("no-run"))
		{
			cout << endl << "Starting Run:" << endl;
			rootobject.run();
		}
		

	} catch (runtime_error runt) {
		cout << "\aA runtime error has occured ";
		if (runt.what())
			cout << ": " << runt.what();
		cout << endl << "Terminating program." << endl;
	} catch (...) {
		cout << "\aAn unrecoverable, unidentified exception occured - terminating program." << endl;
	}

	// Hold at the end of execution until they hit enter
	#ifdef WIN32
		cout << "Completed Run. Press any key to continue." << endl; 
		getchar();
	#endif

	return 0;
}
