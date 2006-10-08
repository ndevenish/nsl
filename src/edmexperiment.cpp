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


#include "errors.h"

#include "nslobject.h"
#include "nslobjectfactory.h"

#include "container.h"
#include "electromagnetics.h"
#include "particle.h"

#include "edmexperiment.h"

using std::runtime_error;

/*container *particlebox;
	particle *particles[MAX_PARTICLES];
	bfield * magfield;
	efield *elecfield;*/
edmexperiment::edmexperiment()
{
	particlecount = 0;
	particlebox = 0;
	magfield = 0;
	elecfield = 0;
	for (int i = 0; i < MAX_PARTICLES; i++)
		particles[i] = 0;

	objecttype = "edmexperiment";
	types.push_back(objecttype);
}

bool edmexperiment::prepareobject()
{
	//Iterator
	std::vector<nslobject*>::iterator sublist;

	// Check we only have the right number of containers
	if (countchildren("container") != 1)
		throw runtime_error("Container number other than one specified");
	//particle = findbytype("container", 0);
	// Find and add the container object
	for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
		if ((*sublist)->isoftype("container"))
			particlebox = (container*)(*sublist);

	// And that we have at least one particle
	if (countchildren("particle") < 1)
		throw runtime_error("No particles specified under edmexperiment object");
	// Add the particles to the list
	for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
		if ((*sublist)->isoftype("particle"))
			particles[particlecount++] = (particle*)(*sublist);
	
	// EM field objects
	if (countchildren("efield") != 1)
		throw runtime_error("efield number other than one specified");
	if (countchildren("bfield") != 1)
		throw runtime_error("bfield number other than one specified");
	// Find and add the bfield object
	for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
		if ((*sublist)->isoftype("bfield"))
			magfield = (bfield*)(*sublist);
	// Find and add the efield object
	for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
		if ((*sublist)->isoftype("efield"))
			elecfield = (efield*)(*sublist);

	// If at this point we have the particlebox, particles, bfield and efield objects
	// configured

	return true;
}

bool edmexperiment::runobject()
{
	return false;
}

void edmexperiment::bigstep(particle* part, long double time)
{

}

void edmexperiment::smallstep(particle* part, long double time)
{

}
