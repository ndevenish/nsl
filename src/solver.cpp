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


#include <string>

#include "solver.h"
#include "particle.h"
#include "electromagnetics.h"
#include "errors.h"
#include "physics.h"
#include "neutronphysics.h"
#include "edmexperiment.h"
#include "reporters.h"


#include "boost/foreach.hpp"

using std::runtime_error;




solver::solver()
{
	objecttype = "solver";
	types.push_back(objecttype);
	
	return;
}


midpointsolver::midpointsolver()
{
	objecttype = "midpointsolver";
	types.push_back(objecttype);
	
	return;
}

bool midpointsolver::prepareobject ( void )
{
	// Read in our steptime
	require("steptime");
	
	steptime = getlongdouble("steptime", 0.0);
	
	elecfield = (efield*)findbytype("efield", 1);
	if (!elecfield)
		throw runtime_error("Electric field not found from midpointsolver");
	
	magfield = (bfield*)findbytype("bfield", 1);
	if (!magfield)
		throw runtime_error("Magnetic field not found from midpointsolver");
	
	exp = (edmexperiment*)findbytype("edmexperiment", 1);
	if (!exp)
		throw runtime_error("Experiment not found from midpointsolver");
	
	if (exp->get("gravity") == "on")
		gravity = true;
	else
		gravity = false;
	
	return true;
}

void midpointsolver::prepareparticles ( std::vector<particle*> &particles )
{
	// go through each of the particles and set it's steptime
	BOOST_FOREACH( particle* part, particles) {
		part->steptime = steptime;
	}
	
}

// Run a step for the particle over the specified time. It can safely be assumed that the
// particle will not hit any objects during this time.
void midpointsolver::step( particle &part, const long double &time )
{
	long steps;
	
	// calculate the Exv effect for the particle- this does not change over a bigstep
	// NOTE: if gravity is on, this does change over a bigstep so don't do this here
	if (!gravity)
		part.updateExv(*elecfield);
/*
	////////////////////////////////////
	/// TEMPORARY DEBUG - match the other programs stepping algorithm
	steps = (int)(time/3.e-5) + 1;
	part.steptime = time / steps;
#warning "Temporary debug code"
*/	
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

void midpointsolver::smallstep( particle &part, const long double &time)
{
	long double halftime = time / 2.0;
	
	// Firstly calculate the step midpoint
	vector3 midpoint, newposition;
	midpoint = part.position + (part.velocity_vec*halftime);
	
	// Calculate the (non-gravitational) position after this step... this is because we recalculate
	// velocity here (gravitationally) and this messes up the later calculation!
	newposition = part.position += part.velocity_vec * time;
	
	// If gravity is turned on, adjust the relevant properties
	if (gravity)
	{
		// the midpoint is different...
		midpoint.z -= 0.5*g*halftime*halftime;
		
		// update the velocity at the halfway point
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
	part.position = newposition;
	
	//...and gravity adjustments
	if (gravity)
	{
		// Update the position properly
		part.position.z -= 0.5*g*time*time;
		
		// and finish adjusting the velocity with the other half of the step
		part.velocity_vec.z -= g*halftime;
		part.velocity = mod(part.velocity_vec);
	}
	
	// Update the particles flytime
	part.flytime += time;
	
	// Now spin the particle
	neutron_physics::spin_calculation(part, B, time);
	
	// Now (god forbid) call the reporters that report every step
	BOOST_FOREACH( reporter *rep, exp->report_step ) {
		rep->report(*exp);
	}
}
