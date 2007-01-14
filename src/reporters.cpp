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
#include <fstream>
#include <string>
#include <ctime>

#include <boost/random.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>


#include "errors.h"
#include "reporters.h"
#include "edmexperiment.h"
#include "particle.h"
#include "container.h"

#include "electromagnetics.h"

#include "boost/foreach.hpp"

using std::string;
using std::runtime_error;
using std::ofstream;

using std::endl;

using boost::mt11213b;


reporter::reporter()
{
	outfile = 0;
//	report_count = 0;
	
	streamname = "void";

	// Set a default reporting frequency
	report_frequency = rfreq_none;

	objecttype = "reporter";
	types.push_back(objecttype);
}

reporter::~reporter()
{
	if (outfile)
		delete (ofstream*)outfile;
}

bool reporter::prepareobject()
{
	// See if we should override the default report frequency for this class
	string runfreq;
	if(isset("report_frequency"))
	{
		runfreq = get("report_frequency");
		if (runfreq == "none")
			report_frequency = rfreq_none;
		else if (runfreq == "run")
			report_frequency = rfreq_run;
		else if (runfreq == "bounce")
			report_frequency = rfreq_bounce;
		else if (runfreq == "step")
			report_frequency = rfreq_step;
		else if (runfreq == "interval")
			report_frequency = rfreq_interval;
		else
			throw runtime_error("Unknown report frequency being set in reporter");
	}
	
	// Get the output format
	string outfmt;
	if (isset("output_format"))
	{
		outfmt = get("output_format");
		if (outfmt == "plain")
			output_format = format_plain;
		else
			throw runtime_error("Unknown output format being specified.");
	}
	
	// Get the output filename
	if (!isset("output_file")) {
		outfile = &(std::cout);
		streamname = "stdout";
	} else {
		streamname = get("output_file");
		std::cout << "Logging to stream: " << streamname << endl;
		
		outfile = new ofstream(streamname.c_str());
		
	}
	
		
	return true;
}

void reporter::preparefile( edmexperiment &experiment )
{
	
	// Warn the user if no output file being written
	logger << "Warning: No header being written for output file " << get("output_file") << endl;
	
	/*
	*outfile << "Base Data file" << endl;
	
	*outfile << experiment.get("runtime") << endl;
	*outfile <<  "---------------------------------" << endl;
*/
	
  //printf ( "Current date and time are: %s", asctime (timeinfo) );
  
	
}
	
void reporter::closefile( edmexperiment &experiment )
{/*
	*outfile << "---------------------------------" << endl;
	*outfile << "End of all data runs at ";
	
	time_t rawtime;
	tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	//set("runtime",  asctime(timeinfo));
	*outfile << asctime(timeinfo) << endl;*/
}
	
void reporter::report( edmexperiment &experiment )
{
	static long stepcount = 0;
	
	*outfile << "Report #" << ++stepcount << endl;
}

/////////////////////////////////////////////////////////
// Impactreporter - a small class to log impact positions
impactreporter::impactreporter() {
	report_frequency = rfreq_bounce;
//	report_frequency = rfreq_none;
	objecttype = "impactreporter";
	types.push_back(objecttype);
}
void impactreporter::report( edmexperiment &experiment ) {
	static long bounce = 0;
	
	*outfile << bounce++ << ", " << experiment.particles[0]->flytime << ", " << experiment.particles[0]->position << endl;
}

/////////////////////////////////////////////////////////
// Phasereporter - a small class to output the phase over time
phasereporter::phasereporter()
{
	objecttype = "phasereporter";
	types.push_back(objecttype);
	
	report_frequency = rfreq_step;
//	report_frequency = rfreq_none;
}

void phasereporter::preparefile (edmexperiment &exp)
{
	*outfile << "# Phase progress log file: " << exp.get("runtime") << endl;
	*outfile << "# #Num, flytime, phase_e+, diff" << endl << "------------------------------------" << endl;
}
void phasereporter::report ( edmexperiment &ex )
{
	static long stepa = 0;
	outfile->precision(20);
	*outfile << ++stepa << ", " << ex.particles[0]->flytime 
				//<< ", " << ex.particles[0]->spinEplus << endl;
	
				<< ", " << ex.particles[0]->E_sum_phase 
				<< ", " << ex.particles[0]->E_minus_sum_phase
				<< ", " << ex.particles[0]->E_sum_phase - ex.particles[0]->E_minus_sum_phase
				/*<< ", " << ex.change.average()
				<< ", " << ex.tmpld*/
				<< endl;
}


