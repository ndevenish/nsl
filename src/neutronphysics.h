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


#ifndef NEUTRON_PHYSICS_H
#define NEUTRON_PHYSICS_H

class particle;
class vector3;
class efield;


/** Class to handle the physics for a neutron */
class neutron_physics {
private:
	neutron_physics();

public:
	/** Function to process spin calculations for a particle.
	* @param part	The particle to spin
	* @param basemagneticfield The base magnetic field - i.e. not including the Exv effect (this is cached inside the particle and so is applied later.
	* @param time	The time for which to rotate the particle around this field. */
	static void spin_calculation( particle &part, const vector3 &basemagneticfield, const long double time);

	/** Calculates false EDM values.*/
	static void edmcalcs( particle &part, efield& Ez );
	
};
#endif

