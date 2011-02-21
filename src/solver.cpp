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

#include "solver.h"
#include "particle.h"
#include "electromagnetics.h"
#include "errors.h"
#include "physics.h"
#include "neutronphysics.h"
#include "edmexperiment.h"
#include "reporters.h"


#include "boost/foreach.hpp"

using std::runtime_error;
using std::endl;
//using neutron_physics::spinvec_change;
//using neutron_physics::Exveffect;

struct particlestore {
	vector3	position;
	vector3 velocity_vec;
	vector3 spinEplus;
	vector3 spinEminus;
	vector3 vxEeffect;
	
	long double velocity; // m s^-1
	
	// Particles velocity gamma
	long double vgamma;
	
	// Number of bounces!
	long bounces;
	
	long double E_sum_phase, E_minus_sum_phase;
	long double flytime; // s
	long double steptime;
};

particlestore storeparticle(const particle &p)
{
	particlestore ps;
	
	ps.position = p.position;
	ps.velocity_vec = p.velocity_vec;
	ps.spinEplus = p.spinEplus;
	ps.spinEminus = p.spinEminus;
	ps.vxEeffect = p.vxEeffect;
	
	ps.velocity = p.velocity;
	
	ps.vgamma = p.vgamma;
	ps.bounces = p.bounces;
	
	ps.E_sum_phase = p.E_sum_phase;
	ps.E_minus_sum_phase = p.E_minus_sum_phase;
	ps.flytime = p.flytime;
	ps.steptime = p.steptime;
	
	return ps;
}

void setparticle ( particle &p, const particlestore &ps)
{
	p.position = ps.position;
	p.velocity_vec = ps.velocity_vec;
	p.spinEplus = ps.spinEplus;
	p.spinEminus = ps.spinEminus;
	p.vxEeffect = ps.vxEeffect;
	
	p.velocity = ps.velocity;
	
	p.vgamma = ps.vgamma;
	p.bounces = ps.bounces;
	
	p.E_sum_phase = ps.E_sum_phase;
	p.E_minus_sum_phase = ps.E_minus_sum_phase;
	p.flytime = ps.flytime;
	p.steptime = ps.steptime;
}

std::ofstream phaselog("phaselog.txt");


solver::solver()
{
	objecttype = "solver";
	types.push_back(objecttype);
	
	return;
}


midpointsolver::midpointsolver()
{
	objecttype = "midpointsolver";
	types.push_back(objecttype);
	
	return;
}

bool midpointsolver::prepareobject ( void )
{
	// Read in our steptime
	require("steptime");
	
	steptime = getlongdouble("steptime", 0.0);
	
	elecfield = (efield*)findbytype("efield", 1);
	if (!elecfield)
		throw runtime_error("Electric field not found from midpointsolver");
	
	magfield = (bfield*)findbytype("bfield", 1);
	if (!magfield)
		throw runtime_error("Magnetic field not found from midpointsolver");
	
	exp = (edmexperiment*)findbytype("edmexperiment", 1);
	if (!exp)
		throw runtime_error("Experiment not found from midpointsolver");
	
	if (exp->get("gravity") == "on")
		gravity = true;
	else
		gravity = false;
	
/*#warning "Initialising debug output files"
	phaselog << "# Debug phase tracking" << endl;
	phaselog << "# Flytime\tBounces\tPlus_phase\tMinus_phase\tphase_diff\tx\ty\tz\tBupx\tBupy\tBupz\tBdownx\tBdowny\tBdownz\tvxex\tvxey\tvxez" << endl;
	phaselog.precision(20);
*/	
	return true;
}

void midpointsolver::prepareparticles ( std::vector<particle*> &particles )
{
	// go through each of the particles and set it's steptime
	BOOST_FOREACH( particle* part, particles) {
		part->steptime = steptime;
	}
	
}

