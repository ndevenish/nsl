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


static long double spin_vector( vector3 &spinvector, const long double gyromag, const vector3& mag_field, const long double time )
{
	// Let's twist again, like we did last summer
	vector3 oldspin = spinvector;
	long double oldxylength = sqrtl(spinvector.x*spinvector.x + spinvector.y*spinvector.y);
	
	// Calculate the change in spin
	vector3 dS = time * gyromag * crossproduct(spinvector, mag_field);
	
	// Now apply this to the spin vector
	spinvector += dS;
	
	// Scale it to ensure that it remains of constant length
	/*cout.precision(1000);
	cout << endl << spinvector.x << endl << spinvector.y << endl << spinvector.z << endl;*/
	spinvector.scaleto(1.0);
	
	// Calculate the new length of the xy vectorshar
	long double newxylength = sqrtl(spinvector.x* spinvector.x + spinvector.y*spinvector.y);
	
	//calculate the planar angle shift from the dot product	
	long double cosphase = (oldspin.x*spinvector.x + oldspin.y*spinvector.y) / (oldxylength*newxylength);
	
	if (cosphase > 1.)
		cerr << "Cosphase > 1 by : " << lddistance(cosphase, 1.) << endl;;//throw runtime_error("Cosphase > 1");
		
	long double phase_change = acos(cosphase);
		
	return phase_change;
}

// B is the base magnetic field, without vxE effects added
void neutron_physics::spin_calculation( particle &part, const vector3 &B, const long double time)
{
	// First calculate the two different magnetic fields
	vector3 BplusvxE, BminusvxE;
	BplusvxE = B + part.vxEeffect;
	BminusvxE = B - part.vxEeffect;
	
	// Now spin it both ways 
	long double plusphase, minusphase, framediff;
	plusphase		= spin_vector(part.spinEplus,	part.gamma, BplusvxE,  time);
	minusphase		= spin_vector(part.spinEminus,	part.gamma, BminusvxE, time);
	framediff = plusphase - minusphase;
	
	part.E_sum_phase += plusphase;
	part.E_minus_sum_phase += minusphase;
	
	return;
}

// Calculates the false-edm effect value
void neutron_physics::edmcalcs( particle &part, efield &elecfield )
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
	elecfield.getfield(E, vector3(0,0,0));
	const long double E_FIELD = E.z;
	
	// Now calculate the EDM from this
	part.fake_edm = part.frequencydiff * hbar/E_FIELD/echarge*100*1e26/4;
}
