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
#include <ctime> // For time()


#include "random.h"

#include <boost/random.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

using boost::mt11213b;
using std::cout;
using std::endl;

static mt11213b rng;
static boost::uniform_real<> uni_r(0,1);
static boost::normal_distribution<> norm_r(0,1);

static boost::variate_generator<mt11213b&, boost::uniform_real<> > uni(rng, uni_r);
static boost::variate_generator<mt11213b&, boost::normal_distribution<> > norm(rng, norm_r);

// Initialise the random number generator
//initrn initirandomm();
namespace nsl{
	
void seedrand()
{
	cout << "Initialising random number generator..." << endl;
	rng.seed(static_cast<unsigned> (std::time(0)));
}

long double rand_uniform()
{
	return uni();
}

long double rand_normal()
{
	return norm();
}

}

