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

	b0.x = getlongdouble("b0x", 0.0);
	b0.y = getlongdouble("b0y", 0.0);
	b0.z = getlongdouble("b0z", 0.0);

	return true;
}

bool efield::prepareobject()
{
	forcefield::prepareobject();

	e0.x = getlongdouble("e0x", 0.0);
	e0.y = getlongdouble("e0y", 0.0);
	e0.z = getlongdouble("e0z", 0.0);

	return true;
}

linear_zgradient::linear_zgradient()
{
	objecttype = "linear_zgradient";
	types.push_back(objecttype);
}

bool linear_zgradient::prepareobject()
{
	d_dz = getlongdouble("d_dz", 0.0);
	
	return true;
}

void linear_zgradient::field(vector3& field, const vector3 &position)
{
	//vector3 thefield(0,0,0);

	field.x += -position.x * (d_dz / 2.0);
	field.y += -position.y * (d_dz / 2.0);
	field.z += position.z * d_dz;
}