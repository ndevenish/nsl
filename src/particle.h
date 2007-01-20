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

#include "datasets.h"

class container;
class bfield;
class efield;

/** Object representing a particle. */
class particle : public nslobject {
	friend class edmexperiment;
	friend class reporter;
	friend class impactreporter;
	friend class phasereporter;
	friend class edmreporter;
	friend class intervalreporter;
	friend class polreporter;
	friend class poldistreporter;
	friend class posreporter;
	friend class neutron_physics;
	
	// Physical Properties values
	vector3	position;
	vector3 velocity_vec;
	vector3 spinEplus;
	vector3 spinEminus;

	// A 'cache' for the exB effect that only changes every bounce (in a linear electric field)
	vector3 vxEeffect;
	
	long double velocity;

	/// The steptime is particle-dependant for more complicated solvers
	long double steptime;
	
	// Gyromagnetic ratio of the particle
	long double gamma;
	
	// Particles velocity gamma
	long double vgamma;
	
	long double E_sum_phase, E_minus_sum_phase;
	
	long double flytime;
	
	// particles mass
	long double mass;
	
	// Our calculated edm parameters
	long double frequencydiff;
	long double fake_edm;
	
	dataset cumulativeedm;
	
	// Utility class pointers
	container *particlebox;
	bfield *mag_field;
	efield *elec_field;

	bool prepareobject();
	void readsettings(void);
	
	// Initialise the values in the class
	void initvals();

	//bool getvaluetoproperty(std::string valuename, long double &dest);

public:
	particle();
	
	// Update the vxe effect
	void updateExv ( efield &Efield );
	

	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new particle; }
	};
};

#endif
