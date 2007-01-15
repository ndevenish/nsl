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
//#include <cmath>

#include "vector3.h"
//#include "datasets.h"

// Two functions to compare long doubles accurately
long long lddistance(long double A, long double B);
bool AlmostEqual2sComplement(long double A, long double B, int maxUlps);

// *************************************************************
// Physical Constants

const long double pi = 3.1415926535897932384626433832795028841971693993751058209; ///< Pi
const long double echarge = 1.60217646e-19; ///< The charge of an electron
const long double csquared = (299792458.*299792458.); ///< The speed of light squared
const long double hbar = 1.054571596e-34; ///< h-bar (h/2*pi)
const long double mu0 = 12.566370614E-7;
const long double dipole_mult = mu0 / (4.*pi);

const long double g = 9.80665;
const long double k = 1.3806503e-23;

// ***************************************************************
// Other inline functions

#endif

