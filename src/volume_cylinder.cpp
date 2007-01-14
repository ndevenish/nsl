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


#include "nslobject.h"
#include "nslobjectfactory.h"
#include "container.h"

#include "physics.h"

using std::string;
using std::runtime_error;

volume_cylinder::volume_cylinder()
{
//	initvals();
	icept_entry = interception_entry;
	icept_exit = interception_exit;
	
	position.x = position.y = position.z = 0.0;
	radius = 0.0;
	height = 0.0;
	volume_type = volume_none;
	reflection = reflection_diffuse;
	
	objecttype = "volume_cylinder";
	types.push_back(objecttype);
}

void volume_cylinder::readsettings(void)
{
	position.x = position.y = position.z = 0.0;
	radius = 0.0;
	height = 0.0;
	volume_type = volume_none;
	reflection = reflection_diffuse;

	// The requirements
	require("radius");
	require("height");
	
	//if (isset("x"))
	position.x = getlongdouble("x", 0.0);
	position.y = getlongdouble("y", 0.0);
	position.z = getlongdouble("z", 0.0);
	
	string voltype = get("type", "none");
	if (voltype == "none")
		volume_type = volume_none;
	else if (voltype == "negative")
	{
		volume_type = volume_negative;
		icept_entry = interception_entry;
		icept_exit = interception_exit;
	}
	else if (voltype == "positive")
	{
		volume_type = volume_positive;
		// Reverse the interception parameters
		icept_entry = interception_exit;
		icept_exit = interception_entry;
	}
	else
		throw runtime_error("Unrecognised volume type");
	
	radius = getlongdouble("radius", 0.0);
	height = getlongdouble("height", 0.0);
	
	// Now set up the starting positions - edge and center
	positionlist["edge"] = position + vector3(0.0, -radius, height/2.0);
	positionlist["center"] = position + vector3(0.0, 0.0, (height/2.0));
	
	string reflectiontype;
	reflectiontype = get("reflection", "diffuse");
	if (reflectiontype == "diffuse")
		reflection = reflection_diffuse;
	else if (reflectiontype == "specular")
		reflection = reflection_specular;
	else
		throw runtime_error("Reflection type not recognised");
	
	// Setup the boundaries
	boundaries.radius = radius;
	boundaries.height = height;
	boundaries.position = position;
	boundaries.position.z += height*0.5;
	boundaries.real = true;
}


