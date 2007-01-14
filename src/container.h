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

#ifndef CONTAINER_H
#define CONTAINER_H

#include <string>
#include <map>
#include "nslobject.h"
#include "nslobjectfactory.h"

#include "physics.h"

// Predeclarations
class container;

//const long double fudge = 1.0; //0.9999;

// *****************************************************************
// Structures and enums

enum itype {
		interception_none,
		interception_entry,
		interception_exit
};

struct intercept {
	long double time; ///< The time to the interception
	itype type; ///< The interception type. This should always be returned as interception_exit
	vector3 location; ///< The spatial position of the intercept
	vector3 normal; ///< The surface normal at the point of interception

	const container *collideobject; ///< Pointer to the object with which the ray collides

	intercept() : time(0.0), type(interception_none), location(0,0,0), normal(0,0,0), collideobject(0) {}
};

struct cylbounds {
	friend class container;
	
	cylbounds() : radius(0.0), height(0.0), real(false) {}
	long double radius;
	long double height;
	vector3 position;
	bool real;
	
	cylbounds operator+=(const cylbounds& rt);
};
	
// ******************************************************************
// Classes

/** Ties together multiple volumes as a single large volume. */
class container : public nslobject {
	static const int MAX_INTERCEPTS = 32;
	friend class edmexperiment;

	bool prepareobject();

	//void readsettings();
	
	// List of intercepts
	//intercept interceptlist[MAX_INTERCEPTS];

	// Internal version of cast - this produces a list of intersections for the current
	// and all subobjects. Called by cast. 
	int castinternal ( const vector3& position, const vector3& direction, intercept *nextintercept) const;

protected:
	std::map<std::string, vector3>	positionlist;
	std::vector<container *> subcontainers;

	// Virtual function that we pass an intersection list, and it fills it out with any
	// intersections that occur, and returns the number of intersections.
	virtual int findintersections ( const vector3& position, const vector3& direction, intercept *nextintercept ) const { return 0; };

	enum {
		reflection_diffuse,
		reflection_specular
	} reflection;
	
	/** The boundaries of this object, and all of it's subobjects */
	cylbounds boundaries;
	
	
public:
	container();

	// Get a named position in the volume - Needs to be non-const as may need to
	// initialise
	vector3 getposition(std::string);

	/** Test to see if a position is inside the current object.
	* Virtual as this version only calls subobjects, and derived
	* classes must explicitly pass execution to this function to
	* inherit the same behaviour.
	* @param position The position of the particle
	* @return The number of objects that the particle is inside of (counts current+subobjects) */
	virtual int isinside( const vector3 &position ) const;

	/** Get's the cylindrical boundaries of the container.
		The 'volume' is the size and position of a cylinder that would encompass all container
		objects in the scene. The position is taken to be the center of the cylinder, i.e.
		the caps are at position.z +/-  height/2 */
	cylbounds getcylinder ( void ) const;
	
	/** Casts a ray and returns the intersection point.
	* Cast a ray from a position, and returns (through manipulation of the supplied
	* result and surface parameters) a reference to an intercept object with the:
	* Distance to the collision, collision point, normal at that point, and a pointer
	* to the object with which it is colliding.
	*
	* IMPORTANT NOTE: The normal in the intercept structure returned will most likely NOT
	* be normalised. This is done in order to save calculation inside of the casting process. For
	* similar reasons the position is also not filled out, and these must be calculated manually.
	* @param position The position of the particle
	* @param direction The direction the particle is facing i.e. the velocity
	* @param unused Currently unused.
	* @return an intercept object with the interception parameters. */
	const intercept &cast ( const vector3 &position, const vector3 &direction, int unused = 0) const;

	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new container; }
	};
};

/** Represents an upright, finite, cylindrical volume. */
class volume_cylinder : public container {
//	bool prepareobject();

	void readsettings( void );
	
	enum {
		volume_none,
		volume_negative,
		volume_positive
	} volume_type;
	long double radius;
	long double height;
	vector3 position;

	itype icept_entry;
	itype icept_exit;
	
	int findintersections ( const vector3& position, const vector3& direction, intercept *nextintercept ) const;

public:
	volume_cylinder();

	int isinside(const vector3 &position) const;

	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new volume_cylinder; }
	};
};

#endif
