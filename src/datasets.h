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

/*
 *  datasets.h
 *  nslneutron
 *	A class for keeping track of large datasets wherein the individual result numbers are unimportant,
 *	and only the bulk properties such as mean and standard deviation are interesting.
 *
 *  Created by Nicholas Devenish on 11/10/2006.
 *
 */

#ifndef DATASETS_H
#define DATASETS_H

#include <boost/operators.hpp>

/** Tracks information about a massive dataset.
* This class does not track invididual dataset values, but keeps track
* of the bulk values such as mean, variance etc. */
class dataset : boost::addable< dataset >//,
				// boost::addable< long double > >
{
	long double vaverage;
	long double vaveragesq;
	long vpointcount;
	
	public:
		/// Retrieves the average value of the dataset
		long double average( void ) { return vaverage; }
		/// Retrieves the average square value of the dataset
		long double averagesq( void ) { return vaveragesq; }
		/// Retrieves the number of points in the dataset
		long points( void ) { return vpointcount; }
		
		/** Retrieves the varaiance of the dataset.
		* This value is only calculated when it is requested. */
		long double variance ( void );
		/** Retrieves the standard deviation of the dataset.
		* This value is only calculated when requested. */
		long double stdev ( void );
		/** Retrieves the uncertainty in the dataset.
		* This is calculated as the standard error i.e. stdev / sqrt(N) */
		long double uncert ( void );
		
		/** Adds a new point to the data set.
		*	A new point is added to the data set by, literally, adding it on.
		* The mean and mean-squared value are updated here, but not values
		* such as the standard deviation and variance - this is for efficiencies
		* sake and the end result is transparent to the user anyway because the
		* only way they can retrieve these values is through the functions variance()
		* and std().
		*
		* Only mean and mean squared are tracked, and they are done in a way that should
		* minimize floating point errors. */
		dataset operator+=( const long double& newpoint );
		//dataset operator+(const long double& lhs, const dataset& rhs );
		//dataset operator+( const dataset& rhs; const long double &lhs );
		
		/** Merges the data set with another.
		* NOTE: Not yet thoroughly testes, however should work fine.
		*
		* This will combine two datasets, however with the method used
		* it is not necessaraly as accurate as adding a plain data point. */
		dataset operator+=( const dataset& newset );
		
		dataset() : vaverage(0), vaveragesq(0), vpointcount(0) {}
};

dataset dataset::operator+=(const long double& newpoint)
{
	// Update the running average
	vpointcount++;
	vaverage += (newpoint - vaverage) / (vpointcount);
	vaveragesq += ((newpoint*newpoint) - vaveragesq) / (vpointcount);
	
	return *this;
}
//dataset operator+(const long double& lhs, const dataset& rhs )

//dataset operator+( const dataset& rhs; const long double &lhs );
		

dataset dataset::operator+=( const dataset &mergeset )
{
	vaverage = ((vaverage*vpointcount)+(mergeset.vaverage*mergeset.vpointcount)) / (vpointcount+mergeset.vpointcount);
	vaveragesq = ((vaveragesq*vpointcount)+(mergeset.vaveragesq*mergeset.vpointcount)) / (vpointcount+mergeset.vpointcount);
	vpointcount += mergeset.vpointcount;
	return *this;
}
long double dataset::variance ( void )
{
	return vaveragesq - (vaverage*vaverage);
}

long double dataset::stdev ( void )
{
	return sqrtl(variance());
}

long double dataset::uncert ( void )
{
	return stdev() / sqrtl((long double) vpointcount);
}
#endif