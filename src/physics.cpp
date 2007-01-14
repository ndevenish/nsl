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
#include <assert.h>
#include <cmath>
#include <float.h> // for _finite

//#include <stdlib.h>
#include "errors.h"

#include "physics.h"

#include "datasets.h"

using std::runtime_error;


std::ostream& operator<<(std::ostream& os, const vector3& vec)
{
  //char fillc = os.fill('0');
  //os << "[ " << vec.x << ", " << vec.y << ", " << vec.z << " ]";
  os << vec.x << ", " << vec.y << ", " << vec.z;
  return os;
}

std::ostream& operator<<(std::ostream& os, const dataset& dst)
{
	os << dst.average() << " +/- " << dst.uncert();
	return os;
}
/*
vector3 operator+(const vector3& lhs, const vector3&rhs)
{
	return 
}*/

// Scales the vector to fit a specified length
void vector3::scaleto(long double newlength)
{
	long double scale_ratio;
	long double modcache;
	
	if (newlength == 0.0)
	{
		x = y = z = 0.0;
		return;
	} else if ((modcache = mod()) == 0.0) {
		throw runtime_error("Attempting to scale zero-length vector");
	}
	
	scale_ratio = newlength/modcache;
	
#ifdef WIN32
	assert(_finite(scale_ratio));
#endif
	
	x *= scale_ratio;
	y *= scale_ratio;
	z *= scale_ratio;
}


// Usable AlmostEqual function
// Code from a paper by Bruce Dawson for comparing floats by integer representation
// http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
bool AlmostEqual2sComplement(long double A, long double B, int maxUlps)
{
    // Make sure maxUlps is non-negative and small enough that the
    // default NAN won't compare as equal to anything.
    assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);
    
	long long aLL = *(long long*)&A;
    // Make aInt lexicographically ordered as a twos-complement int
    if (aLL < 0)
        aLL = 0x8000000000000000LL - aLL;

    // Make bInt lexicographically ordered as a twos-complement int
    long long bLL = *(long long*)&B;
    if (bLL < 0)
        bLL = 0x8000000000000000LL - bLL;
    
	//long long llDiff = llabs(aLL - bLL);
	long long llDiff = (aLL - bLL);
	if (llDiff < 0)
		llDiff *= -1;

    if (llDiff <= maxUlps)
        return true;
    return false;
}

// Variation that returns the 'float distance' between two numbers
long long lddistance(long double A, long double B)
{
//    assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);
    
	long long aLL = *(long long*)&A;
    // Make aInt lexicographically ordered as a twos-complement int
    if (aLL < 0)
        aLL = 0x8000000000000000LL - aLL;

    // Make bInt lexicographically ordered as a twos-complement int
    long long bLL = *(long long*)&B;
    if (bLL < 0)
        bLL = 0x8000000000000000LL - bLL;
    
	//long long llDiff = llabs(aLL - bLL);
	long long llDiff = (aLL - bLL);
	if (llDiff < 0)
		llDiff *= -1;

	return llDiff;
}


