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

#ifndef EDMEXPERIMENT_H
#define EDMEXPERIMENT_H

#include "nslobject.h"
#include "nslobjectfactory.h"

class container;
class particle;
class bfield;
class efield;

const int MAX_PARTICLES = 32;

/** Object to handle running of EDM Experiment */
class edmexperiment : public nslobject {

	container *particlebox;
	particle *particles[MAX_PARTICLES];
	bfield * magfield;
	efield *elecfield;

	int particlecount;

	bool prepareobject();
	bool runobject();	
	
	void bigstep(particle* part, long double time);
	void smallstep(particle* part, long double time);

public:
	edmexperiment();

	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new edmexperiment; }
	};
};
#endif