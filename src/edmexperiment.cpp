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
#include <cstdlib>
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

#include "reporters.h"
#include "tools.h"

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
	collision_offset = 0.;
	
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
	steptime = getlongdouble("steptime", 3e-4);
		
	phase_steps = getint("phase_steps", 1);
	bounces = getlong("bounces", 1);
	
	collision_offset = getlongdouble("collision_compensation_distance", 0.0);
	
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

	for (int exprun = 0; exprun < variation.runs; exprun++)
	{
		// Calculate the value for the variation this loop
		long double varyval = variation.minval + (variation.maxval - variation.minval)*exprun / variation.runs;
		logger << "Varying " << variation.parameter << ": Value = " << varyval;
		// now set it!
		variation.varyobject->set(variation.parameter, str(varyval));
		
		// Now Reset all the children
		BOOST_FOREACH ( nslobject *child, subobjects)
			child->reset();
		
		// First loop over starting phases
		for (int phase_loop = 0; phase_loop < phase_steps; phase_loop++)
		{
			logger << "Phase Averaging Loop " << phase_loop+1 << " of " << phase_steps << endl;

			// Reset the particle each phase
			BOOST_FOREACH( particle* part, particles ) {
				part->reset();
			}
			
			// Now loop over bounces
			for (int bounce = 0; bounce < bounces; bounce++)
			{
				// NOW loop over particles - having them here means that they move together
				BOOST_FOREACH( particle *part, particles)
				{
					// This section is run every bounce
					
					// Calculate the next point of intersection
					intercept	collisionpoint = particlebox->cast(part->position, part->velocity_vec);
					// Normalise the Normal! (see cast documentation)
					collisionpoint.normal.scaleto(1.0);
					// Fill out the collisionpoint location
					collisionpoint.location = part->position + (part->velocity_vec*collisionpoint.time);
					
					//logger << " " <<  bounce << ": Time to Collision: " << collisionpoint.time << endl;
					
					// If we get a zero-time collision, make a note of it
					if (collisionpoint.time == 0.)
						logger << "\t------- Zero time Intersection" << endl; //	throw runtime_error("Zero Collision-point time");
					
					// Now we know the time to collision, step over it calculating the spin changes as we go
					if (collisionpoint.time > 0.)
						bigstep(*part, collisionpoint.time);
					
					// Reflect the particle now
					if (collisionpoint.collideobject->reflection == container::reflection_specular) 
					{
						part->velocity_vec = part->velocity_vec - ((collisionpoint.normal * 2.)*(collisionpoint.normal * part->velocity_vec));
					} else { // Assume diffuse reflection otherwise (for now)
						part->velocity_vec.x = rand()-(RAND_MAX/2);
						part->velocity_vec.y = rand()-(RAND_MAX/2);
						part->velocity_vec.z = rand()-(RAND_MAX/2);
						
						// Ensure it faces away from the normal
						if ((part->velocity_vec * collisionpoint.normal) < 0.)
							part->velocity_vec *= -1.0;
					}
					// Ensure velocity is kept constant
					part->velocity_vec.scaleto(part->velocity);
					
					// Output the difference between particle and calculated impact position
					//logger << "\tPositional Difference: " << mod(part->position - collisionpoint.location) << endl;
					
					// Move the particle to the collision point, plus a tiny offset - should eliminate need for fudge
					// whilst having a minimal physical impact (preliminary tests indicate this is usually or order
					// 1e-30 anyway)
					part->position = collisionpoint.location + (collisionpoint.normal * collision_offset);
					
				} // FOREACH particle

				// Now call the reporters that report each bounce
				BOOST_FOREACH( reporter *repo, report_bounce) {
					repo->report(*this);
				}
					
			} //FOREACH bounce
		} // FOREACH phase
		
		// Calculate the EDM stuff for each particle
		dataset collective_fedm;
		BOOST_FOREACH(particle *part, particles) {
			edmcalcs(*part);
			collective_fedm += part->fake_edm;
		}
		
		// Now output the calculated edm values
		logger << "Calculated False-EDM : " << collective_fedm.average() << " +/- " << collective_fedm.stdev() << endl;
		
		
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

// Performs a large step between successive collisions. Use the E and B fields tied to
// the edmexperiment object for now
void edmexperiment::bigstep(particle& part, long double time)
{
	long steps;
	
	// Calculate the number of steps we are going to take
	steps = floorl(time / steptime);
	
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
	steplog << part.flytime << ", " << plusphase << ", " << minusphase << ", " << framediff << endl;
	part.E_sum_phase += plusphase;
	part.E_minus_sum_phase += minusphase;
	 */
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
