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

#ifndef PARTICLE_H
#define PARTICLE_H

#include <string>

#include "physics.h"

#include "nslobject.h"
#include "nslobjectfactory.h"

class container;
class bfield;
class efield;

/** Object representing a particle. */
class particle : public nslobject {
	friend class edmexperiment;
	
	// Physical Properties values
	vector3	position;
	vector3 velocity_vec;
	vector3 spinEplus;
	vector3 spinEminus;

	long double velocity;

	long double gamma;
	
	long double flytime;
	
	// Utility class pointers
	container *particlebox;
	bfield *mag_field;
	efield *elec_field;

	bool prepareobject();
	
	// Initialise the values in the class
	void initvals();

	//bool getvaluetoproperty(std::string valuename, long double &dest);

public:
	particle();

	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new particle; }
	};
};

#endif