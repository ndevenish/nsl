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
#include <assert.h>
#include <iostream>

#include "nslobject.h"
#include "nslobjectfactory.h"
#include "container.h"

#include "errors.h"

#include "physics.h"

#include "random.h"

#include "boost/foreach.hpp"
#include "tools.h"

using std::runtime_error;
using std::cout;
using std::endl;
using nsl::rand_uniform;

container::container()
{
	reflection = reflection_specular;
	
	objecttype = "container";
	types.push_back(objecttype);
}

// Get a named position in the volume
vector3 container::getposition(std::string pos)
{
	if (!initialised)
		prepareobject();

	vector3 retval;
	if (positionlist.find(pos) != positionlist.end())
		return positionlist[pos];
	else
	{
		Warning("Attempting to access unidentified volume position");
		return vector3(0.0, 0.0, 0.0);
	}
}

bool container::prepareobject( void ) 
{
	// Go through our children, and add any containers to our container list - this
	// will mean come runtime we will not need to verify they are of correct type.
	std::vector<nslobject*>::iterator sublist;
	for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
		if ((*sublist)->isoftype("container"))
			subcontainers.push_back((container*)(*sublist));

	return true;
}

const intercept &container::cast ( const vector3 &position, const vector3 &direction, int in_volumes ) const
{
	//logger << __FILE__ << ":" << __LINE__ << ": " << position << endl;
//	trace ( position );

	// Firstly fill out the intercept list
	int intercepts;
	intercept interceptlist[MAX_INTERCEPTS];
	intercepts = castinternal( position, direction, interceptlist );

	// If intercepts is equal to one, just pass it back without anything further -
	// it is the only intercept
	if (intercepts == 1)
	{
		// FUDGE
		//interceptlist[0].time *= fudge;
		//interceptlist[0].normal.scaleto(1.0);
		return interceptlist[0];
	}

	// Make sure we have collected intercepts
	if (intercepts == 0)
	{
		logger << "Zero intercepts detected:" << endl;
		logger << " Pos:   " << position << endl;
		logger << " Direc: " << direction << endl;
		throw runtime_error("Zero intercepts detected");
	}
	assert(intercepts < MAX_INTERCEPTS); // this would mean we have violated internal memory

	// Now sort the list of intercepts
	intercept *sortinglist[MAX_INTERCEPTS], *temp;
	int swaps = 0;
	// Firstly, read the list into a pointer array
	for (int i = 0; i < intercepts; i++)
	{
		//interceptlist[i].time *= fudge;
		sortinglist[i] = &interceptlist[i];
	}
	// Now sort this
	do {
		swaps = 0;
		for (int j = 0; j < (intercepts-1); j++)
			if (sortinglist[j]->time > sortinglist[j+1]->time)
			{
				temp = sortinglist[j];
				sortinglist[j] = sortinglist[j+1];
				sortinglist[j+1] = temp;
				swaps++;
			}
	} while (swaps);

	// We now have a sorted list of interceptions. Find out if any of these exit
	// all known volumes, assuming that we are starting inside of one.
	//int in_volumes = 1;

	in_volumes = isinside(position);
//	if (in_volumes > 1)
//		cout << "inv>1" << endl;
	if (in_volumes == 0)
		cout << "In Zero Volumes...." << endl;

	for (int i = 0; i < intercepts; i++)
	{
		switch (sortinglist[i]->type)
		{
		case interception_entry:
			in_volumes++;
			break;
		case interception_exit:
			in_volumes--;
			break;
		default:
			throw runtime_error("Intercept is neither entry nor exit");
		}
		
		// If we have exited all volumes
		if (in_volumes <= 0)
		{
			// Is this the last intercept?
			if (i == (intercepts - 1))
			{
				return *(sortinglist[i]);
			}

			// Check the next to see if it a 'very close' entry:
			if (sortinglist[i+1]->type == interception_entry)
				if (fabsl(sortinglist[i]->time - sortinglist[i+1]->time) < 1e-5)
					// If it is then continue with the next loop
					continue;
			
			// If we are here, then the next one is not a close entry, so we have
			// found the interception!
			return *(sortinglist[i]);
		}
	}


	throw runtime_error("Particle appears to be outside all known volumes");

	// Return the last intercept in this case.....

	return *(sortinglist[intercepts-1]);
}

int container::castinternal ( const vector3& position, const vector3& direction, intercept *nextintercept ) const
{
	int intercepts = 0;
	intercepts += findintersections(position, direction, nextintercept);

	// Now iterate through each subobject
//	std::vector<container*>::iterator sublist;
//	for (sublist = subcontainers.begin(); sublist != subcontainers.end(); sublist++)
//		intercepts += (*sublist)->castinternal(position, direction, nextintercept+intercepts);
	BOOST_FOREACH(const container *cont, subcontainers)
		intercepts += cont->castinternal(position, direction, nextintercept+intercepts);
	
	return intercepts;
}

