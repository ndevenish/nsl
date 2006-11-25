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

#include "errors.h"
#include "reporters.h"
#include "edmexperiment.h"
#include "particle.h"

#include "boost/foreach.hpp"

using std::string;
using std::runtime_error;
using std::ofstream;

using std::endl;


//using namespace std;


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
{/*
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
	*outfile << "Phase progress log file: " << exp.get("runtime") << endl;
	*outfile << "#Num, flytime, phase_e+, diff" << endl << "------------------------------------" << endl;
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
// edmreporter - The main variational output class
edmreporter::edmreporter()
{
	objecttype = "edmreporter";
	types.push_back(objecttype);
	
	report_frequency = rfreq_run;
}

void edmreporter::preparefile(edmexperiment &exp)
{
	*outfile << "Edm loop report: " << exp.get("runtime") << endl;
	*outfile << exp.variation.parameter << "\t" << "False-EDM" << "\t" << "Error"<< endl;
}
void edmreporter::report ( edmexperiment &experiment )
{
	// Loop over all the particles and generate an average EDM
	dataset edmav;
	BOOST_FOREACH( particle *part, experiment.particles )
	{
		edmav += part->cumulativeedm;
	}

	*outfile << experiment.variation.value << "\t" <<  edmav.average() << "\t" << edmav.uncert() << endl;
}
