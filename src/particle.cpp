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
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "nslobject.h"
#include "nslobjectfactory.h"

#include "particle.h"
#include "container.h"

#include "physics.h"
#include "vector3.h"
#include "errors.h"

#include "random.h"
#include "electromagnetics.h"

#include "neutronphysics.h"

using std::runtime_error;
using std::string;

using nsl::rand_normal;
using std::endl;

particle::particle()
{
	initvals();
	
	objecttype = "particle";
	types.push_back(objecttype);
}

void particle::initvals( void )
{
	particlebox = 0;
	fake_edm = 0.;
	
	flytime = 0.;
	
	gamma = 0.0;
	position.x = position.y = position.z = 0.0;
	velocity_vec.x = velocity_vec.y = velocity_vec.z = 0.0;
	velocity = 0.0;
	spinEplus.x = spinEplus.y = spinEplus.z = 0.0;
	spinEminus = spinEplus;
	
	E_sum_phase = E_minus_sum_phase = 0.0;
	
	vxEeffect *= 0;
	
}
bool particle::prepareobject()
{
	// Find a container!
	particlebox = (container*)findbytype("container");
	if (!particlebox)
		throw runtime_error("Unable to find a container for particle");
	
	return true;
	
}
void particle::readsettings(void)
{
	// Reset all the per-phase run variables here
	fake_edm = 0.;
	flytime = 0.;
	frequencydiff = 0.;
	E_sum_phase = E_minus_sum_phase = 0.;
	
	// Get and validate the velocities
	read_velocitysettings();

	// Get and validate the positions
	read_positionsettings();
	
	// Read the spin settings
	read_spinsettings();
	
	// Find a magnetic field to link to
	mag_field = (bfield*)findbytype("bfield");
	
	// Look for an electric field to link to
	elec_field = (efield*)findbytype("efield");
}

void particle::read_velocitysettings( void )
{
	// First get the vector if we have one
	if (isset("vx") || isset("vy") || isset("vz"))
	{
		velocity_vec.x = getlongdouble("vx", 0.0);
		velocity_vec.y = getlongdouble("vy", 0.0);
		velocity_vec.z = getlongdouble("vz", 0.0);
	} else {
		velocity_vec.x = velocity_vec.y = velocity_vec.z = 1.0;
	}
	
	if (isset("velocity")) {
		velocity = getlongdouble("velocity", 0.0);
		velocity_vec.scaleto(velocity);
	} else {
		velocity = velocity_vec.mod();
	}
	
	// Warn for zero velocity
	if (velocity == 0.0)
		Warning("Velocity in particle is unset (or set to 0.0)");
	
	/////////////////////////////////////////////
	// Maxwell-Boltzmann distribution
	if (get("maxwelldistribution", "off") == "on")
		generate_maxwellianvelocity();
	
	// Calculate the relativistic gamma factor
	vgamma = sqrtl(1. / (1. - (velocity*velocity)/csquared));
	
}

void particle::generate_maxwellianvelocity( void )
{
	//We want this particle to have a velocity on a maxwell-boltzmann distribution
	
	// Don't do it if we have no mass information
	if (!isset("mass"))
		throw runtime_error("Cannot set maxwell-boltzmann velocity without mass information.");

	// Read in the mass information
	mass = getlongdouble("mass", 0);
	if (mass <= 0.)
		throw runtime_error("Zero and negative mass particles unsupported");
	
	// Calculate the effective temperature for the desired maximum velocity
	long double T = (velocity*velocity * mass ) / (2.*k);
	// Precalculate the factor for this
	long double factor = sqrtl((k*T) / mass );
	
	// Check we are not above the cutoff
	if (getlongdouble("maxwelliancutoff", position.z + 1.) < position.z)
		throw runtime_error("Particle start position is higher than maxwellian cutoff");
	
	// Loop until we have a valid velocity
	while(1)
	{
		// Generate random maxwellian velocities - http://research.chem.psu.edu/shsgroup/chem647/project14/project14.html
		velocity_vec.x = rand_normal() * factor;
		velocity_vec.y = rand_normal() * factor;
		velocity_vec.z = rand_normal() * factor;
		
		// Read back the velocity magnitude
		velocity = velocity_vec.mod();
		
		// Are we using a cutoff height? If so, see if we are within it
		if (isset("maxwelliancutoff")) {
			// Is this above our cutoff?
			long double cutoff = sqrtl(2*g*(getlongdouble("maxwelliancutoff", 0.79) - position.z));
			
			// If not, it is valid! otherwise recast.
			if (velocity < cutoff)
				break;
		} else {
			break;
		}
	} // while(1)
}

void particle::read_positionsettings( void )
{
	position.x = getlongdouble("x", 0.0);
	position.y = getlongdouble("y", 0.0);
	position.z = getlongdouble("z", 0.0);
	
	// See if we have set a startvolume
	string startvolume;
	if (isset("startvolume"))
	{
		startvolume = get("startvolume");
		
		string startposition;
		startposition = get("startposition", "center");
		/*if (isset("startposition"))
			startposition = get("startposition");
		else
			startposition = "center";
		*/
		// Get the base position for this 
		nslobject *startvol;
		startvol = particlebox->findbyname(startvolume);
		if (startvol->isoftype("container"))
			position = ((container*)startvol)->getposition(startposition);
		else
			throw runtime_error("Attempting to start particle in non-container volume");
	}
}

void particle::read_spinsettings( void)
{
	// Gamma value
	gamma = getlongdouble("gamma", 0.0);
	if (!isset("gamma"))
		Warning("Gamma in particle is unset (or set to 0.0)");
	// Multiply by 2pi Because we will mostly want to use this setting
	// NOTE: Be careful if changing this in future, as is assumed to be 2*pi*gamma elsewhere
	gamma *= 2. * pi;
	
	// ~~~~~ Spin stuff
	long double start_spin_polar_angle, start_spin_phase;
	// Start by getting the spin polar angles
	start_spin_polar_angle = pi*getlongdouble("start_spin_polar_angle", 0.5);
	start_spin_phase = pi*getlongdouble("start_spin_phase", 0.0);
	
	spinEplus.x = sinl(start_spin_polar_angle)*cosl(start_spin_phase);
	spinEplus.y = sinl(start_spin_polar_angle)*sinl(start_spin_phase);
	spinEplus.z = cosl(start_spin_polar_angle);
	spinEminus = spinEplus;
}

/** Updates the Exv effect for the particle. */
void particle::updateExv ( efield &elecfield )
{
	/*
	vector3 vxE; vector3 E;
	elecfield.getfield(E, this->position);
	this->vxEeffect = (crossproduct(E, this->velocity_vec) * this->vgamma)/ csquared;
	*/
//	void neutron_physics::Exveffect( const vector3 &position, const vector3 &velocity, const long double gamma, vector3 &vxEeffect )
	neutron_physics::Exveffect( this->position, this->velocity_vec, this->vgamma, elecfield, this->vxEeffect );
}