int container::isinside(const vector3 &pos) const
{
	int count = 0;
	std::vector<container*>::iterator sublist;
// 	for (sublist = subcontainers.begin(); sublist != subcontainers.end(); sublist++)
// 		count += (*sublist)->isinside(pos);
	BOOST_FOREACH(const container *cont, subcontainers)
		count += cont->isinside(pos);
	return count;
}


void container::reflect ( vector3& velocity_vec, const vector3& normal, const long double& velocity ) const
{
	// Is it specular reflection?
	if (reflection == reflection_specular)
	{
		velocity_vec =  velocity_vec - ((normal * 2.)*(normal * velocity_vec));
		return;
	}
	// If not, is it diffuse?
	else if(reflection == reflection_diffuse)
	{
		/*
		// Generate a random z and theta
		velocity_vec.z = (rand_uniform()-0.5)*2.;
		
		long double phi = rand_uniform()*pi*2;
		// Calculate the flat-plane (z=0.) radius of this point
		long double planarr = cosl(asinl( velocity_vec.z ));
		
		velocity_vec.x = planarr * cosl(phi);
		velocity_vec.y = planarr * sinl(phi);
		
		
		// Ensure it faces away from the normal
		if ((velocity_vec * normal) < 0.)
			velocity_vec *= -1.0;
		
		// Scale the velocity to the proper one
		
		// Output the magnitude of the velocity vector
		//cout << velocity_vec.mod() << endl;
		
		velocity_vec *= velocity;
		return;*/
		
		// Now try the new method of diffuse reflection (i.e. the proper way)
		
		// Generate the Random Terms
		long double costheta = sqrt(rand_uniform());
		long double sintheta = sqrt(1. - costheta*costheta);
		
		long double phi = 2*pi*rand_uniform();
		long double sincos = sintheta * cos(phi);
		
		// Are we a wall or ceiling reflection? If we are a wall reflection, then we need to rotate,
		// otherwise we don't need to bother. In the future, this could be extended to a full 3d rotation
		// but that is overkill for the time being, as only the two types of surfaces exist
		if (normal.x*normal.x+normal.y*normal.y > 1e-9)
		{
			// A Wall reflection!

			// Now, generate the new velocities,and apply a rotation also
			// Note: The rotation cos and sin factors can be pulled right out of the
			// normal vector
			velocity_vec.x = velocity * (costheta * normal.x	-	sincos * normal.y);
			velocity_vec.y = velocity * (sincos	* normal.x		+	costheta * normal.y);
			velocity_vec.z = velocity * (sintheta * sin(phi));
			
		} else {
			// A Ceiling reflection!
			velocity_vec.x = velocity * sincos;
			velocity_vec.y = velocity * (sintheta * sin(phi));
			velocity_vec.z = velocity * costheta;
			
			//This will do an upwards spread, invert it if we have hit the top ceiling
			if (normal.z < 0)
				velocity_vec.z *= -1.;
		}
		
	}
	else
		throw runtime_error("Unrecognised reflection type requested for reflection");
}

cylbounds container::getcylinder( void ) const
{
	cylbounds borders = boundaries;
	
	/** Update our cylinder boundaries */
	BOOST_FOREACH( container* sub, subcontainers)
	{
		cylbounds tborders = sub->getcylinder();
		// Only use real borders
		if (tborders.real)
			borders += tborders;
	}

	// Now, this should be the outer boundaries!
	return borders;
}

cylbounds cylbounds::operator+=(const cylbounds& rt)
{
	// check for reality - don't merge with unreal sets
	if (!real)
		if (rt.real)
		{
			radius = rt.radius;
			height = rt.height;
			real = true;
			position = rt.position;
			return *this;
		}
	else if (!rt.real)
		return *this;
	
	// Calculate the average position
	
	//Firstly calculate the halfway vector between them, so they are equidistant
	vector3 diffvec = (rt.position - position) * 0.5;
	
	// Take the largest radial distance to extremum as the radius
	if ( radius > rt.radius )
		radius = modxy(diffvec) + radius;
	else
		radius = modxy(diffvec) + rt.radius;
	
	
	// Now, calculate height and z positions
	long double min1, max1, min2, max2;
	min1 = position.z - height/2.; max1 = min1 + height;
	min2 = rt.position.z - rt.height/2.; max2 = min2 + rt.height;

	// Change the 1 vars to be the bounds
	if (min1 > min2)
		min1 = min2;
	if (max1 < max2)
		max1 = max2;
	// Calculate and set the height
	height = max1 - min1;
	
	// Now set the vertical position
	diffvec.z = min1 + height*0.5;

	// Now set he centerpos as our position
	position = diffvec;
	
	return *this;
}
