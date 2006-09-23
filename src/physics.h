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

/* *********************************************************************************
* Physics.h - contains all the physical constant delcarations, along with maths    *
* functions and variable types.                                                    *
********************************************************************************** */

#ifndef PHYSICS_H
#define PHYSICS_H

#include <string.h>
#include <iostream>
#include <math.h>

/// A Class to represent a simple three-component vector autonomously
struct vector3 {
public:
	long double x;
	long double y;
	long double z;

	/// Initialises all the vector components to 0.0
	vector3() : x(0.0), y(0.0), z(0.0) {}
	/// Initialises the vector with three provided values
	vector3( long double xx, long double yy, long double zz) : x(xx), y(yy), z(zz) { }

	vector3 operator+(const vector3& rhs) { return vector3(x+rhs.x, y+rhs.y, z+rhs.z); }
	vector3 operator-(const vector3& rhs) { return vector3(x-rhs.x, y-rhs.y, z-rhs.z); }

	vector3 operator*(const long double s) { return vector3(x*s, y*s, z*s); }
	long double operator*(const vector3 vec) { return x*vec.x + y*vec.y + z*vec.z; }

	vector3& operator+=(const vector3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	vector3& operator*=(const long double &rhs) { x *= rhs; y *= rhs; z *= rhs; return *this; }
	
	int operator==(const vector3 &rt) {
		return (x == rt.x) && (y == rt.y) && (z == rt.z);
	}


	/// Returns the length of the vector
	long double vector3::mod() {	return sqrtl(x*x + y*y + z*z); }

	/// Scales the vector to a specified length
	void scaleto(long double newlength);


	//void print( void ) { std::cout << "[ " << x << ", " << y << ", " << z << " ]"; }
	//vector3 operator+=(const
};

inline long double dot(const vector3 &a, const vector3 &b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

typedef vector3 vec3;

// Define the output stream function
/// A function to allow vectors to be sent to output stream objects
std::ostream& operator<<(std::ostream& os, const vector3& vec);


// Two functions to compare long doubles accurately
long long lddistance(long double A, long double B);
bool AlmostEqual2sComplement(long double A, long double B, int maxUlps);

// *************************************************************
// Physical Constants

const long double pi = 3.1415926535897932384626433832795028841971693993751058209; ///< Pi
const long double echarge = 1.60217646e-19; ///< The charge of an electron
const long double csquared = (299792458.*299792458.); ///< The speed of light squared
const long double hbar = 1.054571596e-34; ///< h-bar (h/2*pi)


// ***************************************************************
// Other inline functions

#endif