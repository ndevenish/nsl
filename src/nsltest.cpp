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

// nsltest.cpp : Defines the entry point for the console application.
//
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h> // for rand()

#include <sstream>

#include "nslobject.h"
#include "nslobjectfactory.h"

#include "cparser.h"
#include "physics.h"
#include "container.h"

#include <stdio.h>
#include <math.h>

using namespace std;

int testparser();
void testnslobject ( void );
void testfactory(void);
void testnslparsing ( void );
void testvectors(void);

void testray(void);

class newobject : public nslobject {
public:
	newobject() { objecttype = "newobject"; return; }
	class Factory : public nslobjectfactory {
		nslobject *create() { return new newobject; }
	};
};

int main (int argc, string argv)
{
	//testparser();
	//return 0;

try {

	/*ifstream infile("d:\\testparse.nsl");
	cparser parser(&infile);
	anobject.parse(parser);
	*/
	testnslobject();

	testfactory();

	testvectors();

//	testnslparsing();

	testray();
	
	cout << endl << "All tests completed successfully." << endl;

} catch (runtime_error runt) {
	cout << endl << "\aA runtime error has occured ";
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

int testparser()
{
	ifstream in("./scripts/parsenum.txt");
	cparser parser(&in);

	if (in)
		cout << "File opened successfully" << endl;
	else
		cout << "Error reading file." << endl;

	cout << "Status: " << in.bad() << in.eof() << in.fail() << in.good() << endl;

	//string stest;
	//parser >> stest;
	//cout << endl << "argh";
	do {
		cout << parser.gettoken() << endl;
	} while (!parser.eof());

	//cout << EOF << endl;

	cout << endl << "Press enter to continue....";
	getchar();

	return 0;
}

void testnslobject ( void )
{
	cout << endl << "===== Testing nslobject class =====" << endl;

	nslobject anobject, *secondobject;
	string a;
	secondobject = new nslobject;

	// Test we can set and get properties
	cout << "Testing object property setting and retrieval: ";
	anobject.set("something", "Success");
	a = anobject.get("something");
	if (a == "Success")
		cout << a << endl;
	else
		throw runtime_error("Faliure manipulating object properties");

	// Set up the names
	anobject.set("name", "baseobject");
	secondobject->set("name", "secondobject");


	cout << "Testing adding subobjects:" << endl;
	anobject.add(*secondobject);
	anobject.tree();

}

void testfactory ( void )
{
	cout << endl << "===== Testing class factory =====" << endl;
		int otypecount;
	// Test adding some objects to the factory
	otypecount = nslobjectfactory::count();
	cout << "Testing adding object to class factory: ";
	nslobjectfactory::add("newobject", new newobject::Factory);

	if (nslobjectfactory::count() != otypecount + 1)
		throw runtime_error("Failed to add test object to object factory");
	else
		cout << "Success." << endl;

	nslobjectfactory::add("container", new newobject::Factory);
	nslobjectfactory::add("particle", new newobject::Factory);
	nslobjectfactory::add("volume_cylinder", new newobject::Factory);
	nslobjectfactory::add("bfield", new newobject::Factory);

	// Testing adding a duplicate
	cout << "Testing that adding duplicate objects doesn't: ";
	otypecount = nslobjectfactory::count();
	nslobjectfactory::add("newobject", new newobject::Factory);
	if (nslobjectfactory::count() != otypecount)
		throw runtime_error("Adding duplicate object somehow succeeded");
	else
		cout << "Success." << endl;

	// Print a list of particle types
	nslobjectfactory::list();

	// Test removing of a specially added object
	cout << "Testing removing added object: ";
	nslobjectfactory::add("testobject", new newobject::Factory);
	otypecount = nslobjectfactory::count();	
	nslobjectfactory::remove("testobject");
	if (nslobjectfactory::count() == otypecount - 1)
		cout << "Success." << endl;
	else
		throw runtime_error("Test object not successfully removed.");

	// Now test removing an item that never existed
	cout << "Testing removing unadded object: ";
	nslobjectfactory::remove("unadded object");
	if (nslobjectfactory::count() != otypecount - 1)
		throw runtime_error("Removing an unadded object from the factory caused a list change");
	else
		cout << "Success." << endl;

}

void testnslparsing ( void )
{
	cout << endl << "===== Testing parsing data file =====" << endl;

	// Set up the object
	nslobject anobject;
	anobject.set("name", "Root Object");

	anobject.parse("./scripts/testparse.nsl");

	cout << "Testing finding an object by name: ";
	// find an object and rename it
	nslobject *retval = anobject.findbyname("subsub1");
	if (retval) {
		retval->set("name", "subsub1 renamed");
		cout << "Success." << endl;
	} else
		throw runtime_error("Cannot find object to rename");

	// Search for a "newobject" from this found object
	cout << "Testing finding by type from this object: ";
	retval = retval->findbytype("newobject");
	if (retval) {
		retval->set("name", "Found by type from subsub1");
		cout << "Success." << endl;
	} else
		throw runtime_error("Cannot find object by type");

	cout << "Parsed Tree: " << endl;
	anobject.tree();

	cout << endl << "Testing regeneration: " << endl;
	anobject.regenerate(cout);
}

void testvectors ( void )
{
	cout << endl << "===== Testing vectors =====" << endl;

	vector3 a(1.0, 1.0, 1.0), b(1.0, 1.0, 1.0), c;

	cout << "Streaming Vector: " << a << endl;
	
	cout << "Adding: \t";
	if (a+b == vector3(2.0, 2.0, 2.0))
		cout << "Success." << endl;
	else
		throw runtime_error("Adding vectors produced incorrect answer");

	cout << "Subtracting: \t";
	if (a-b == vector3(0, 0, 0))
		cout << "Success." << endl;
	else
		throw runtime_error("Subtracting vectors produced incorrect answer");

	cout << "Scaling: \t";
	if ( vector3(2, 2, 2)*3.5 == vector3(7, 7, 7))
		cout << "Success." << endl;
	else
		throw runtime_error("Scaling Vector produced incorrect answer");

	cout << "Scaling to:\t";
	a.scaleto(1.0);
	if (a.mod() == 1.0)
		cout << "Success.";
	else
		throw runtime_error("Scaling to produced incorrect results");
	//cout << a << " |a| = " << a.mod() << endl;

	/*cout << "Assignment: ";
	long double out = a.mod();
	if ((out - 1.0)*(out - 1.0) < 1e-10)
		cout << "Success." << endl;
	else
		throw runtime_error("Assignment to long double went wrong");
*/
//	c.print();
	cout << endl;

}

void testray(void)
{
	cout << endl << "======= Testing ray intersection tests =======" << endl;

	container box;
	volume_cylinder cylind, cylin2, cylin3;
	// Create a cylinder object and attach it to the box
	cylind.set("radius", "1"); cylind.set("height", "2"); cylind.set("type", "negative");
	cylind.set("name", "Cylinder"); cylind.set("z", "-1"); box.set("name", "Box");

	cylin2.set("radius", "1"); cylin2.set("height", "2"); cylin2.set("type", "positive");
	cylin2.set("name", "cylin2er"); cylin2.set("z", "-1.7"); cylin2.set("x", "1.5");

	cylin3.set("radius", "0.5"); cylin3.set("height", "0.5"); cylin3.set("type", "negative");
	cylin3.set("name", "Cylinder3"); cylin3.set("z", "-1.5");


	box.add(cylind);
	box.add(cylin2);
	//box.add(cylin3);

	box.prepare();
	
	// Print out the tree
	box.tree();

	ofstream out("./rayout.txt");

	// Make a couple of vectors for the ray and direction
	vector3 raypos(-0.1,0.0,0);
	vector3 rayvec(1,0.6,1);
	const long double velocity = 1000.0;

	rayvec.scaleto(velocity);
	//rayvec.scaleto(1000.0);

	//raypos = cylind.getposition("center");

	out << raypos << endl;

	intercept icept = box.cast(raypos, rayvec);

	cout << "First collides at: " << icept.time << "; " << raypos + rayvec*icept.time;

	for (long i = 0; i < 10000; i++)//490
	{
		if (i == 5)
			cout << "it" << endl;

		// Build a new ray directions
		raypos += rayvec * icept.time;

//		raypos += rayvec * (-adjust);
		out << raypos << endl;

		//Rout = Rin - 2N(N.Rin)
		icept.normal.scaleto(1.0);

		//long double t2 = (icept.normal * rayvec);
		//vector3 tmp = ;

		// Calculate the reflection
		rayvec = rayvec - ((icept.normal * 2.)*(icept.normal * rayvec));
		rayvec.x = rand()-(RAND_MAX/2);
		rayvec.y = rand()-(RAND_MAX/2);
		rayvec.z = rand()-(RAND_MAX/2);
		rayvec.scaleto(velocity);

		if (dot(rayvec, icept.normal) < 0)
			rayvec *= -1.0;

		
		//rayvec.scaleto(1.0);

		icept = box.cast(raypos, rayvec);
	}
	raypos += rayvec * icept.time;
	out << raypos << endl;
	cout << "End rayvec size" << rayvec.mod() << endl;



}
