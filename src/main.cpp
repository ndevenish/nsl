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

#include "nslobject.h"
#include "nslobjectfactory.h"

#include "cparser.h"

#include "edmexperiment.h"
#include "particle.h"
#include "container.h"
#include "electromagnetics.h"

#include <stdio.h>


using namespace std;

int main ( int argc, string argv )
{
	try {

		nslobject rootobject;

		// Add holders for the classes we want
		// None held at the moment!
		
		
		// the actual classes we want
		nslobjectfactory::add("particle", new particle::Factory);
		nslobjectfactory::add("edmexperiment", new edmexperiment::Factory);
		nslobjectfactory::add("container", new container::Factory);
		nslobjectfactory::add("volume_cylinder", new volume_cylinder::Factory);
		nslobjectfactory::add("bfield", new bfield::Factory);
		nslobjectfactory::add("forcefield", new forcefield::Factory);
		nslobjectfactory::add("linear_zgradient", new linear_zgradient::Factory);
		nslobjectfactory::add("efield", new efield::Factory);
		
		// Build the tree
		rootobject.parse("./scripts/testexperiment.nsl");

		cout << "Read in structure:" << endl;
		rootobject.tree();

		// hold off on regeneration for now
		/*cout << endl << "Regeneration:" << endl;*/
		//rootobject.regenerate(cout);

		rootobject.prepare();

		rootobject.run();
		




	} catch (runtime_error runt) {
		cout << "\aA runtime error has occured ";
		if (runt.what())
			cout << ": " << runt.what();
		cout << endl << "Terminating program." << endl;
	} catch (...) {
		cout << "\aAn unrecoverable, unidentified exception occured - terminating program." << endl;
	}

	// Hold at the end of execution until they hit enter
	getchar();

	return 0;
}