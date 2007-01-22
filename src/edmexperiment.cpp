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

// Standard Headers
#include <iostream> // For endl, and cout inside exception handlers

// C Headers
#include <cmath>   // for maths functions
#include <cassert> // For assertion
#include <ctime>   // For getting the current time

// Boost headers for boost functions we are using
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

// Include our local copy to src, as foreach is not a standard yet
#include "boost/foreach.hpp"

#include "errors.h"
#include "nslobject.h"
#include "nslobjectfactory.h"
#include "container.h"
#include "electromagnetics.h"
#include "particle.h"
#include "edmexperiment.h"
#include "physics.h"
#include "datasets.h"
#include "tools.h"
#include "reporters.h"
#include "random.h"
#include "neutronphysics.h"
#include "solver.h"

using std::runtime_error;
using std::endl;
using std::ios;
using std::cout;

using nsl::rand_uniform;






edmexperiment::edmexperiment()
{
	
	particlecount = 0;
	//reportercount = 0;
	particlebox = 0;
	magfield = 0;
	elecfield = 0;
	
	variation.parameter = "";
	variation.value = 0.;
	variation.runs = 0;
	variation.varying = false;
	
	gravity = false;
	
	phase_steps = 0;
	steptime = 0.0;
	bounces = 0;
	lifetime = 0.0;
	collision_offset = 0.;	
	//initvals();
	
	
	objecttype = "edmexperiment";
	types.push_back(objecttype);
}

void edmexperiment::setvariation( std::string parameter, long double minval, long double maxval, int maxruns, nslobject *varyobject )
//void edmexperiment::setvariation( std::string parameter, long double thisrunvalue, int maxruns )
{
	if (variation.varying)
		throw runtime_error("Trying to set variation in edmexperiment object twice");
	
	variation.parameter = parameter;
//	variation.value = thisrunvalue;
	variation.runs = maxruns;
	variation.maxval = maxval;
	variation.minval = minval;
	variation.varyobject = varyobject;
	
	variation.varying = true;
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
	//for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
	BOOST_FOREACH( nslobject *sublist, subobjects )
	{
		if (sublist->isoftype("container"))
			particlebox = (container*)sublist;
	}

	// And that we have at least one particle
	if ((particlecount = countchildren("particle")) < 1)
		throw runtime_error("No particles specified under edmexperiment object");
	// Add the particles to the list
	//for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
	BOOST_FOREACH( nslobject *subobject, subobjects )
	{
		if (subobject->isoftype("particle"))
//			particles[particlecount++] = (particle*)sublist;
			particles.push_back((particle*)subobject);
	}
	
	// Now collect the reporters together in their piles
	BOOST_FOREACH( nslobject *subobject, subobjects )
	{
		if (subobject->isoftype("reporter"))
		{
			switch (((reporter*)subobject)->when()) {
				case reporter::rfreq_run:
					report_run.push_back((reporter*)subobject);
					break;
				case reporter::rfreq_step:
					report_step.push_back((reporter*)subobject);
					break;
				case reporter::rfreq_bounce:
					report_bounce.push_back((reporter*)subobject);
					break;
				case reporter::rfreq_none:
					break; // Don't do anything in this case
				case reporter::rfreq_interval:
					report_interval.push_back((reporter*)subobject);
					break;
			}
		}
	}
					
					
					
					
					
					//reporters[reportercount++] = (reporter*)(*sublist);
	
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

	// Find and grab the solver object!
	if (countchildren("solver") != 1)
		throw runtime_error("Could not find one solver!");
	thesolver = (solver*)findbytype("solver", 0);
	if (!thesolver)
		throw runtime_error("Could not find solver object!");
	
	
	// Grab the steptime from the property set
	//if (isset("steptime"))
	
	// Log the start time of this simulation run
	time_t rawtime;
	tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	set("runtime",  asctime(timeinfo));
	
	// If we are at this point we have the particlebox, particles, bfield and efield objects
	// configured

	return true;
}

void edmexperiment::readsettings ( void )
{
	steptime = getlongdouble("steptime", 3e-4);
	
	phase_steps = getint("phase_steps", 1);
	bounces = getlong("bounces", 1);
	lifetime = getlongdouble("lifetime", 0.0);
	
	// If we have set a lifetime, null out the bounces
	uselifetime = isset("lifetime");

	// If no intervaltime set, then set it to lifetime + 1 second
	if (uselifetime)
	{
		if (!isset("intervaltime"))
			intervaltime = lifetime + 1.0;
		else
			intervaltime = getlongdouble("intervaltime", 0.0);
	}
	
	collision_offset = getlongdouble("collision_compensation_distance", 0.0);
	
	if(get("gravity", "off") == "on")
		gravity = true;
	else
		gravity = false;
}