// Run a step for the particle over the specified time. It can safely be assumed that the
// particle will not hit any objects during this time.
void midpointsolver::step( particle &part, const long double &time )
{
//	long steps;
	
	// calculate the Exv effect for the particle- this does not change over a bigstep
	// NOTE: if gravity is on, this does change over a bigstep so don't do this here
	if (!gravity)
		part.updateExv(*elecfield);

	////////////////////////////////////
	/// TEMPORARY DEBUG - match the other programs stepping algorithm
//	steps = (int)(time/steptime) + 1;
//	part.steptime = time / steps;
//#warning "Temporary debug code"
	
	// Calculate the number of steps we are going to take
//	steps = (long)floorl((long double)time / (long double)part.steptime);
	
	// Now do this many smallsteps - after which we will have one step of time less
	// than one step to complete
/*	for (int step = 0; step < steps; step++)
	{
		// We can safely callthe smallstep with steptime as inside this loop it is 
		// guaranteed to be so
		smallstep(part, part.steptime);
		
	}
	
	// Check we still have a little excess time to step
	long double laststep = (time - (steps*part.steptime));
	// Had an annoying but where it 'stepped over' by 1e-38 seconds....... 1 fs should be perfectly fine.
	if (laststep < -1e-15)
		throw runtime_error("Stepping routine stepped over maximum time to impact");
	// Now do the actual final step, if time is not our minimum step
	if (laststep > 1e-9)
		smallstep(part, laststep);
		*/
	// Step while there is time left
	long double timeleft = time;
	while (timeleft - part.steptime > 0)
	{
		smallstep(part, part.steptime);
		timeleft -= part.steptime;
	}
	if (timeleft > 0.) 
		smallstep(part, timeleft);
}

void midpointsolver::smallstep( particle &part, const long double &time)
{
	long double halftime = time / 2.0; // Seconds
	
	// Firstly calculate the step midpoint
	vector3 midpoint, newposition; // Meters
	midpoint = part.position + (part.velocity_vec*halftime); // Meters
	
	// Calculate the (non-gravitational) position after this step... this is because we recalculate
	// velocity here (gravitationally) and this messes up the later calculation!
	newposition = part.position += part.velocity_vec * time; // Meters
	
	// If gravity is turned on, adjust the relevant properties
	if (gravity)
	{
		// the midpoint is different...
		midpoint.z -= 0.5*g*halftime*halftime; // Meters
		
		// update the velocity at the halfway point
		part.velocity_vec.z -= g*halftime; // Meters per second
		part.velocity = mod(part.velocity_vec); // Meters per second

		// And the particles Exv effect
		part.updateExv(*elecfield);
	}
	
	// Just grab the magnetic field - now we have precalculated the magnetic field
	// for the particle already
	vector3 B; // Tesla
	magfield->getfield(B, midpoint); // Tesla
	
	
	// Now apply the new physical properties to the particle
	
	// Move it first
	part.position = newposition; // Meters
	
	//...and gravity adjustments
	if (gravity)
	{
		// Update the position properly
		part.position.z -= 0.5*g*time*time; // Meters
		
		// and finish adjusting the velocity with the other half of the step
		part.velocity_vec.z -= g*halftime; // Meters per second
		part.velocity = mod(part.velocity_vec); // Meters per second
	}
	
	// Update the particles flytime
	part.flytime += time; // Seconds
	
	// Now spin the particle
	neutron_physics::spin_calculation(part, B, time);
	
	// Add this magnetic field to the particles average
	part.sampleBx += B.x;
	part.sampleBy += B.y;
	part.sampleBz += B.z;
	
// Temporarily disable this code for a checkin
	// Output the cumulative phase now!
	/*
#warning "Output debug code"
	vector3 bpvxe, bmvxe;
	bpvxe = B + part.vxEeffect;
	bmvxe = B - part.vxEeffect;
	phaselog << part.flytime << "\t" << part.bounces << "\t" << part.E_sum_phase << "\t" << part.E_minus_sum_phase << "\t"
			<< (part.E_sum_phase - part.E_minus_sum_phase) << "\t"
			 << part.position.x << "\t" << part.position.y << "\t" << part.position.z << "\t"
			 << bpvxe.x << "\t" << bpvxe.y << "\t" << bpvxe.z << "\t"
			 << bmvxe.x << "\t" << bmvxe.y << "\t" << bmvxe.z << "\t"
			 << part.vxEeffect.x << "\t" << part.vxEeffect.y << "\t"  << part.vxEeffect.z << endl;
//		phaselog << "Flytime\tbounces\tPlus_phase\tMinus_phase\tx\ty\tz\tBupx\tBupy\tBupz\tBdownx\tBdowny\tBdownz\tvxex\tvxey\tvxez" << endl;
*/
	
	
	
	// Now (god forbid) call the reporters that report every step
	BOOST_FOREACH( reporter *rep, exp->report_step ) {
		rep->report(*exp);
	}
}
/*
class rungekutta_solver : public solver {
private:
	class efield *elecfield;
	class bfield *magfield;
	class edmexperiment *exp;
	
	long double initial_steptime;
	
public:
		rungekutta_solver();
	
	void step( particle &part, const long double &time );
	
	bool prepareobject( void );
	
	// Initialises all particles to be stepped. 
	//	* This reinitialises all particles to an initial stepping state. 
	void prepareparticles( std::vector<particle*> &particles );
	
	// Allow this to be created from the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new rungekutta_solver; }
	};
};
*/
rungekutta_solver::rungekutta_solver()
{
	objecttype = "rungekutta_solver";
	types.push_back(objecttype);
	
	return;
}

