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
class edmreporter;

const int MAX_PARTICLES = 32;
const int MAX_REPORTERS = 16;


/** Object to handle running of EDM Experiment */
class edmexperiment : public nslobject {

	friend class reporter;
	friend class edmreporter;
	friend class midpointsolver;
	friend class alphareporter;
	
	container *particlebox; ///< A shortcut link to the box the particle is stored in
	//particle *particles[MAX_PARTICLES]; ///< An array of particles we control
	//reporter *reporters[MAX_REPORTERS]; ///< An array of reporters to call
	
	std::vector<reporter*>	report_bounce, report_run, report_step, report_interval;

	//void initvals( void );
	void readsettings ( void );
	
	bfield * magfield; ///< The magnetic field object
	efield *elecfield; ///< The electric field object
	
	class solver* thesolver; ///< Solver object link
	
	/// the steptime to be used by the basic inbuilt midpoint solver
	long double steptime;
	/// How many phase averaging steps to take
	int phase_steps;
	/// How many bounces to take
	unsigned long bounces;
	/// The amount of time for a particle to exist!
	long double lifetime;
	long double intervaltime;
	bool uselifetime; // Using lifetime?
	
	/// The floating-point-error collision compensation distance
	long double collision_offset;

	/// A count of how many particles we are controlling
	int particlecount;
	/// A count of how many reporters we have
	//int reportercount;
	
	//Is gravity turned on?
	bool gravity;

	bool prepareobject();
	bool runobject();	
	
	/** Performs a bigstep.
	* This function performs a large step i.e. a step between bounces.
	* It essentially acts as the solver function because it is passed a
	* particle and a time and alters the particle along that path. Based
	* on naming scheme from phil harris' original edm program. */
//	void bigstep(particle& part, long double time);
	
	/** Performs a midpoint integrator step.
	* This function steps a particle forwards a very small amount and then calculates
	* the spin changes. */
//	void smallstep(particle& part, long double time);
	
	/** Performs the necessary spin calculations.
	* This function spins the particle, and returns the phase change in the horizontal plane */
//	long double spin_calculation( vector3& spinvector, const long double gyromag, const vector3& mag_field, const long double time);

	/** Performs an interval step.
	* This means that it will run bounces for only a certain amount of time, for each particle.
	* @Returns a bool - false if should stop (i.e. bounces have run out) */
	void runinterval( long double intervaltime );

	/** Performs an interval step on an individual particle.
	* This should only ever be called by runinterval! */
	void runinterval( long double intervaltime, particle *part);
	
	/** HACK HACK HACK passes execution to runinterval. Really shitty method, but I can't see a better way to
	launch a thread into runinterval AND have it access member functions. partsleft is how many particles there are
	left to be processed.*/
	static void runruninterval ( edmexperiment *thisp, long double intervaltime, int* partsleft);//, particle *part );
	
	/** Runs a single phase loop.
	* The particles can be run over several starting phases for a single run. This function runs
	* a single phase, after resetting all phase-specific particle properties (i.e. all except the cumulative
	* edm value, though that should be held seperately anyway.
	* @param which phase loop we are on (out of the total) */
	void run_phaseloop( int phase_num ); 
	
	/** Updates the variation parameters.
	* This updates the parameter being varied for the object which is being varied. */
	void update_variationparameters ( int run_number );
	
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