/////////////////////////////////////////////////////////
// Interval reporter - a simple test of interval reporting

intervalreporter::intervalreporter()
{
	objecttype = "intervalreporter";
	types.push_back(objecttype);
	
	report_frequency = rfreq_interval;
}

void intervalreporter::preparefile(edmexperiment &exp)
{
	*outfile << "# Interval reporter test run " << endl;
	*outfile << "# List of times at the inteval" << endl;
}

void intervalreporter::report ( edmexperiment & experiment )
{
	dataset ensemble;
	
	// calculate the average flytime
	BOOST_FOREACH(particle *p, experiment.particles)
	{
		ensemble += p->flytime;
	}
	*outfile << ensemble.points() << "\t" << ensemble << endl;;
	
	
}
 
/////////////////////////////////////////////////////////
// edmreporter - The main variational output class
edmreporter::edmreporter()
{
	objecttype = "edmreporter";
	types.push_back(objecttype);
	
	report_frequency = rfreq_run;
}

void edmreporter::preparefile(edmexperiment &exp)
{
	*outfile << "# Edm loop report: " << exp.get("runtime") << endl;
	*outfile << "# " << exp.variation.parameter << "\t" << "False-EDM" << "\t" << "Error";
	
	if (volaverage)
		*outfile << "\t" << "volavg-dbzdz" << "\t" << "error";
	
	*outfile << endl;
}

void edmreporter::report ( edmexperiment &experiment )
{
	// Loop over all the particles and generate an average EDM
	dataset edmav;
	BOOST_FOREACH( particle *part, experiment.particles )
	{
		edmav += part->cumulativeedm;
	}
	
	*outfile << experiment.variation.value << "\t" <<  edmav.average() << "\t" << edmav.uncert();
	
	// Calculate an average vertical field gradient if required
	if (volaverage)
	{
		// Grab pointers to the container and magnetic field
		container *box = experiment.particlebox;
		bfield* b = experiment.magfield;
		dataset vergrad = calc_dbdz(*b, *box);
		
		*outfile << "\t" << vergrad.average() << "\t" << vergrad.uncert();
		
	}
		
	*outfile << endl;
}

bool edmreporter::prepareobject()
{
	reporter::prepareobject();
	
	if (isset("volaverage_dbdz"))
		volaverage = true;

	return true;
}


/// Calcualte the volume average dbz/dz value
dataset edmreporter::calc_dbdz(bfield &b, container &cont)
{
	// Create and initialise the random number generator 
	mt11213b rng;
	rng.seed(static_cast<unsigned> (std::time(0)));
	boost::uniform_real<> uni_r(0,1);
	boost::variate_generator<mt11213b&, boost::uniform_real<> > uni(rng, uni_r);
	
	// Let's grab the size of the volume to generate over
	cylbounds cyl = cont.getcylinder();
	
	unsigned long hits = 0, misses = 0, throws = 0;
	
//	long double x, y, z;
	vector3 pos, magfield;
	dataset vertgrad;
	
	while (1)
	{
		//r = sqrtl(uni()) * cyl.radius;
		pos.z =(uni() - 0.5) * cyl.height;
		pos.x = uni() * cyl.radius*2. - cyl.radius;
		pos.y = uni() * cyl.radius*2. - cyl.radius;
		
		// Throw these out if outside test cylinder
		if (pos.x*pos.x + pos.y*pos.y > cyl.radius *cyl.radius)
		{
			throws++;
			continue;
		}
		pos += cyl.position;
		
		//Now, test to see if these points are inside a container
		if (!cont.isinside(pos))
		{
			// If it isn't inside a container, this is a miss. This can be used to estimate the colume
			misses++;
			continue;
		} else {
			hits++;
			magfield = vector3(0.0,0.0,0.0);
			b.getfieldgradient(magfield,pos);
			
			vertgrad += magfield.z;
		}
		
		if (hits > 1e6)
			break;
	}
	// Calculate the estimated volume
	long double hitratio = (long double)hits / (long double)(hits+misses);
	long double volumeestimate = hitratio * pi * cyl.radius * cyl.radius * cyl.height;
	// Estimate the error on the volume, by culculating the volume estimate change by a
	// single hit difference
	long double volumeplushitest = (((long double)(hits+1) / (long double)(hits+misses+1))) * pi * cyl.radius * cyl.height * cyl.radius;
	long double volerrest = volumeestimate * ((volumeestimate / volumeplushitest) - 1.);
	
	
	logger << "  DBDZ averaging: Hits: " << hits << ", Misses: " << misses << endl;
	logger << "     Estimated Volume = " << volumeestimate << " +- " << volerrest << endl;
	logger << "		dBz/dz			 = " << vertgrad << endl;
	//vertgrad /= volumeestimate;
	//logger << "   Average field gradient over volume : " << vertgrad<< endl;
	
	return vertgrad;
}