int volume_cylinder::findintersections ( const vector3& rayposi, const vector3& direction, intercept *nextplace ) const
{
	// count of the intercepts so far
	int intercepts = 0;//, invalids = 0;

	long double lowerheight, upperheight;
	lowerheight = 0;//position.z;
	upperheight = height;//position.z + height;

	vector3 raypos(rayposi.x-position.x, rayposi.y-position.y, rayposi.z-position.z);

	// Only do this part if we have a horizontal plane velocity, otherwise it will 
	// never hit the cylinder sides and we will have a problem
	if (!((direction.x == 0) && (direction.y == 0)))
	{
		// Firstly calculate interception with an infinite cylinder
		long double a, b, bsq, c, fac;
		//long double icept, plusminus, plus, minus;

		// Calculate the intersection constants
		a = direction.x*direction.x + direction.y * direction.y;
		b = 2 * (raypos.x*direction.x + raypos.y*direction.y);
		bsq = b*b;
		c = raypos.x*raypos.x + raypos.y*raypos.y - radius*radius;
		fac = 4.*a*c;

		// If no intersection
		if (bsq <= fac)
			return 0;

		// Now calculate the two intercepts, and throw them out if negative
		long double i1, i2, plusminus;
		long double ray_z_icept;
		plusminus = sqrtl(bsq - fac);
		
		i1 = ((-b) + plusminus) / (2.*a);
		// If -b + sqrt() is negative, both intercepts are behind us, so no intercept
		if (i1 < 0)
			return 0;
		else { // i1 is positive, so test it is in height range and add it
			// Calculate the z position this equates to
			ray_z_icept = raypos.z + i1*direction.z;

			// Otherwise, check to see if it is in height ranges. If so, add it!
			if ((ray_z_icept > lowerheight) && (ray_z_icept < upperheight))
			{
				nextplace->time = i1;
				nextplace->type = icept_exit;
				nextplace->normal.z = 0.0;
				nextplace->normal.x = position.x - (rayposi.x+ i1*direction.x);
				nextplace->normal.y = position.y - (rayposi.y+ i1*direction.y);
				nextplace->collideobject = (container*)this;
				
				nextplace++;
				intercepts++;
			}
		}

		// Now check The closer case
		i2 = ((-b) - plusminus) / (2.*a);
		if (i2 > 0)
		{
			//Calculate this z intercept
			ray_z_icept = raypos.z + i2*direction.z;
			
			// Check to see if it is in height ranges. If so, add it!
			if ((ray_z_icept > lowerheight) && (ray_z_icept < upperheight))
			{
				nextplace->time = i2;
				nextplace->type = icept_entry;
				nextplace->normal.z = 0.0;
				nextplace->normal.x = position.x - (rayposi.x+ i2*direction.x);
				nextplace->normal.y = position.y - (rayposi.y+ i2*direction.y);
				nextplace->collideobject = (container*)this;
				
				//nextplace->location.
				nextplace++;
				intercepts++;
			}
		} else {
			;//invalids++;
		}

		// Check, if we have two valid intersections there can be no more
		if (intercepts == 2)
			return 2;
	}

	// Test to see if there is actually the chance of a cap intersection
	// i.e. that dz != 0
	if (direction.z == 0.0)
		return intercepts;

	// If at this point, we know that there is an end-cap intersection - test each end cap.
	long double i3, i4, xsq, ysq;
	// Find the plane-intersection for the Upper Cap
	i3 = (upperheight - raypos.z) / direction.z;
	// find if this hit is positive and within the cap
	if (i3 > 0)
	{
		// calculate x and y squared
		xsq = raypos.x + i3*direction.x;
		xsq *= xsq;
		ysq = raypos.y + i3*direction.y;
		ysq *= ysq;
		if ( xsq + ysq < (radius*radius))
		{
			// A Hit on the upper cap!
			nextplace->time = i3;
			if (raypos.z > upperheight)
				nextplace->type = icept_entry;
			else
				nextplace->type = icept_exit;
			nextplace->normal.x = nextplace->normal.y = 0.0;
			nextplace->normal.z = -1;
			nextplace->collideobject = (container*)this;
			
			nextplace++;
			// return if this makes two intercepts
			if ((++intercepts) == 2)
				return 2;
		}
	}

	// Now test the last possible cap
	i4 = (lowerheight - raypos.z) / direction.z;
	// find if this hit is positive and within the cap
	if (i4 > 0)
	{
		// calculate x and y squared of the intersection, and test it is
		// within the radius
		xsq = raypos.x + i4*direction.x;
		xsq *= xsq;
		ysq = raypos.y + i4*direction.y;
		ysq *= ysq;
		if ( xsq + ysq < (radius*radius))
		{
			// A Hit!
			nextplace->time = i4;
			if (raypos.z > lowerheight)
				nextplace->type = icept_exit;
			else
				nextplace->type = icept_entry;
			nextplace->normal.x = nextplace->normal.y = 0.0;
			nextplace->normal.z = 1;
			nextplace->collideobject = (container*)this;

			nextplace++;

			// If we get here, we MUST have two intercepts.
			// check this then return
//			assert((++intercepts) == 2);

			return (++intercepts);
		}
	}

//	if (intercepts)
		return intercepts;
	
	throw runtime_error("No intercepts were found, but weren't detected early");

	// Otherwise, no intercepts :(
	return 0;
}

int volume_cylinder::isinside(const vector3 &pos) const
{
	int count = 0;
	vector3 adjpos;
	long double xysq;

	adjpos = (position*-1.0)+ pos;
	xysq = adjpos.x*adjpos.x + adjpos.y*adjpos.y;
	if ((radius*radius)-xysq > - 1e-15)
		if (height-adjpos.z > -1e-15)
			if (adjpos.z > -1e-15)
				count++;

	if (volume_type == volume_positive)
		count *= -1;
	return count + container::isinside(pos);

}
