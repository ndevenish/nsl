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


#ifndef SOLVER_H
#define SOLVER_H

class particle;
#include "nslobject.h"
#include "nslobjectfactory.h"

#include <vector>

/// Runs a particle over time, calculating it's properties as it goes. Cannot be created though nsl scripting.
class solver : public nslobject {
protected:
	bool gravity;
	
public:
	solver();

	// Runs a particle for a specified length of time
	virtual void step( particle &part, const long double &time ) {};
	
	virtual void prepareparticles( std::vector<particle*> &particles ) {};
};

class midpointsolver : public solver {
private:
	long double steptime;
	class efield *elecfield;
	class bfield *magfield;
	class edmexperiment *exp;
	
	void smallstep( particle &part, const long double &time);

public:
	midpointsolver();
	
	void step( particle &part, const long double &time );

	bool prepareobject( void );
	
	/** Initialises all particles to be stepped. 
	* This reinitialises all particles to an initial stepping state. */
	void prepareparticles( std::vector<particle*> &particles );
	
	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new midpointsolver; }
	};
};

class rungekutta_solver : public solver {
private:
	class efield *elecfield;
	class bfield *magfield;
	class edmexperiment *exp;
	
	long double initial_steptime;
	
public:
		rungekutta_solver();
	
	void step( particle &part, const long double &time );
	void rkstep( particle &part, const long double &time );
	
	bool prepareobject( void );
	
	/** Initialises all particles to be stepped. 
		* This reinitialises all particles to an initial stepping state. */
	void prepareparticles( std::vector<particle*> &particles );
	
	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new rungekutta_solver; }
	};
};
#endif

