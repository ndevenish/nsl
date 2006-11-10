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


#ifndef REPORTERS_H
#define REPORTERS_H

#include <iosfwd>
#include <string>

#include "nslobject.h"
#include "nslobjectfactory.h"

class edmexperiment;
class particle;


/** Reports statuses to log files.
* This class is the base object for logging and tracking aspects
* of the simulation. */
class reporter : public nslobject {
public:
	enum enum_report_frequency {
		rfreq_none,
		rfreq_run,
		rfreq_bounce,
		rfreq_step
	};
	enum enum_output_format {
		format_plain
	};
	
private:
	std::string	streamname; ///< The name of the output stream
	
	/** The file format to output into */
	enum_output_format output_format;
	
	/** Stores the number of times it has been called to report */
//	long report_count;
	
//	edmexperiment *experiment;
	
protected:
	bool prepareobject();
	std::ostream *outfile; ///< The stream to use for output
	
	/** Stores the frequency of reporting */
	enum_report_frequency report_frequency;
	
	
	
public:
	reporter( );
	~reporter();
	
	/** Prepares the data file.
	This function is for preparing the data file and other pre-run stuff. */
	virtual void preparefile( edmexperiment &experiment );
	
	/** Closes the data file.
	This function closes the data file and writes out any final information. */
	virtual void closefile( edmexperiment &experiment );
	
	
	/** Reports on an individual particle.
	This function reports on a single particle. */
	//virtual void report( particle &particle) {}
	/** Reports on the state of an experiment.
	This function generates a report for the state of the whole
	experiment. */
	virtual void report( edmexperiment &experiment );

	/** Returns the frequency of when the reporter should be run.
	Note that this is only a 'label' and that the class may be
	called at any time. */
	enum_report_frequency when(void) { return report_frequency; };
	


	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new reporter; }
	};
};

/** Logs each position a particle impacts.
This will only work for the first particle in the edmexperiment object. */
class impactreporter : public reporter {
	
public:
	impactreporter();
	
	void preparefile( edmexperiment & ) {}
	void closefile(edmexperiment &) {}
	
	void report( edmexperiment &experiment );
	
	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new impactreporter; }
	};
};

/** Logs the progress of the phase of a particle.
This will only work for the first particle in an experiment */
class phasereporter : public reporter {
public:
	phasereporter();
	
	void preparefile(edmexperiment &);
	void report ( edmexperiment &experiment );
	
	class Factory : public nslobjectfactory {
		nslobject *create() { return new phasereporter; }
	};
};
#endif