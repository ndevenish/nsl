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

#include "errors.h"

#include "nslobject.h"
#include "nslobjectfactory.h"

#include "container.h"
#include "electromagnetics.h"
#include "particle.h"

#include "edmexperiment.h"

#include "physics.h"

using std::runtime_error;
using std::endl;
using std::ofstream;

//using std::floorl;


/*container *particlebox;
	particle *particles[MAX_PARTICLES];
	bfield * magfield;
	efield *elecfield;*/
edmexperiment::edmexperiment()
{
	particlecount = 0;
	particlebox = 0;
	magfield = 0;
	elecfield = 0;
	for (int i = 0; i < MAX_PARTICLES; i++)
		particles[i] = 0;

	phase_steps = 0;
	steptime = 0.0;
	bounces = 0;
	collision_offset = 0.;
	
	objecttype = "edmexperiment";
	types.push_back(objecttype);
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
	for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
		if ((*sublist)->isoftype("container"))
			particlebox = (container*)(*sublist);

	// And that we have at least one particle
	if (countchildren("particle") < 1)
		throw runtime_error("No particles specified under edmexperiment object");
	// Add the particles to the list
	for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
		if ((*sublist)->isoftype("particle"))
			particles[particlecount++] = (particle*)(*sublist);
	
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
	
	// If we are at this point we have the particlebox, particles, bfield and efield objects
	// configured

	return true;
}

bool edmexperiment::runobject()
{
	ofstream poslog("poslog.txt");
	
	// Ensure that all of our particles are within the starting volume
	for (int i = 0; i < particlecount; i++)
		if (!particlebox->isinside(particles[i]->position))
			throw runtime_error("Particle is not starting inside of a container volume");


	// for now assume that everything is set up correctly
	
	// FOR EACH PARTICLE
	particle *part;
	int partloop = 0;
	for (partloop = 0; partloop < particlecount; partloop++)
	{
		// Assign the current particle
		part = particles[partloop];
		
		//We want to perform this calculation over multiple starting phases
		for (int phase_loop = 0; phase_loop < phase_steps; phase_loop++)
		{
			logger << "Phase Averaging Loop " << phase_loop+1 << " of " << phase_steps << endl;
			
			for (int bounce = 0; bounce < bounces; bounce++)
			{
				// Log each bounces position to a file (this will not output the last bounce here, but that
				// doesn't really matter)
				poslog << bounce << ", " << part->position << endl;
			
				// This is run every bounce
				intercept	collisionpoint = particlebox->cast(part->position, part->velocity_vec);
				
				// Normalise the Normal! (see cast documentation)
				collisionpoint.normal.scaleto(1.0);
				// Fill out the collisionpoint location
				collisionpoint.location = part->position + (part->velocity_vec*collisionpoint.time);
				
				logger << " " <<  bounce << ": Time to Collision: " << collisionpoint.time << endl;
				
				if (collisionpoint.time == 0.)
					logger << "\t------- Zero time Intersection" << endl; //	throw runtime_error("Zero Collision-point time");
					
				// Now we know the time to collision, cast a bigstep
				if (collisionpoint.time > 0.)
					bigstep(*part, collisionpoint.time);
				
				// Reflect the particle now
				if (collisionpoint.collideobject->reflection == container::reflection_specular) 
				{
					part->velocity_vec = part->velocity_vec - ((collisionpoint.normal * 2.)*(collisionpoint.normal * part->velocity_vec));
				} else { // diffuse reflection
					part->velocity_vec.x = rand()-(RAND_MAX/2);
					part->velocity_vec.y = rand()-(RAND_MAX/2);
					part->velocity_vec.z = rand()-(RAND_MAX/2);
					
					// Make sure it faces away from the normal
					if ((part->velocity_vec * collisionpoint.normal) < 0.)
						part->velocity_vec *= -1.0;
				}
				// Ensure velocity is kept constant
				part->velocity_vec.scaleto(part->velocity);
				
				// Output the difference between particle and calculated impact position
				logger << "\tPositional Difference: " << mod(part->position - collisionpoint.location) << endl;
				
				// Move the particle to the collision point, plus a tiny offset - should eliminate need for fudge
				// whilst having a minimal physical impact (preliminary tests indicate this is usually or order
				// 1e-30 anyway)
				part->position = collisionpoint.location + (collisionpoint.normal * collision_offset);
				
			}
			// This is run every phase loop
		}
		// Run every particle
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
	
	// Now do this many smallsteps - after which we will have one step of time less
	// than one step to complete
	for (int step = 0; step < steps; step++)
	{
		// We can safely callthe smallstep with steptime as inside this loop it is 
		// guaranteed to be so
		smallstep(part, steptime);
	}

	// Check we still have a little excess time to step
	if ((time-(steps*steptime)) < 0.0)
		throw runtime_error("Stepping routine stepped over maximum time to impact");
	// Now do the actual final step
	smallstep(part, time-(steps*steptime));
}

void edmexperiment::smallstep(particle& part, long double time)
{
	// Firstly calculate the step midpoint
	vector3 midpoint;
	midpoint = part.position + (part.velocity_vec*(time/2.0));
	
	// Now grab the electric and magnetic fields at this point
	vector3 E, B;
	magfield->getfield(B, midpoint);
	elecfield->getfield(E, midpoint);
	
	// Now (here for now) calculate the VxE effect
	vector3 vxE;
	vxE = crossproduct(E, part.velocity_vec) / csquared;
	
	// Now calculate the two different magnetic fields
	vector3 BplusvxE, BminusvxE;
	BplusvxE = B + vxE;
	BminusvxE = B - vxE;
	
	
	
	
	// Now apply the new physical properties to the particle
	// Move it first
	part.position += part.velocity_vec * time;
	
	// Now spin it
	spin_calculation(part.spinEplus, BplusvxE, time);
	spin_calculation(part.spinEminus, BminusvxE, time);
	
}

void edmexperiment::spin_calculation( vector3 &spinvector, const vector3& mag_field, const long double time )
{
	// for now, don't do anything
	return;
}
