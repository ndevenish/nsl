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

#include "reporters.h"

//#include "physics.h"
#include "datasets.h"

class container;
class particle;
class bfield;
class efield;
class vector3;
class reporter;

const int MAX_PARTICLES = 32;
const int MAX_REPORTERS = 16;


/** Object to handle running of EDM Experiment */
class edmexperiment : public nslobject {

	friend class reporter;
	container *particlebox; ///< A shortcut link to the box the particle is stored in
	//particle *particles[MAX_PARTICLES]; ///< An array of particles we control
	//reporter *reporters[MAX_REPORTERS]; ///< An array of reporters to call
	
	std::vector<reporter*>	report_bounce, report_run, report_step;

	//void initvals( void );
	void readsettings ( void );
	
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
	/// A count of how many reporters we have
	//int reportercount;
	

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
	* This function spins the particle, and returns the phase change in the horizontal plane */
	long double spin_calculation( vector3& spinvector, const long double gyromag, const vector3& mag_field, const long double time);

	/** Calculates false EDM values.*/
	void edmcalcs( particle &part);
	
protected:

public:
	edmexperiment();

	dataset change;
	long double tmpld;

	/** Sets a variable that is being varied.
	* This function informs the edmexperiment class that a (single) parameter is being
	* varied. Multiple variations are not currently supported.
	* @param parameter The name of the parameter that is being varied
	* @param thisrunvalue The value of the parameter in this run
	* @param maxruns The number of runs to perform to vary over the full requested range. */
	void setvariation( std::string parameter, long double minval, long double maxvalm, int maxruns, nslobject *varyobject );

	/** Structure to hold information about a variation. */
	struct {
		std::string parameter; ///< What is the name of the parameter we are varying?
		long double minval;
		long double maxval;
		long double value; ///< What value is the variation taking this run?
		
		int runs; ///< How many runs will the variation take?
		bool varying; ///< Are we actually wanting a variation?
		
		nslobject *varyobject; ///< The object to vary
	} variation;
	
	/// Shortcut to a list of particles
	std::vector<particle*>	particles;
	
	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new edmexperiment; }
	};
};
#endif