bool rungekutta_solver::prepareobject( void )
{
	// Read in our steptime
	require("initial_steptime");
	require("tolerance");
	require ("minimum_step");
	
	initial_steptime = getlongdouble("initial_steptime", 0.0);
	minimum_step = getlongdouble("minimum_step", 0.00);
	
	tolerance = getlongdouble("tolerance", 0.0);
	
	elecfield = (efield*)findbytype("efield", 1);
	if (!elecfield)
		throw runtime_error("Electric field not found from midpointsolver");
	
	magfield = (bfield*)findbytype("bfield", 1);
	if (!magfield)
		throw runtime_error("Magnetic field not found from midpointsolver");
	
	exp = (edmexperiment*)findbytype("edmexperiment", 1);
	if (!exp)
		throw runtime_error("Experiment not found from midpointsolver");
	
	if (exp->get("gravity") == "on")
		gravity = true;
	else
		gravity = false;
	
	return true;
}

void rungekutta_solver::prepareparticles( std::vector<particle*> &particles )
{
	// go through each of the particles and set it's steptime
	BOOST_FOREACH( particle* part, particles) {
		part->steptime = initial_steptime;
	}	
}


void rungekutta_solver::step( particle &part, const long double &time )
{
	// Use this routine from the midstepper for now
	
	if (!gravity)
		part.updateExv(*elecfield);
	/*
	long double steps = (long)floorl((long double)time / (long double)part.steptime);

	for (int step = 0; step < steps; step++)
	{
		rkstep(part, part.steptime);	
	}
	
	// Check we still have a little excess time to step
	long double laststep = (time - (steps*part.steptime));
	if (laststep < 0.0)
		throw runtime_error("Stepping routine stepped over maximum time to impact");
	// Now do the actual final step
	rkstep(part, laststep);*/
	
	long double timeleft = time;

	while ( timeleft - part.steptime > 0 )
	{
		rkstep(part, part.steptime);
		timeleft -= part.steptime;
	}
	if (timeleft > 1e-9)
		rkstep(part, timeleft);

/*
	//long double spindot = tolerance + 1.;
	
	if (timeleft < part.steptime*2)
	{
//		logger << "STEPTIME OVER WHOLE STEP" << endl;
		// Force it to evaluate, as long as timeleft isn't under/about the minimum stepsize
		if (timeleft / 2.001 > minimum_step)
			part.steptime = timeleft / 2.001;
		//logger << "-------" << endl;
	}

	//Only do the double stepping procedure if the timeleft is more than twice the stepsize
	while (timeleft > part.steptime*2)
	{
		// Backup the particle's properties
		//particle partstore, norm_store;
		//memcpy(&norm_store, &part, sizeof(particle));
		//memcpy(&partstore,&part,sizeof(particle));
		particlestore pstore, normstore, doublestore, halfstore;
		
		pstore = storeparticle(part);
		
		// Take a single step of the particles steptime
		rkstep(part, part.steptime);
		
		// Only worry about spinEplus, reverse should be affected the same way
		vector3 normstep_spin = part.spinEplus;
		normstore = storeparticle(part);
		
		// Reset the particle
		setparticle(part, pstore);
		
		// Now take a double step
		part.steptime *= 2.;
		rkstep(part, part.steptime);
		doublestore = storeparticle(part);
		
		// Compare this spin with the normal steptime spin
		long double spindot = 1. - (part.spinEplus * normstep_spin);
		
		//Check the tolerance...
		if (spindot < tolerance)
		{
			// Within tolerance, we can double the stepsize!
			;//logger << "+" << std::flush;	
			//since particle has already been stepped, we don't need to do anyhting else here
			//logger << "Stepping up steptime: " << part.steptime << endl;
		} else {
			// NOT within tolerance, we cannot double, and may have to halve the stepsize
			
			//Reset the particle from storage...
			setparticle(part,pstore);
			
			// Don't try stepping down if at minimum stepsize
			if ((part.steptime / 2.) > minimum_step)
			{
				// Run the particle with half-stepsize
				part.steptime /= 2.;
				rkstep(part, part.steptime);
				
				// Now calculate if the tolerance is withing range here
				spindot = 1. - (normstep_spin * part.spinEplus);
				if (spindot < tolerance)
				{
					// The current steptime is within tolerances!
					// copy back the particle settings
					setparticle(part,normstore);
				} else {
					// We need to have a smaller steptime. Don't waste any more time here and 
					// shrink it next time.
					// The particle already has the small step applied!
					//logger << "Stepping down steptime: " << part.steptime << endl;
				;//	logger << "-" << std::flush;
				}
			} else {
				//logger << "x" << std::flush;
			}
			
		} // spindot < tolerance
		
		// Reduce the time left, by whatever the particle stepsize is now
		timeleft -= part.steptime;
	} // while (timeleft > part.steptime*2)
	
	// By this point, there is timeleft < 2*steptime. Finish off the steps without bothering about altering stepsize
	if (timeleft > part.steptime)
	{
		rkstep(part, part.steptime);
		timeleft -= part.steptime;
	}
	
	// Now do the final step!
	rkstep(part, timeleft);
*/
}