/////////////////////////////////////////////////////////
// Polarizationrepoter - reports on the polarisation of the particles
// at an interval step

polreporter::polreporter()
{
	objecttype = "polreporter";
	types.push_back(objecttype);
	
	report_frequency = rfreq_interval;
}

void polreporter::preparefile(edmexperiment &exp)
{
	*outfile << "# Polarization report: " << exp.get("time") << endl;
	*outfile << "# Flight-time\t Average_phase\t Stdev\t Average_Phase(-E)\t Stdev" << endl;
}

bool polreporter::prepareobject()
{
	reporter::prepareobject();
	
	return true;
}

void polreporter::report( edmexperiment &exp )
{
	// Firstly, calculate the average cumulative frequencies
	dataset cumfreq, cumfreqminus, flighttime;
	
	BOOST_FOREACH(particle *p, exp.particles)
	{
		cumfreq		+= p->E_sum_phase;	//long double E_sum_phase, E_minus_sum_phase;
		cumfreqminus+= p->E_minus_sum_phase;
		flighttime += p->flytime;
	}
	if (flighttime.stdev() > 1e-7)
	{
		logger << "Flighttime Deviation: Flighttime: " << flighttime.average() << " +/- " << flighttime.stdev() << endl;
		//throw runtime_error("Flight-time of particles do not all agree");
	}
	
	*outfile << flighttime.average() << "\t" << cumfreq.average() << "\t" << cumfreq.stdev()
			 << "\t" << cumfreqminus.average() << "\t" << cumfreqminus.stdev();

	*outfile << endl;
}

/////////////////////////////////////////////////////////
// Poldistreporter - reports on the distribution of particles
/*
class poldistreporter : public reporter {
protected:
	void preparefile( edmexperiment &exp );
	
public:
	poldistreporter();
	void report ( edmexperiment &experiment );
	
	class Factory : public nslobjectfactory {
		nslobject *create() { return new poldistreporter; }
	};	
};*/
poldistreporter::poldistreporter()
{
	objecttype = "poldistreporter";
	types.push_back(objecttype);
	
	report_frequency = rfreq_interval;
}

bool poldistreporter::prepareobject ( void )
{
	reporter::prepareobject();

	if (isset("negphase"))
		logphase = phase_negative;
	else
		logphase = phase_positive;

	return true;
}
void poldistreporter::preparefile( edmexperiment &exp )
{
	*outfile << "# Polarization Distribution report: " << exp.get("time") << endl;
	*outfile << "# Reporting ";
	if (logphase == phase_negative)
		*outfile << "negative ";
	else	
		*outfile << "positive ";
	*outfile << "phase." << endl;
	*outfile << "# Flight-time\tAverage_phase";
	for (unsigned int i = 0; i < exp.particles.size(); i++)
		*outfile << "\t" << "Part_" << i+1 << "_phase";
	*outfile << endl;
}

void poldistreporter::report( edmexperiment &exp )
{
	dataset time, phase;
	//Grab the elapsed time
	BOOST_FOREACH(particle *p, exp.particles)
	{
		time += p->flytime;
		if(logphase == phase_positive)
			phase += p->E_sum_phase;
		else
			phase += p->E_minus_sum_phase;
	}
	
	outfile->precision(20);
	*outfile << time.average() << "\t" << phase.average();
	
	BOOST_FOREACH(particle *p, exp.particles)
		if (logphase == phase_positive)
			*outfile << "\t" << (p->E_sum_phase-phase.average());
		else 
			*outfile << "\t" << (p->E_minus_sum_phase-phase.average());
	
	*outfile << endl;
}
