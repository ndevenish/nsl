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

/* Header for electromagnetic functions and classes.

Classes forcefield, bfield, linear_zgradient and any others derived from forcefield
will iterate through each of their forcefield-derived children applying their modifiers
to the forcefield. */

#ifndef ELECTROMAGNETICS_H
#define ELECTROMAGNETICS_H

#include "nslobject.h"
#include "nslobjectfactory.h"

#include <vector>

#include "physics.h"

const int max_fieldmods = 8;

/** Object representing a generic force-field.*/
class forcefield : public nslobject {

	// A vector to store a list of subobjects that are field modifiers
	//vector<bfield *> sub_magnetics;
	int sub_fieldcount;
	forcefield *sub_fields[max_fieldmods];

protected:
	bool prepareobject();

public:
	forcefield();

	//void initvals( void );
	//void readsettings( void );
	
	/** Retrieves the strength of the field at a specified position.
	* This is returned as a vector. */
	virtual void field(vector3& field, const vector3& position) { return; }
	/** Retrieves the strength of the field gradient.*/
	virtual void fieldgradient(vector3& field, const vector3 &position) { return; }
	
	/** Retrieves the field from this object tree.
	* This function will iterate through all of it's children, asking them for
	* their field modifiers and will modify the passed vector by this. */
	void getfield( vector3& field, const vector3 &position );
	
	/** Retrieves the field gradient from the object tree.
	* This function will iterate through all of it's children and 
	* accumulate a value for the field gradient */
	void getfieldgradient( vector3& field, const vector3 &position);
	

	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new forcefield; }
	};
};

/** Magnetic field object representing a uniform magnetic field*/
class bfield : public forcefield {
	// Vectors for magnetic field strength and gradient
	vector3 b0;
	//vector3 dbd;

	bool prepareobject();
	void readsettings( void );

protected:
	void field(vector3& field, const vector3& position) { field += b0; }

public:
	bfield();

	//vector3 getfieldgradient(const vector3& position) { return dbd; };

	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new bfield; }
	};
};

/** Electric field object representing a uniform electric field*/
class efield : public forcefield {
	// Vectors for magnetic field strength and gradient
	vector3 e0;
	//vector3 dbd;

	bool prepareobject();
	void readsettings( void );

protected:
	void field(vector3& field, const vector3& position) { field += e0; }

public:
	efield();

	//vector3 getfieldgradient(const vector3& position) { return dbd; };

	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new efield; }
	};
};

/** A linear gradient modifier for forcefields.
* The linear gradient this produces is centered on the origin, and purely vertical. */
class linear_zgradient : public forcefield {
	long double d_dz;

protected:
	void field(vector3& field, const vector3 &position);
	void fieldgradient(vector3& field, const vector3 &position) { field += vector3(0,0,d_dz); }

//	bool prepareobject();
	void readsettings(void);

public:
	linear_zgradient();

	class Factory : public nslobjectfactory {
		nslobject *create() { return new linear_zgradient; }
	};
};

/** Provides dipole calculations.
* This is for a purely 'z' magnetic dipole - that is, a magnetic dipole purely
* in the z plane and centered with only a vertical position. */
class dipole_zmagnetic : public forcefield {
	long double mz;
	long double z;
	
protected:
	void field(vector3 &field, const vector3 &position);
	void fieldgradient(vector3 &field, const vector3 &position);
	
	void readsettings();
	
public:
	dipole_zmagnetic();
	
	class Factory : public nslobjectfactory {
		nslobject *create() { return new dipole_zmagnetic; }
	};
};













#endif

