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
class vector3;

const int MAX_PARTICLES = 32;

/** Object to handle running of EDM Experiment */
class edmexperiment : public nslobject {

	container *particlebox; ///< A shortcut link to the box the particle is stored in
	particle *particles[MAX_PARTICLES]; ///< An array of particles we control
	bfield * magfield; ///< The magnetic field object
	efield *elecfield; ///< The electric field object
	
	/// the steptime to be used by the basic inbuilt midpoint solver
	long double steptime;
	/// How many phase averaging steps to take
	int phase_steps;
	/// How many bounces to take
	long bounces;
	
	/// The floating-point-error collision compensation distance
	long double collision_offset;

	/// A count of how many particles we are controlling
	int particlecount;

	bool prepareobject();
	bool runobject();	
	
	/** Performs a bigstep.
	* This function performs a large step i.e. a step between bounces.
	* It essentially acts as the solver function because it is passed a
	* particle and a time and alters the particle along that path. Based
	* on naming scheme from phil harris' original edm program. */
	void bigstep(particle& part, long double time);
	
	/** Performs a midpoint integrator step.
	* This function steps a particle forwards a very small amount and then calculates
	* the spin changes. */
	void smallstep(particle& part, long double time);
	
	/** Performs the necessary spin calculations.
	* This function spins the particle */
	void spin_calculation( vector3& spinvector, const vector3& mag_field, const long double time);

public:
	edmexperiment();

	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new edmexperiment; }
	};
};
#endif