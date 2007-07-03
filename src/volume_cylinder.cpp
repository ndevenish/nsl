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


#include "edmexperiment.h"
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
	
	gravity = false;
		
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
	
	// Do we use gravity?
	// Find the edmexperment objecgt
	edmexperiment *exp = (edmexperiment*)findbytype("edmexperiment");
	if (!exp)
		throw runtime_error("Could not find edmexperiment object to determine gravity setting");
	if (exp->get("gravity", "off") == "on")
		gravity = true;
	else
		gravity = false;
	
	
	// Setup the boundaries
	boundaries.radius = radius;
	boundaries.height = height;
	boundaries.position = position;
	boundaries.position.z += height*0.5;
	boundaries.real = true;
}


int volume_cylinder::findintersections ( const vector3& rayposi, const vector3& direction, intercept *nextplace ) const
{
	
	/// If we are going gravity, skip this routine and go to an altogether seperate one.
	if (gravity)
		return findintersections_gravity( rayposi, direction, nextplace );
	
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
		/*
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
		 */
		
		int retint = sideintersections( raypos, direction, nextplace );
		intercepts += retint;
		nextplace += retint;
			
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

// Search for intersections, with gravity!
int volume_cylinder::findintersections_gravity ( const vector3& rayposi, const vector3& direction, intercept *nextplace ) const
{
	long double lowerheight, upperheight;
	lowerheight = 0;//position.z;
	upperheight = height;//position.z + height;
	// Note: The above are in the frame of reference of the cylinder, which we transform into a couple of lines below
		
	int intercepts = 0, return_intercepts = 0;
		
	vector3 raypos(rayposi.x-position.x, rayposi.y-position.y, rayposi.z-position.z);

	// Check the top cap for intersections and update the intercepts
	return_intercepts = topcap_intersections_gravity(raypos, direction, nextplace);
	intercepts += return_intercepts;
	nextplace += return_intercepts;
	
	// What about the bottom cap?
	return_intercepts = bottomcap_intersections_gravity(raypos, direction, nextplace);
	intercepts += return_intercepts;
	nextplace += return_intercepts;	
	
	// Now check the sides for an intercept
	return_intercepts = sideintersections(raypos, direction, nextplace);
	intercepts += return_intercepts;
	nextplace += return_intercepts;
	
	

	return intercepts;
	
}

int volume_cylinder::sideintersections ( const vector3& raypos, const vector3& direction, intercept* nextplace ) const
{
	int intercepts = 0;
	
	const long double lowerheight = 0;//position.z;
	const long double upperheight = height;//position.z + height;
			
	//Calculate the intersection times
	long double a, b, c, infactor;
	a = direction.x*direction.x + direction.y*direction.y;
	b = 2 * (raypos.x*direction.x + raypos.y*direction.y);
	c = raypos.x*raypos.x + raypos.y*raypos.y - radius*radius;
	infactor = b*b - 4.*a*c;
	
	// If we have no intersections, return with none
	if (infactor < 0)
		return 0;
	
	infactor = sqrtl(infactor);
	
	// We have intersections!
	long double t1, t2;
	t1 = (-b - infactor) / (2*a);
	t2 = (-b + infactor) / (2*a);
	
	// Now check each of these times
	if (t1 > 0)
	{
		long double z;
		// calculate the z point of this intersection
		if (gravity)
			z = raypos.z  + t1*direction.z - 0.5*g*t1*t1;
		else
			z = raypos.z + t1*direction.z;
		
		// Is this z within the cylinder ends?
		if ((z > lowerheight) && (z < upperheight))
		{
			nextplace->time = t1;

			nextplace->normal = -1. * (raypos+t1*direction);
			nextplace->normal.z = 0;
			// This will always be an entry, whether inside or out (though inside t1 will
			// always be less than 0)
			nextplace->type = icept_entry;
						
			nextplace->collideobject = (container*)this;
									
			nextplace++;
			intercepts++;
		}
	} // End of t1
	
	if (t2 > 0)
	{
		long double z;
		// calculate the z point of this intersection
		if (gravity)
			z = raypos.z  + t2*direction.z - 0.5*g*t2*t2;
		else
			z = raypos.z + t2*direction.z;
		// Is this z within the cylinder ends?
		if ((z > lowerheight) && (z < upperheight))
		{
			nextplace->time = t2;
			
			nextplace->normal = -1. * (raypos+t2*direction);
			nextplace->normal.z = 0;
			// This will always be an exit.
			nextplace->type = icept_exit;
			
			nextplace->collideobject = (container*)this;
			
			nextplace++;
			intercepts++;
		}		
	}
	
	return intercepts;
}

/*
int volume_cylinder::capintersections_gravity ( long double capheight, const vector3& raypos, const vector3& direction, intercept *nextplace) const
{
	int intercepts = 0;

	long double infactor = direction.z*direction.z - 2*g*capheight + 2*g*raypos.z;
	
	// Does it hit the cap? (Don't count touches) if not, return nothing
	if (infactor < 0)
		return 0;

	// Change infactor to a more useful form now
	infactor = sqrtl(infactor);
	
	// We do touch, so calculate the times for these intersections
	long double t1, t2;
	t1 = (direction.z - infactor) / g;
	t2 = (direction.z + infactor) / g;
	
	// Check if each of these times are valid
	// Is t1 valid? If so, process it.
	if (t1 > 0)
	{
		// Calculate the xy positon at this time
		long double x = raypos.x + t1*direction.x;
		long double y = raypos.y + t1*direction.y;
		
		// Is it inside the radius?
		if (x*x + y*y < radius*radius)
		{
			// We have an intersection! but is it an entry or exit?
			nextplace->time = t1;
			nextplace->collideobject = (container*)this;
			nextplace->normal.x = nextplace->normal.y = 0.0;
			if (raypos.z < capheight)
				nextplace->normal.z = -1;
			else
				nextplace->normal.z = 1;
			
			// Is this always correct? i.e. first intersection with cap exit, second entry? I think so.
			nextplace->type = icept_exit;
			
			nextplace++;
			intercepts++;
		}
	}
	
	if (t2 > 0)
	{
		// Calculate the xy postion at this time
		long double x = raypos.x + t2*direction.x;
		long double y = raypos.y + t2*direction.y;
		// Is it inside the radius?
		if (x*x + y*y < radius*radius)
		{
			// We have an intersection! but is it an entry or exit?
			nextplace->time = t2;
			nextplace->collideobject = (container*)this;
			nextplace->normal.x = nextplace->normal.y = 0.0;
			if (raypos.z < capheight)
				nextplace->normal.z = -1;
			else
				nextplace->normal.z = 1;
			
			// Is this always correct? i.e. first intersection with cap exit, second entry? I think so.
			nextplace->type = icept_entry;
			
			nextplace++;
			intercepts++;
		}
	}
	
	return intercepts;
}	
*/
int volume_cylinder::topcap_intersections_gravity (const vector3& raypos, const vector3& direction, intercept *nextplace) const
{
	// Remember: This is in the cylinders reference frame
	long double capheight = height;
	
	int intercepts = 0;
	
	long double infactor = direction.z*direction.z - 2*g*capheight + 2*g*raypos.z;
	
	// Does it hit the cap? (Don't count touches) if not, return nothing
	if (infactor < 0)
		return 0;
	
	// Change infactor to a more useful form now
	infactor = sqrtl(infactor);
	
	// We do touch, so calculate the times for these intersections
	long double t1, t2;
	t1 = (direction.z - infactor) / g;
	t2 = (direction.z + infactor) / g;
	
	// Check if each of these times are valid
	// Is t1 valid? If so, process it.
	if (t1 > 0)
	{
		// Calculate the xy positon at this time
		long double x = raypos.x + t1*direction.x;
		long double y = raypos.y + t1*direction.y;
		
		// Is it inside the radius?
		if (x*x + y*y < radius*radius)
		{
			// We have an intersection! but is it an entry or exit?
			nextplace->time = t1;
			nextplace->collideobject = (container*)this;
			nextplace->normal.x = nextplace->normal.y = 0.0;

			//			if (raypos.z < capheight)
//				nextplace->normal.z = -1;
//			else
//				nextplace->normal.z = 1;
			
			// Is this always correct? i.e. first intersection with cap exit, second entry?
			// Yes! At least, for the top cap case.
			nextplace->type = icept_exit;
			// A top cap exit always has a negative normal
			nextplace->normal.z = -1.;
			
			nextplace++;
			intercepts++;
		}
	}
	
	if (t2 > 0)
	{
		// Calculate the xy postion at this time
		long double x = raypos.x + t2*direction.x;
		long double y = raypos.y + t2*direction.y;
		// Is it inside the radius?
		if (x*x + y*y < radius*radius)
		{
			// We have an intersection! but is it an entry or exit?
			nextplace->time = t2;
			nextplace->collideobject = (container*)this;
			nextplace->normal.x = nextplace->normal.y = 0.0;
//			if (raypos.z < capheight)
//				nextplace->normal.z = 1;
//			else
//				nextplace->normal.z = -1;
			
			// Is this always correct? i.e. first intersection with cap exit, second entry?
			// Yes! At least it is for the top cap case
			nextplace->type = icept_entry;
			// a top cap entry always has a positive normal
			nextplace->normal.z = 1.;
			
			nextplace++;
			intercepts++;
		}
	}
	
	return intercepts;
	
}

int volume_cylinder::bottomcap_intersections_gravity ( const vector3& raypos, const vector3& direction, intercept *nextplace) const
{
	// Remember: This is in the cylinders reference frame
	long double capheight = 0.;
	
	int intercepts = 0;
	
	long double infactor = direction.z*direction.z - 2*g*capheight + 2*g*raypos.z;
	
	// Does it hit the cap? (Don't count touches) if not, return nothing
	if (infactor < 0)
		return 0;
	
	// Change infactor to a more useful form now
	infactor = sqrtl(infactor);
	
	// We do touch, so calculate the times for these intersections
	long double t1, t2;
	t1 = (direction.z - infactor) / g;
	t2 = (direction.z + infactor) / g;
	
	// Check if each of these times are valid
	// Is t1 valid? If so, process it.
	if (t1 > 0)
	{
		// Calculate the xy positon at this time
		long double x = raypos.x + t1*direction.x;
		long double y = raypos.y + t1*direction.y;
		
		// Is it inside the radius?
		if (x*x + y*y < radius*radius)
		{
			// We have an intersection! but is it an entry or exit?
			nextplace->time = t1;
			nextplace->collideobject = (container*)this;
			nextplace->normal.x = nextplace->normal.y = 0.0;
//			if (raypos.z < capheight)
//				nextplace->normal.z = -1;
//			else
//				nextplace->normal.z = 1;
			
			// For the bottom cap, the first is always entry, the second exit - this is opposite
			// the behaviour of the top cap
			nextplace->type = icept_entry;
			// bottom cap entry always negative
			nextplace->normal.z = -1.;
			
			nextplace++;
			intercepts++;
		}
	}
	
	if (t2 > 0)
	{
		// Calculate the xy postion at this time
		long double x = raypos.x + t2*direction.x;
		long double y = raypos.y + t2*direction.y;
		// Is it inside the radius?
		if (x*x + y*y < radius*radius)
		{
			// We have an intersection! but is it an entry or exit?
			nextplace->time = t2;
			nextplace->collideobject = (container*)this;
			nextplace->normal.x = nextplace->normal.y = 0.0;
			if (raypos.z < capheight)
				nextplace->normal.z = -1;
			else
				nextplace->normal.z = 1;
			
			
			// For the bottom cap, the first is always entry, the second exit - this is opposite
			// the behaviour of the top cap
			nextplace->type = icept_exit;
			// Bottom cap exit always positive!
			nextplace->normal.z = 1.;
			
			nextplace++;
			intercepts++;
		}
	}
	
	return intercepts;
	
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