bool edmexperiment::runobject()
{
	// Dataset to track the false EDM
//	dataset false_edm;

	// Ensure that all of our particles are within the starting volume
	BOOST_FOREACH( particle* part, particles) {
		if (!particlebox->isinside(part->position))
			throw runtime_error("Particle is not starting inside of a container volume");
	}


	// for now assume that everything is set up correctly
	
	// Tell our reporters to initialise, here for now.
	BOOST_FOREACH( nslobject *ob, subobjects) {
		if (ob->isoftype("reporter"))
			((reporter*)(ob))->preparefile(*this);
	}
	
	////////////////////////////////////////////////////////////////////////////
	// Start the simulation loops

	// give it something to vary
	if (!variation.varying)
	{
		variation.minval = 1;
		variation.maxval = 1;
		variation.runs = 1;
		variation.parameter = "None";
		variation.varying = false;
		variation.varyobject = this;
	}
	
	// shout out what we are doing loopwise
	if (uselifetime)
		if (intervaltime < lifetime)
			logger << "Lifetime-based looping: Intervals of " << intervaltime << " for " << lifetime << " (seconds)" << endl;
		else
			logger << "Lifetime-based looping for " << lifetime << " seconds" << endl;
	else
		logger << "Using bounce-based looping: For " << bounces << " bounces." << endl;
		
	// Run over experimental runs
	for (int exprun = 0; exprun < variation.runs; exprun++)
	{
		// Calculate the value for the variation this loop
		long double varyval = variation.minval + (variation.maxval - variation.minval)*exprun / variation.runs;
		logger << "Outer Loop " << exprun+1 << "/" << variation.runs << " of " << variation.parameter << ": Value = " << varyval << endl;
		// now set it!
		variation.varyobject->set(variation.parameter, str(varyval));
		variation.value = varyval;
		
		// Resetting this should also get all our children to read!
			this->reset();
			
		// Reset all the particles cumulative phase average things,
		// and set each particles steptime to be our global steptime
		BOOST_FOREACH(particle *part, particles)
		{
			part->cumulativeedm.reset();
			part->steptime = steptime;
		}
			
		// First loop over starting phases
		for (int phase_loop = 0; phase_loop < phase_steps; phase_loop++)
		{
			logger << "   Phase Averaging Loop " << phase_loop+1 << " of " << phase_steps << endl;
			// Calculate the phase
			long double phaseavg = phase_loop * (2.0 / phase_steps);
			// Reset the particle each phase, and set it's phase
			BOOST_FOREACH( particle* part, particles ) {
				part->set("spin_phase", str(phaseavg));
				part->reset();
			}
		
			
			// Decide how to loop - two choices:
			// 1) Bounce for a certain number of bounces
			// 2) Start a loop for intervals - this will run all the particles for a specified time,
			//    after which they will all have identical running times (so that reports that
			//    depend on having an ensemble of equal-time particles can be made)
			
			bool runningintervals = true; // are we still runnning/planning to run intervals?
			
			// If we are not doing lifetime based looping, don't even bother with the loop
			if (!uselifetime)
			{
				runningintervals = false; // This cancels the loop ahead so it won't happen
				runinterval(0.0); // This runs the interval - it doesn't care about time in this case
			}
			
			// Loop over the time intervals, as long as we have time left.. signified by 
			// a counter of life!
			long double timeleft = lifetime; //lifetime remaining
			while (runningintervals)
			{
				// Loop through, running the simulation for an intervals length
				// Firstly, check to see if we have enough time left!
				if (timeleft < intervaltime)
				{
					// We have less than one intervals time remaining... run what is left
					// then check the var to quit the loop
					runinterval(timeleft);
					timeleft = 0;
					runningintervals = false;
				} else {
					// We have more than one interval left, so run one and reduce that from the overall
					// time remaining.
					runinterval(intervaltime);
					timeleft -= intervaltime;
				}
				
				// Call the interval reporters
				BOOST_FOREACH( reporter *rep, report_interval )
					rep->report(*this);
			} // End of running intervals
			// At this point we have run over al time, whether it be from bounce or interval based procedures
			
			// Calculate an edm for each particle, and accumulate it
			BOOST_FOREACH(particle *part, particles) {
				neutron_physics::edmcalcs(*part, *elecfield);
				part->cumulativeedm += part->fake_edm;
//				falseedmav += part->fake_edm;
			}
		} // FOREACH phase
		
		// Calculate the edm!
		//dataset collective_fedm;
		dataset falseedmav;
		BOOST_FOREACH(particle *part, particles)
		{
			falseedmav += part->cumulativeedm;
		}
		
		// Now output the calculated edm values
		logger << "   Calculated False-EDM : " << falseedmav.average() << " +/- " << falseedmav.uncert() << endl;
		
		
		// Now tell all the reporters that are supposed to report every run, to report
		BOOST_FOREACH( reporter *rep, report_run ) {
			rep->report(*this);
		}
		
	} // Close per value loop
	
	// Tell all of the reporters to do the file-closing 'final report'
	BOOST_FOREACH( nslobject *ob, subobjects) {
		if (ob->isoftype("reporter"))
			((reporter*)(ob))->closefile(*this);
	}
		
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Runinterval function


// The first of the runinterval functions - we are handed a time and tasked with running
// the particles for this amount of time (over bounces or whatever).
// This particular function is now responsible for launching threads which then sort out
// the tasks themselves.
void edmexperiment::runinterval( long double time )
{
	
	// How many threads do we want doing the particles?
	const int thread_max = 2;
	
	// Build the storage that all threads will store access to
	int partsleft = particles.size();
	
	// Create the controller for the threads
	boost::thread_group threads;
	
	// Just launch a bunch of threads, and let them sort out the mess. (which particles to do)
	for (int thread_count = 0; thread_count < thread_max; thread_count++)
	{
		// Launch a thread - the function has to be static which is why we pass a this pointer
		threads.create_thread( boost::bind(&edmexperiment::runruninterval, this, time, &partsleft) );
	}

	// Sit and wait for all of our little pets to return to us
	threads.join_all();
	
	return;
}

/** Launching point for a thread to process particles. * * * * * * * * * * * * * * * * 
* Threads are created into this function, where they sort out getting a new particle  *
* to process, and return once there are no more jobs left to do.					  *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void edmexperiment::runruninterval ( edmexperiment *thisp, long double intervaltime, int *partsleft )
{
	// Enclose this in an exception block to catch any exceptions... This doesn't otherwise
	// seem to happen with threads - Any execution here is going to be in a thread seperate from
	// the main one
	try {
		// Loop infinitely and let threads find out inside whether there is anything left to do
		while(1)
		{
			// Which particle shall we run?
			int partnum = 0;
			
			// Read in the Next particle to process... it is slightly easier to do this
			// backwards, counting down the number of particles left. This definitely has to
			// be mutex locked otherwise we could get multiple threads acting on the same particle
			{
				// We want a static local mutex so that all threads passing through here can share it.
				static boost::mutex readnum_mutex;
				boost::mutex::scoped_lock lock(readnum_mutex);
				
				partnum = --(*partsleft);
			}
			
			// Have we exhausted all particles? If so, quit this function (and thread).
			if (partnum < 0)
				return;
			
			// We have generated a particle index number... run it's interval!
			thisp->runinterval(intervaltime, thisp->particles[partnum]);
			
		} //while(1)
		
	// do the exception handling stuff
	} catch (runtime_error runt) {
		cout << "\a\a\aA runtime error has occured in a seperate thread";
		if (runt.what())
			cout << ": " << runt.what();
		cout << endl << endl;
		assert(0); // Kill the application!
	} catch (...) {
		cout << "\aAn unrecoverable, unidentified exception occured - terminating program." << endl;
		assert(0); // Kill the application!
	}
	
}

// This is the second (and original) runinterval function. Rather than processing threads, this
// actually does what it's name says - runs a particle for a specified length of time!
void edmexperiment::runinterval ( long double time, particle *part )
{
	// Reset the time counter for this particle
	long double timeleft = time;
	unsigned long bounce = 0; // Tracks the bounce count
	
	// Start a loop over bounces, but keep it infinite, as if we are in lifetime mode then
	// we don't care about the bounces and we can decide on this later
	while (1)
	{
		// If we are doing bounce-based looping, track that here
		// Stop if we have reached the maximum number of bounces
		if (!uselifetime)
			if((unsigned long)bounces == (unsigned long)(bounce++))
				break;
			
		// Calculate the next point of intersection
		intercept	collisionpoint = particlebox->cast(part->position, part->velocity_vec);
		
		// Complete the collisionpoint variable with stuff not calculated in cast
		collisionpoint.normal.scaleto(1.0);
		collisionpoint.location = part->position + (part->velocity_vec*collisionpoint.time);
		// add the adjustments for gravity, if we are using it
		if (gravity)
			collisionpoint.location.z -= 0.5*g*collisionpoint.time*collisionpoint.time;
		
		// If we get a zero-time collision, make a note of it
		if (collisionpoint.time == 0.)
			logger << "\t------- Zero time Intersection" << endl; //	throw runtime_error("Zero Collision-point time");
		
		/* Now we have three possibilities:
			1)	We have enough time for a bounce to complete
			2)	We don't, so have to go halfway
			3)	We are bounce-looping - in which case (for now) short-circuit the test */
		
		// If doing bounce mode, short circuit the test ahead DISASSEMBLE
		if (!uselifetime)
			timeleft = collisionpoint.time + 1.;
		
		// check to see what the case is
		if (collisionpoint.time < timeleft)
		{
			// We have no problem with time here!
			
			
			// Now we know the time to collision, step over it calculating the spin changes as we go
			if (collisionpoint.time > 0.)
				//bigstep(*part, collisionpoint.time);
				thesolver->step(*part, collisionpoint.time);
			
			// Calculate the reflection - now done by the container being hit
			collisionpoint.collideobject->reflect(part->velocity_vec, collisionpoint.normal, part->velocity);
			
			// Take away the time for the travel for this bounce from the time left
			timeleft -= collisionpoint.time;
			
			// Move the particle to the collision point, plus a tiny offset - should eliminate need for fudge
			// whilst having a minimal physical impact (preliminary tests indicate this is usually of order
			// 1e-30 anyway)
			// Note... this automatically accounts for gravity via collisionpoint.location
			part->position = collisionpoint.location + (collisionpoint.normal * collision_offset);
			
			
			// See if this was a wall, and if not then reduce the bouncecount.
			// This is for compatability with the old system, which didn't count ceiling bounces as
			// true bounces.
			if (collisionpoint.normal.z < 0.001)
				;//logger << "Wall bounce" << endl;
			else {
				//logger << "ceil bounce";
				bounce--;
			}
			
			// Now call the reporters that report each bounce
			BOOST_FOREACH( reporter *repo, report_bounce) {
				repo->report(*this);
			}
		} else {
			// Now process the case where there is not enough time for a full trip across the bottle..
			// .. just do a partial trip
			//bigstep(*part, timeleft);
			thesolver->step(*part, timeleft);
			timeleft = 0; // probably not needed - but better safe...
		}
	} //while(1) over bounces	
}




