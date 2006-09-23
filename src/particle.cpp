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
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "nslobject.h"
#include "nslobjectfactory.h"

#include "particle.h"
#include "container.h"

#include "errors.h"

using namespace std;


particle::particle()
{
	particlebox = 0;

	gamma = 0.0;
	position.x = position.y = position.z = 0.0;
	velocity_vec.x = velocity_vec.y = velocity_vec.z = 0.0;
	velocity = 0.0;
	spin.x = spin.y = spin.z = 0.0;

	objecttype = "particle";
}


bool particle::prepareobject()
{
	// Find a container!
	particlebox = (container*)findbytype("container");
	if (!particlebox)
		throw runtime_error("Unable to find a container for particle");

	// ***************************************
	// Get and validate the velocities

	// First get the vector if we have one
	if (isset("vx") || isset("vy") || isset("vz"))
	{
		velocity_vec.x = getlongdouble("vx", 0.0);
		velocity_vec.y = getlongdouble("vy", 0.0);
		velocity_vec.z = getlongdouble("vz", 0.0);
	} else {
		velocity_vec.x = velocity_vec.y = velocity_vec.z = 1.0;
	}

	if (isset("velocity")) {
		velocity = getlongdouble("velocity", 0.0);
		velocity_vec.scaleto(velocity);
	} else {
		velocity = velocity_vec.mod();
	}

	// Warn for zero velocity
	if (velocity == 0.0)
		Warning("Velocity in particle is unset (or set to 0.0)");

	// **************************************
	// Get and validate the positions
	position.x = getlongdouble("x", 0.0);
	position.y = getlongdouble("y", 0.0);
	position.z = getlongdouble("z", 0.0);

	// See if we have set a startvolume
	string startvolume;
	if (isset("startvolume"))
	{
		startvolume = get("startvolume");

		string startposition;
		startposition = get("startposition", "center");
		/*if (isset("startposition"))
			startposition = get("startposition");
		else
			startposition = "center";
*/
		// Get the base position for this 
		nslobject *startvol;
		startvol = particlebox->findbyname(startvolume);
		if (startvol->isoftype("container"))
			position = ((container*)startvol)->getposition(startposition);
		else
			throw runtime_error("Attempting to start particle in non-container volume");
	}


	// *********************************************************
	// Extract all the physical properties from the property set

	// Gamma value
	gamma = getlongdouble("gamma", 0.0);
	if (gamma == 0.0)
		Warning("Gamma in particle is unset (or set to 0.0)");

	// ~~~~~ Spin stuff
	long double start_spin_polar_angle, start_spin_phase;
	// Start by getting the spin polar angles
	start_spin_polar_angle = pi*getlongdouble("start_spin_polar_angle", 0.5);
	start_spin_phase = pi*getlongdouble("start_spin_phase", 0.0);

	spin.x = sinl(start_spin_polar_angle)*cosl(start_spin_phase);
	spin.y = sinl(start_spin_polar_angle)*sinl(start_spin_phase);
	spin.z = cosl(start_spin_polar_angle);

	// ************************************************************
	// Find a magnetic field to link to
	mag_field = (bfield*)findbytype("bfield");

	// Look for an electric field to link to
	elec_field = (efield*)findbytype("efield");

	return true;
}