void rungekutta_solver::rkstep( particle &part, const long double &time )
{
	long double halftime = time / 2.;
	
	// Calculate the positions throughout the step
	vector3 start_pos, mid_pos, end_pos;
	start_pos = part.position;
	mid_pos = part.position + (part.velocity_vec*halftime);
	end_pos = part.position + (part.velocity_vec*time);
	
	//..And calculate the velocities throughout the step.. if there is no gravity
	// then this will be constant
	vector3 start_vel, mid_vel, end_vel;
	start_vel = mid_vel = end_vel = part.velocity_vec;

	// and adjustments for both of these for gravity
	if (gravity)
	{
		// Position
		mid_pos.z -= 0.5*g*halftime*halftime;
		end_pos.z -= 0.5*g*time*time;
		
		// Velocity
		mid_vel.z -= g*halftime;
		end_vel.z -= g*time;
	}
	
	
//	static void Exveffect( const vector3 &position, const vector3 &velocity, const long double gamma, efield &elecfield, vector3 &vxEeffect );
	
	// Variables to hold the base field at all three positions
	vector3 startfield, midfield, endfield;
	magfield->getfield(startfield, start_pos);
	magfield->getfield(midfield, mid_pos);
	magfield->getfield(endfield, end_pos);

	
	long double gyromag = part.gamma;
	
	
	// Variables to hold the calculations for the Exveffect
	vector3 start_exv, mid_exv, end_exv;
	if (gravity)
	{
		/*
		vector3 start_efield, mid_efield, end_efield;
		elecfield->getfield(start_efield, start_pos);
		elecfield->getfield(mid_efield, mid_pos);
		elecfield->getfield(end_efield, end_pos);
		*/
		neutron_physics::Exveffect(start_pos, start_vel, gyromag, *elecfield, start_exv);
		neutron_physics::Exveffect(mid_pos, mid_vel, gyromag, *elecfield, mid_exv);
		neutron_physics::Exveffect(end_pos, end_vel, gyromag, *elecfield, end_exv);
	}
	else
	{
		// No gravity.. just read the settings off of the particle
		start_exv = mid_exv = end_exv = part.vxEeffect;
	}
	//static void spinvec_change( const long double &time, const long double &gyromag, const vector3 &spinvector, const vector3& mag_field, vector3 &dS)
	
	vector3 oldp, oldm;
	oldp = part.spinEplus;
	oldm = part.spinEminus;
	
	vector3 d1_p, d2_p, d3_p, d4_p;
	vector3 d1_m, d2_m, d3_m, d4_m;
	vector3 half_d_p, half_d_m;
	
	// Calculate the first difference estimation
	neutron_physics::spinvec_change(time, gyromag, part.spinEplus, startfield + start_exv, d1_p);
	neutron_physics::spinvec_change(time, gyromag, part.spinEminus, startfield - start_exv, d1_m);
	// Calculate the 'halfway-estimates' of these
	half_d_p = part.spinEplus + (d1_p / 2);
	half_d_m = part.spinEminus + (d1_m / 2);
	half_d_p.scaleto(1.0);
	half_d_m.scaleto(1.0);
	
	// Now calculate the second difference approximation
	neutron_physics::spinvec_change(time, gyromag, half_d_p, midfield + mid_exv, d2_p);
	neutron_physics::spinvec_change(time, gyromag, half_d_m, midfield - mid_exv, d2_m);
	// and again, the halfway estimations
	half_d_p = part.spinEplus + (d2_p / 2);
	half_d_m = part.spinEminus + (d2_m / 2);
	half_d_p.scaleto(1.0);
	half_d_m.scaleto(1.0);
	
	// Now for the third difference approximations!
	neutron_physics::spinvec_change(time, gyromag, half_d_p, midfield + mid_exv, d3_p);
	neutron_physics::spinvec_change(time, gyromag, half_d_m, midfield - mid_exv, d3_m);
	// Full distance changes.. but reusing halfway variables
	half_d_p = part.spinEplus + d3_p;
	half_d_m = part.spinEminus + d3_m;
	half_d_p.scaleto(1.0);
	half_d_m.scaleto(1.0);
	
	// Now the fourth and final difference approximation
	neutron_physics::spinvec_change(time, gyromag, half_d_p, endfield + end_exv, d4_p);
	neutron_physics::spinvec_change(time, gyromag, half_d_m, endfield - end_exv, d4_m);
	
	// Now calculate the final difference from these
	vector3 df_p, df_m;
	df_p = (d1_p + 2*d2_p + 2*d3_p + d4_p) / 6;
	df_m = (d1_m + 2*d2_m + 2*d3_m + d4_m) / 6;
	
	// Add these to the spin vectors
	part.spinEplus	+= df_p;
	part.spinEminus += df_m;

	// Now scale them
	part.spinEplus.scaleto(1.0);
	part.spinEminus.scaleto(1.0);
	
	// Calculate the phase change (for one of them)
	
	long double cosphase = (oldp.x*part.spinEplus.x + oldp.y*part.spinEplus.y) / (oldp.modxy()*part.spinEplus.modxy());
	/*if (cosphase > 1.)
		logger << "Cosphase > 1 by : " << lddistance(cosphase, 1.) << " (Time = " << std::scientific << time << ")" << endl;
	*/
	long double phase_change = acos(cosphase);
	part.E_sum_phase += phase_change;
	
	// And now the other
	cosphase = (oldm.x*part.spinEminus.x + oldm.y*part.spinEminus.y) / (oldm.modxy()*part.spinEminus.modxy());
	/*if (cosphase > 1.)
		logger << "Cosphase > 1 by : " << lddistance(cosphase, 1.) << " (Time = " << std::scientific << time << ")" << endl;
	*/
	phase_change = acos(cosphase);
	part.E_minus_sum_phase += phase_change;
	
	
	//logger << "Phase Change: " << phase_change << endl;
	
	part.flytime += time;
}