/*
// Performs a large step between successive collisions. Use the E and B fields tied to
// the edmexperiment object for now
void edmexperiment::bigstep(particle& part, long double time)
{
	long steps;
	
	// calculate the Exv effect for the particle- this does not change over a bigstep
	// NOTE: if gravity is on, this does change over a bigstep so don't do this here
	if (!gravity)
		part.updateExv(*elecfield);

	// Calculate the number of steps we are going to take
	steps = (long)floorl((long double)time / (long double)part.steptime);
	
	// Now do this many smallsteps - after which we will have one step of time less
	// than one step to complete
	for (int step = 0; step < steps; step++)
	{
		// We can safely callthe smallstep with steptime as inside this loop it is 
		// guaranteed to be so
		smallstep(part, part.steptime);

	}

	// Check we still have a little excess time to step
	long double laststep = (time - (steps*part.steptime));
	if (laststep < 0.0)
		throw runtime_error("Stepping routine stepped over maximum time to impact");
	// Now do the actual final step
	smallstep(part, laststep);
}
*/

/*
void edmexperiment::smallstep(particle& part, long double time)
{
	long double halftime = time / 2.0;
	
	// Firstly calculate the step midpoint
	vector3 midpoint;
	midpoint = part.position + (part.velocity_vec*halftime);

	// If gravity is turned on, adjust the relevant properties
	if (gravity)
	{
		// the midpoint is different...
		midpoint.z -= 0.5*g*halftime*halftime;
		
		// Update the particles velocity....
		part.velocity_vec.z -= g*halftime;
		part.velocity = mod(part.velocity_vec);
		
		// And the particles Exv effect
		part.updateExv(*elecfield);
	}
	
	// Just grab the magnetic field - now we have precalculated the magnetic field
	// for the particle already
	vector3 B;
	magfield->getfield(B, midpoint);


	// Now apply the new physical properties to the particle
	// Move it first
	part.position += part.velocity_vec * time;
	
	//...and gravity adjustments
	if (gravity)
	{
		// Update the position properly
		part.position.z -= 0.5*g*time*time;
		
		//and finish adjusting the velocity with the other half of the step
		part.velocity_vec.z -= g*halftime;
		part.velocity = mod(part.velocity_vec);
		
	}
	
	// Update the particles flytime
	part.flytime += time;
	
	// Now spin the particle
	neutron_physics::spin_calculation(part, B, time);
	
	// Now (god forbid) call the reporters that report every step
	BOOST_FOREACH( reporter *rep, report_step ) {
		rep->report(*this);
	}
}

*/