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


#include "errors.h"

#include "nslobject.h"
#include "nslobjectfactory.h"

#include "electromagnetics.h"

using std::runtime_error;

forcefield::forcefield()
{
	objecttype = "forcefield";
	types.push_back(objecttype);
	
	// Blank the sub field array
	for (int i = 0; i < max_fieldmods; i++)
		sub_fields[i] = 0;
	sub_fieldcount = 0;
}

bool forcefield::prepareobject()
{
	
	// Go through each of our subobjects, and if they are a field modifier
	// add them to our 'call list'
	std::vector<nslobject*>::iterator sublist;
	for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
		if ((*sublist)->isoftype("forcefield"))
		{
			if (sub_fieldcount == max_fieldmods)
				throw runtime_error("Exceeded max_fieldmods for field modifier subobjects");
			sub_fields[sub_fieldcount++] = (forcefield*)(*sublist);
		}

	return true;
}
void forcefield::getfield(vector3 &ffield, const vector3 &position)
{
	// Apply our field modifiers
	field(ffield, position);

	// Apply each of our field subobjects to the field
	for (int i = 0; i < sub_fieldcount; i++)
		sub_fields[i]->getfield(ffield, position);
}

void forcefield::getfieldgradient( vector3& ffield, const vector3 &position)
{
	// apply our field gradient
	fieldgradient(ffield, position);
	
	// now apply each of our subobjects gradients
	for (int i = 0; i < sub_fieldcount; i++)
		sub_fields[i]->getfieldgradient(ffield, position);
}

bfield::bfield()
{
	objecttype = "bfield";
	types.push_back(objecttype);
}

efield::efield()
{
	objecttype = "efield";
	types.push_back(objecttype);
}

bool bfield::prepareobject()
{
	// We need to prepare the underlying forcefield - it tracks what subobjects we 
	// need to call.
	forcefield::prepareobject();

	return true;
}

void bfield::readsettings( void )
{
	
	b0.x = getlongdouble("b0x", 0.0);
	b0.y = getlongdouble("b0y", 0.0);
	b0.z = getlongdouble("b0z", 0.0);	
}

bool efield::prepareobject()
{
	forcefield::prepareobject();

	return true;
}

void efield::readsettings()
{
	e0.x = getlongdouble("e0x", 0.0);
	e0.y = getlongdouble("e0y", 0.0);
	e0.z = getlongdouble("e0z", 0.0);	
}

linear_zgradient::linear_zgradient()
{
	objecttype = "linear_zgradient";
	types.push_back(objecttype);
}

void linear_zgradient::readsettings( void )
{
	d_dz = getlongdouble("d_dz", 0.0);
	//myposition.x = getlongdouble("x", 0.0);
//	myposition.y = getlongdouble("y", 0.0);
	zposition = getlongdouble("z", 0.0);
}
void linear_zgradient::field(vector3& field, const vector3 &position)
{
	//vector3 thefield(0,0,0);

	field.x += -position.x * (d_dz / 2.0); // Tesla
	field.y += -position.y * (d_dz / 2.0); // Tesla
	field.z += (position.z - zposition) * d_dz;		   // Tesla
}

/////////////////////////////////////////////////////////////////
// Magnetic dipole

dipole_zmagnetic::dipole_zmagnetic()
{
	mz = 0; // amps . meters^2
	z = 0;
}

void dipole_zmagnetic::readsettings( void )
{
	require("mz");
	require("z");
	
	mz = getlongdouble("mz", 0.0); // Amps.meters^2
	mz *= dipole_mult; // *4*pi Amps.meters.Henrys
	
	z = getlongdouble("z", 0.0);
}

void dipole_zmagnetic::field( vector3 &field, const vector3 &position )
{
	// Calculate the radius and relative z positions
	long double relz = position.z - z;
	long double r = sqrtl(position.x*position.x + position.y*position.y + relz*relz);
	long double rto5 = powl(r, 5.);
		
	field.x += (mz / rto5) * ( 3 * position.x * relz			); // Tesla
	field.y += (mz / rto5) * ( 3 * position.y * relz			); // Tesla
	field.z += (mz / rto5) * ( 3 * relz		  * relz	- r*r	); // Tesla
}

void dipole_zmagnetic::fieldgradient( vector3 &field, const vector3 &position)
{
	//throw runtime_error("Dipole Vertical field gradient not yet implemented");
	
	// Calculate the radius and relative z positions
	long double relz = position.z - z; // Meters
	long double r = sqrtl(position.x*position.x + position.y*position.y + relz*relz); // Meters
	long double rto7 = powl(r, 7.); // Meters^7

	field.z += mz * ((3. * relz) / rto7 ) * (3. * r * r - 5. * relz * relz ); // Tesla
}
