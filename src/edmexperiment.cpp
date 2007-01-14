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





#include <iostream>
#include <fstream>

#include <cmath>
#include <cassert>
//#include <cstdlib>
#include <ctime>



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

using nsl::rand_uniform;

//#include "efence.h"
//#include "efencepp.h"

using std::runtime_error;
using std::endl;
using std::ofstream;
using std::ios;


//using std::floorl;


/*container *particlebox;
	particle *particles[MAX_PARTICLES];
	bfield * magfield;
	efield *elecfield;*/
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
			
		// Reset all the particles cumulative phase average things
		BOOST_FOREACH(particle *part, particles)
			part->cumulativeedm.reset();
			
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
				edmcalcs(*part);
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


// The task now is to run over bounces, but to stop if we run out of time
void edmexperiment::runinterval( long double time )
{
	
	// Run this routine for a particle-at-a-time - this may be better moved to the main loop
	// for easier paralellization
	
	// Use an iterator since the compiler threw a wobbly here when it was in the main
	// function and I don't want to tempt fate again
	std::vector<particle*>::iterator parts;
	for (parts = particles.begin(); parts != particles.end(); parts++)
	//BOOST_FOREACH( part, particles)
	{
		particle *part = *parts; // Reassign our working pointer from the little boost debacle
		
		// Reset the time counter for this particle
		long double timeleft = time;
		
		// Start a loop over bounces, but keep it infinite as if we are in lifetime mode then
		// we don't care about the bounces
		unsigned long bounce = 0; // Tracks the bounce count
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
			
			// If we get a zero-time collision, make a note of it
			if (collisionpoint.time == 0.)
				logger << "\t------- Zero time Intersection" << endl; //	throw runtime_error("Zero Collision-point time");
			
			/* Now we have three possibilities:
				1)	We have enough time for a bounce to complete
				2)	We don't so, have to go halfway
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
					bigstep(*part, collisionpoint.time);
				
				// Reflect the particle now
				if (collisionpoint.collideobject->reflection == container::reflection_specular) 
				{
					part->velocity_vec = part->velocity_vec - ((collisionpoint.normal * 2.)*(collisionpoint.normal * part->velocity_vec));
				} else { // Assume diffuse reflection otherwise (for now)
					
					
					// Generate a random z and theta
					part->velocity_vec.z = (rand_uniform()-0.5)*2.;
					
					long double phi = rand_uniform()*pi*2;
					// Calculate the flat-plane (z=0.) radius of this point
					long double planarr = cosl(asinl( part->velocity_vec.z ));
					
					part->velocity_vec.x = planarr * cosl(phi);
					part->velocity_vec.y = planarr * sinl(phi);

					// Old method - not perfect? Untested distribution.
//					part->velocity_vec.x = rand()-(RAND_MAX/2);
//					part->velocity_vec.y = rand()-(RAND_MAX/2);
//					part->velocity_vec.z = rand()-(RAND_MAX/2);
					
					// Ensure it faces away from the normal
					if ((part->velocity_vec * collisionpoint.normal) < 0.)
						part->velocity_vec *= -1.0;
				}

				// Scale the velocity up to the particles velocity.
				part->velocity_vec *= part->velocity;

				// Take away the time for the travel for this bounce from the time left
				timeleft -= collisionpoint.time;
				
				// Move the particle to the collision point, plus a tiny offset - should eliminate need for fudge
				// whilst having a minimal physical impact (preliminary tests indicate this is usually or order
				// 1e-30 anyway)
				part->position = collisionpoint.location + (collisionpoint.normal * collision_offset);
				
				// See if this was a wall, and if not then reduce the bouncecount
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
			}
			else
			{
				// Now process the case where there is not enough time for a full trip across the bottle..
				// .. do a partial trip
				bigstep(*part, timeleft);
				timeleft = 0; // probably not needed - but better safe...
				
				// finish this and go to the next particle
				break;
			}
		
		} //FOREACH bounce
	
	} // FOREACH particle

	// This ending point should be reached by both modes of looping
	
	return;
}







// Performs a large step between successive collisions. Use the E and B fields tied to
// the edmexperiment object for now
void edmexperiment::bigstep(particle& part, long double time)
{
	long steps;
	
	// Calculate the number of steps we are going to take
	steps = (long)floorl((long double)time / (long double)steptime);
	
	// calculate the ExB effect for the particle- this does not change over a bigstep
	// Now (here for now) calculate the VxE effect
	vector3 vxE; vector3 E;
	elecfield->getfield(E, part.position);
	part.vxEeffect = (crossproduct(E, part.velocity_vec) * part.vgamma)/ csquared;

	// WAAGH WAAAGH WAAAGH Setting this to zero for debugging
	//part.vxEeffect *= 0;
	
	
	// Now do this many smallsteps - after which we will have one step of time less
	// than one step to complete
	for (int step = 0; step < steps; step++)
	{
		// We can safely callthe smallstep with steptime as inside this loop it is 
		// guaranteed to be so
		smallstep(part, steptime);

	}

	// Check we still have a little excess time to step
	long double laststep = (time - (steps*steptime));
	if (laststep < 0.0)
		throw runtime_error("Stepping routine stepped over maximum time to impact");
	// Now do the actual final step
	smallstep(part, laststep);
	static ofstream lasts("laststep.txt");
	lasts.precision(20);
	lasts << laststep << endl;
}

void edmexperiment::smallstep(particle& part, long double time)
{
//	time = 0.00014977692797960484496350153094113011;
	
	// Firstly calculate the step midpoint
	vector3 midpoint;
	midpoint = part.position + (part.velocity_vec*(time/2.0));
	
	// Just grab the magnetic field - now we have precalculated the magnetic field
	// for the particle already
	vector3 B;//, E;
	magfield->getfield(B, midpoint);
	//elecfield->getfield(E, midpoint);
	
	
	// Now calculate the two different magnetic fields
	vector3 BplusvxE, BminusvxE;
	BplusvxE = B + part.vxEeffect;
	BminusvxE = B - part.vxEeffect;
	
	// Now apply the new physical properties to the particle
	
	// Move it first
	part.position += part.velocity_vec * time;
	
	// Update the particles flytime
	part.flytime += time;
	
	/*
	static long double sumtime = 0;
	vector3 spin = part.spinEplus;
	
	sumtime += time;
	long double a, b, c;
	
	a = spin_calculation(spin, part.gamma, BplusvxE, time);
	spin = part.spinEplus;
	b = 100. * spin_calculation(spin, part.gamma, BplusvxE, time/10.);
	
	c = a - b;
	tmpld = c;
	*/
	
	
	// Now spin it both ways 
	//BplusvxE = vector3(8.38780388e-13, -2.6690424730e-11, 1.0e-6);
	//BminusvxE = vector3(-8.38780388e-13, 2.669042473e-11, 1.0e-6);
	//time = 0.0001497769279;
	
	long double plusphase, minusphase, framediff;
	plusphase		= spin_calculation(part.spinEplus,	 part.gamma, BplusvxE,  time);
	minusphase		= spin_calculation(part.spinEminus, part.gamma, BminusvxE, time);
	framediff = plusphase - minusphase;
	/*
	static ofstream steplog("steplog.txt");
	steplog << part.flytime << ", " << plusphase << ", " << minusphase << ", " << framediff << endl; */
	part.E_sum_phase += plusphase;
	part.E_minus_sum_phase += minusphase;
	
	/*
	part.E_sum_phase		+= spin_calculation(part.spinEplus,	 part.gamma, BplusvxE,  time);
	part.E_minus_sum_phase	+= spin_calculation(part.spinEminus, part.gamma, BminusvxE, time);
	*/
	
	// Now (god forbid) call the reporters that report every step
	BOOST_FOREACH( reporter *rep, report_step ) {
		rep->report(*this);
	}
}

long double edmexperiment::spin_calculation( vector3 &spinvector, const long double gyromag, const vector3& mag_field, const long double time )
{
//	static ofstream steplog("steplog.txt");

	// Let's twist again, like we did last summer
	vector3 oldspin = spinvector;//vector3(spinvector.x, spinvector.y, 0.0);
	long double oldxylength = sqrtl(spinvector.x*spinvector.x + spinvector.y*spinvector.y);
	
	// Calculate the change in spin
	vector3 dS = time * gyromag * crossproduct(spinvector, mag_field);
	
	// Now apply this to the spin vector
	spinvector += dS;
	
	// Scale it to ensure that it remains of constant length
	spinvector.scaleto(1.0);
	
	// Calculate the new length of the xy vectorshar
	long double newxylength = sqrtl(spinvector.x* spinvector.x + spinvector.y*spinvector.y);

	//calculate the planar angle shift from the dot product	
	long double cosphase = (oldspin.x*spinvector.x + oldspin.y*spinvector.y) / (oldxylength*newxylength);
	
	if (cosphase > 1.)
		logger << "Cosphase > 1 by : " << lddistance(cosphase, 1.) << endl;;//throw runtime_error("Cosphase > 1");
	
	long double phase_change = acos(cosphase);
	
	return phase_change;
}

void edmexperiment::edmcalcs( particle &part )
{
	// Firstly calculate the frequency differences between the two spins
	part.frequencydiff = atan2( part.spinEplus.x * part.spinEminus.y - part.spinEplus.y*part.spinEminus.x,
								part.spinEplus.x*part.spinEminus.x + part.spinEplus.y*part.spinEminus.y );
	part.frequencydiff /= part.flytime;
	/*
	atan2((particle->spin_x*particle->minusE_spin_y - particle->spin_y*particle->minusE_spin_x),
							   (particle->spin_x*particle->minusE_spin_x + particle->spin_y*particle->minusE_spin_y)	            
							   )/particle->tot_time;
	 */
	
	// Grab the E field vertical charge
	vector3 E;
	elecfield->getfield(E, vector3(0,0,0));
	long double E_FIELD = E.z;
	
	// Now calculate the EDM from this
	part.fake_edm = part.frequencydiff * hbar/E_FIELD/echarge*100*1e26/4;
}


