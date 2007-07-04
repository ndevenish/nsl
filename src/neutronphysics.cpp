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

#include "neutronphysics.h"
#include "particle.h"
#include "vector3.h"
#include "electromagnetics.h" // for electric field for edm_calcs
using std::endl;
using std::cout;
using std::cerr;

void neutron_physics::spinvec_change( const long double &time, const long double &gyromag, const vector3 &spinvector, const vector3& mag_field, vector3 &dS)
{
	dS = time * gyromag * crossproduct(spinvector, mag_field);
}

static long double spin_vector( vector3 &spinvector, const long double gyromag, const vector3& mag_field, const long double time )
{
	// Let's twist again, like we did last summer
	vector3 oldspin = spinvector;
	long double oldxylength = sqrtl(spinvector.x*spinvector.x + spinvector.y*spinvector.y);
/*	
	// Calculate the change in spin (through a function now)
	//vector3 dS = time * gyromag * crossproduct(spinvector, mag_field);
	 vector3 dS;
	 neutron_physics::spinvec_change( time, gyromag, spinvector, mag_field, dS);
	// Now apply this to the spin vector
	spinvector += dS;
	// Scale it to ensure that it remains of constant length
	spinvector.scaleto(1.0);
	
*/	
	// NEW METHOD? Try to use exact formulae
#warning "Using accurate, slower method for rotation angle calculations
	vector3 normmag = mag_field;
	normmag.scaleto(1.0);
	long double theta = time*gyromag*mod(mag_field);
	vector3 newvector;
// Attempt to use the proper rotation formula
	newvector = spinvector * cos(theta);
	newvector += crossproduct(spinvector, normmag) * sin(theta);
	newvector += normmag * (spinvector * normmag) * (1. - cos(theta));
	// compare this with the dS vector
//	vector3 compare = newvector - spinvector;
	spinvector = newvector;
//	long double len = spinvector.mod();
	spinvector.scaleto(1.0);

	// Calculate the new length of the xy vectorshar
	long double newxylength = sqrtl(spinvector.x* spinvector.x + spinvector.y*spinvector.y);
	
	//calculate the planar angle shift from the dot product
	long double cosphase = (oldspin.x*spinvector.x + oldspin.y*spinvector.y) / (oldxylength*newxylength);
	
	if (cosphase > 1.)
		cerr << "Cosphase > 1 by : " << lddistance(cosphase, 1.) << endl;;//throw runtime_error("Cosphase > 1");
		
	long double phase_change = acos(cosphase); // Radians
		
	return phase_change;
}

// B is the base magnetic field, without vxE effects added
void neutron_physics::spin_calculation( particle &part, const vector3 &B, const long double time)
{
	// First calculate the two different magnetic fields
	vector3 BplusvxE, BminusvxE; // Tesla
	BplusvxE = B + part.vxEeffect; // Tesla
	BminusvxE = B - part.vxEeffect; // Tesla
	
	// Now spin it both ways 
	long double plusphase, minusphase;//, framediff;
	plusphase		= spin_vector(part.spinEplus,	part.gamma, BplusvxE,  time); // Radians
	minusphase		= spin_vector(part.spinEminus,	part.gamma, BminusvxE, time); // Radians
	//framediff = plusphase - minusphase; // Radians
	
	part.E_sum_phase += plusphase; // Radians
	part.E_minus_sum_phase += minusphase; // Radians
	
	return;
}

// Calculates the false-edm effect value
void neutron_physics::edmcalcs( particle &part, efield &elecfield )
{
	// Firstly calculate the frequency differences between the two spins (Radians)
	part.frequencydiff = atan2( part.spinEplus.x * part.spinEminus.y - part.spinEplus.y*part.spinEminus.x,
								part.spinEplus.x*part.spinEminus.x + part.spinEplus.y*part.spinEminus.y );
	part.frequencydiff /= part.flytime; // Radians per second
	
	// Grab the E field vertical charge
	vector3 E; // Volts per meter
	elecfield.getfield(E, vector3(0,0,0)); // Volts per meter
	const long double E_FIELD = E.z; // Volts per meter
	
	// Now calculate the EDM from this... The 2pi from the hbar removes the radian dependence
	part.fake_edm = part.frequencydiff * hbar/E_FIELD/4; // Meter Coulombs
	// And convert into friendlier units
	part.fake_edm *= 100/echarge; // e.cm
	part.fake_edm *= 1e26; // e.cm x10^(-26)
}

void neutron_physics::Exveffect( const vector3 &position, const vector3 &velocity,  const long double gamma, efield &elecfield, vector3 &vxEeffect )
{
	vector3 vxE; vector3 E;
	elecfield.getfield(E, position);
	vxEeffect = (crossproduct(E, velocity) * gamma)/ csquared;
}
