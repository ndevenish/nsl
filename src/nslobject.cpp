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

#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <cctype>
#include <fstream>
#include <set>
#include <stdexcept>
#include <algorithm> // For string transformations

#include "boost/foreach.hpp"
#include "nslobject.h"
#include "cparser.h"
#include "errors.h"
#include "edmexperiment.h" // For variation
#include "tools.h"

using namespace std;



int nslobject::variationcount = 0;
int nslobject::runid = 0;

/*	struct {
bool isvariation;
std::string parameter;
long double value;
} variation;*/
//bool nslobject::variation::isvariation = false;

nslobject::nslobject() : logger(cout)
{
	candeletethis = false;
	parent = 0;
	initialised = false;

//	logger = cout;
	
	objecttype = "nslobject";
	types.push_back(objecttype);
}
nslobject::~nslobject()
{
	//cout << "Deleting " << get("name") << endl;
	for (unsigned int i = 0; i < subobjects.size(); i++)
		if (subobjects[i]->candelete())
			delete subobjects[i];
}

// Add an subobject to the current object
void nslobject::add (nslobject & newobject )
{
	subobjects.push_back(&newobject);

	// Set ourselves as the objects parent
	newobject.setparent(*this);
}

// Set a property for the current 
void nslobject::set ( std::string propertyname, std::string value )
{
	// If setting the name, ensure we do not enter duplicate names
	static std::set<string> namelist;

	if (propertyname == "name")
		if (namelist.count(value))
		{
			//Warning("Throwing a warning");
			stringstream thewarn;
			thewarn << "Duplicate name \"" << value << "\"";
			Warning( thewarn.str() );
			//cout << "Warning: Name \"" << value << "\" already exists and so is duplicate" << endl;
		}
		else
			namelist.insert(value);

	// Convert to lower case
	string newstring;
	transform (propertyname.begin(),propertyname.end(), propertyname.begin(), (int (*)(int))tolower);

	properties[propertyname] = value;
}

bool nslobject::isset( std::string propertyname )
{
	if (properties.find(propertyname) != properties.end())
		return true;
	else
		return false;
}

std::string nslobject::get ( std::string propertyname, std::string defaultval )
{
	if (isset(propertyname))
		return properties[propertyname];
	else
		return defaultval;
}

void nslobject::require( std::string requiredproperty )
{
	if (!isset(requiredproperty))
		throw runtime_error("Required property not found");
}

// Tree outputs a simple representation of the object tree to stdout
void nslobject::tree(int level)
{
	string thisname = get("name");

	cout << gettype() << " [" << get("name") << "]" ;
	/*if (parent)
		cout << " Parent is {" << parent->get("name") << "}"; */
	cout << endl;
	for (unsigned int i = 0; i < subobjects.size(); i++)
	{
		for (int j=0; j < level-1; j++)
			cout << "| ";
		cout << "|-";

		subobjects[i]->tree(level+1);
		//cout << endl;
	}
}

// Creates an object, parses it and returns a pointer to it
// Made because we need multiple adding routines
nslobject *nslobject::createadd( cparser &parser )
{
	string token;
	//next token should be object type
	string newtype = parser.gettoken();
	
	// Call the class factory to create a new object now
	nslobject *newob = nslobjectfactory::createobject(newtype);
	
	// Next token is either a name or a {
	token = parser.gettoken();
	//if (token != "{" && token != "\n")
	//	newob->set("name", token);
	if (isalnum(token[0]))
		newob->set("name", token);
	
	// Give the new object a parent!
	//newob->parent 
	
	add( *newob );
		 
	// Carry on parsing
	newob->parseparser(parser);
	
	return newob;
}

tokens nslobject::parseparser( cparser & parser)
{
	string token;
	//tokens  retval;
	while(1)
	{
		token = parser.gettoken();

		if (parser.eof())
			return token_eof;

		if (token == "set")	{
			//The next two token should be the value to set and it's value respectively
			string propname, value;
			propname = parser.gettoken();
			value = parser.gettoken();
			if ( propname == "\n" || value == "\n" )
			{
				throw unexpected_newline();
				//cout << "Error: Newline while reading value at line " << parser.getline() << endl;
				//return token_stop;
			}
			set(propname, value);
			
		//////////////////////////////////////////////////////////////////////////
		// Adding subobjects	
		} else if (token == "add") {
			//next token should be object type
//			string newtype = parser.gettoken();
/*
			// Call the class factory to create a new object now
			nslobject *newob = nslobjectfactory::createobject(newtype);

			// Add the new object to our child array
			add( *newob );

			// Next token is either a name or a {
			token = parser.gettoken();
			//if (token != "{" && token != "\n")
			//	newob->set("name", token);
			if (isalnum(token[0]))
				newob->set("name", token);
 */
			
			// Instead of doing this here, now do it externally.
			//nslobject *newob = createadd( parser );
			//add( *newob );
			createadd( parser );
			
			//now contunue parsing in the new object
			/*retval = newob->parseparser(parser);
			if (retval == token_stop)
				return token_stop;
			*/
			
		//////////////////////////////////////////////////////////////////////////
		// Adding multiple copies of objects	
		} else if (token == "multiadd") {
			// Next token should be number to add
			string addcounts = parser.gettoken();
			/*
			if (isalnum(token[0]))
				throw parse_error("Encountered alnum token when expecting particle add count");
			*/
			//convert this to an integer
			istringstream iss(addcounts);
			long addcount = 0;
			
			iss >> addcount;
			
			if (iss == 0)
				throw parse_error("Encountered zero multiadd add count");
			
			//next object token should be object type
			//string newtype = parser.gettoken();
			
			// Read in the next block, so we can add it multiple times
			string addblock = parser.getblock();
			//logger << "Scanning block recovered: " << endl << addblock << endl << "---------" << endl;
			
			// Declare a stream in order to pass to multiple addingses
			stringstream parsingblock;
			parsingblock << addblock;
			
			//Now add multiple versions of this object
			for (int i = 0; i < addcount; i++)
			{/*
				// Convert the loop number to a string
				ostringstream ss;
				ss << "_" << i;
				*/
				// Rewind the stringstream to the beginning
				parsingblock.seekg(0, ios::beg);
				
				// Make a parser for this stringstream
				cparser ssparse(&parsingblock);
				
				// Add this object, not caring for now about name clashes
				createadd( ssparse );
			}
			// Now we should have added all the objects....
		
		//////////////////////////////////////////////////////////////////////////
		// Loading in external files	
		} else if (token == "load") {
			//we want to load in an external file, open it in a new fstream
			//and hand a new parser object to this objects parse routine
			token = parser.gettoken();
			load(token);
		
		//////////////////////////////////////////////////////////////////////////
		// Selecting a class to alter parameters
		} else if (token == "with") {
			// The next token should be a name
			token = parser.gettoken();
			if (!isalnum(token[0]))
				throw parse_error("Unexpected non-alphanumeric value trying to read name");

			// The token is a name, so find that named class
			nslobject *retval;
			retval = findbyname(token);
			if (retval)
				retval->parseparser(parser);
			else
				throw parse_error("Unidentified name");

		//////////////////////////////////////////////////////////////////////////
		// Ignoring blocks of code	
		} else if (token == "ignore") {
			// We should ignore this object... loop until we hit the next }
			/*while (parser.gettoken() != "}")
				;*/
			// this new method should work for stacked blocks
			parser.getblock();
		
		//////////////////////////////////////////////////////////////////////////
		// Variation of parameters
		} else if (token == "vary") {
			
			// Ensure we hav eno other variations active
			if (variationcount)
				throw parse_error("Being asked to do more than one variation");
			
			// Next token should be the name of the parameter to cary
			string parameter = parser.gettoken();
			
			// Next token should be from
			if (parser.gettoken() != "from")
				throw parse_error("Unexpected token when expecting 'from' in varation");
			
			// Next token is start value for parameter
			string sstartval= parser.gettoken();
			long double startval = strld(sstartval);
				
			// Next token tells us how to vary
			// (Currently supported is only 'to')
			string varytype = parser.gettoken();
			if (varytype == "to") {
				// Next token is the value to vary to
				string sendval = parser.gettoken();
				long double endval = strld(sendval);
				
				// Next token should be 'in'
				if (parser.gettoken() != "in")
					throw parse_error("Unexpected token when expecting 'in' in variation");
				
				// Next should be number of steps!
				string sstepcount = parser.gettoken();
				int stepcount = strint(sstepcount);
				
				// Next token should be a throwaway - 'steps'
				// otherwise return, so safe to grab
				parser.gettoken();
				
				logger	<< "Two-point variation of " << parameter << ": From " << startval
						<< " to " << endval << " in " << stepcount << " steps." << endl;
				
				// Calculate the value to set this round, and then set it
				// runid is the current run count
				long double thisrunval;
				thisrunval = startval + ((endval-startval)*runid) / (long double) stepcount;
				logger << "\tValue this time: " << thisrunval << endl;
				
				// set the actual parameter value - convert it to a string first!
				ostringstream ss;
				ss.precision(20);
				ss << thisrunval;
				set(parameter, ss.str());
				
				// Find the edmexperiment object and set the varaiation
				edmexperiment *exproot = (edmexperiment*)findbytype("edmexperiment");
				if (!exproot)
					throw runtime_error("Cannot find edmexperiment object to set variation");
				exproot->setvariation(parameter, startval, endval, stepcount, this);
	//				void setvariation( std::string parameter, long double minval, long double maxvalm, int maxruns, nslobject *varyobject );

				/*	struct {
					std::string parameter; ///< What is the name of the parameter we are varying?
				long double minval;
				long double maxval;
				long double value; ///< What value is the variation taking this run?
				
				int runs; ///< How many runs will the variation take?
				bool varying; ///< Are we actually wanting a variation?
				
				nslobject *varyobject; ///< The object to vary
				} variation;*/
				
			} else {
				throw parse_error("Unrecognized variation type");
			}
			
			
			logger << "Varying parameter " << parameter << endl;
		
		//////////////////////////////////////////////////////////////////////////
		// Random punctuation
		} else if (token == "{") {
			//ignore this
			;
		} else if (token == "}") {
			return token_closebrace;
		} else if (token == "\n") {
			;
		} else if (token == "#") {
			// loop until we hit anewline
			while (1)
			{
				token = parser.gettoken();
				if (token == "\n")
					break;
			}
		} else if (token == "stop")	{
			return token_stop;
		} else {
			// See if the virtual functions knows it, if so then dandy otherwise throw
			// an exception
			if (!processtoken(token, parser))
				throw unrecognised_token(token);
		}
	} // infinite loop
	
	// We should never reach this point, so throw an exception if we do.
	throw runtime_error("Reached end of parseparser function.. This should not be possible");
	//return token; //THIS IS NEVER CALLED
}

void nslobject::load(string filename)
{
	cparser newparser(filename);
	parse(newparser);
}

// Parsestream: Creates a parser object from an input stream and passes it along
tokens nslobject::parse ( std::istream *instream )
{
	cparser newparser(instream);
	return parse(newparser);
}

// Parsefile: Creates a parser object from a file and passes it along
tokens nslobject::parse ( std::string infile )
{
	cparser newparser(infile);
	return parse(newparser);
}

// Parseparser: Starts the actual parsing of a parser object, and catches any parsing exceptions.
tokens nslobject::parse ( cparser &inparser )
{
	// If handed a zero-parser, just eject out of the function.
	/*if (inparser == 0)
		return;*/
	// TODO: Find ways of passing zero references and testing

	try {

		return parseparser(inparser);


	} catch(unexpected_newline) {
		cout << "\aError\t Unexpected newline at line " << inparser.getline() << " of stream " << inparser.name() << endl;
		throw parse_error();
	} catch(unexpected_eof) {
		cout << "\aError\t Unexpected EOF at line " << inparser.getline() << " of stream " << inparser.name() << endl;
		throw parse_error();
	} catch(unrecognised_token tokerr) {
		cout << "\aError\t Unrecognised token ";
		if (tokerr.what())
			cout << "\"" << tokerr.what() << "\" ";
		cout << "at line " << inparser.getline() << " of stream " << inparser.name() << endl;
		throw parse_error();
	} catch (unrecognised_objecttype objecterr) {
		cout << "\aError\t Unrecognised O-type ";
		if (objecterr.what())
			cout << "\"" << objecterr.what() << "\" ";
		cout << "at line " << inparser.getline() << " of stream " << inparser.name() << endl;
		throw parse_error();
	} catch (parse_error) {
		cout << "\aError\t whilst parsing " << inparser.name() << " line " << inparser.getline() << endl;
		throw;
	} catch (...) {
		cout << "\aAn unknown exception occured." << endl;
		throw;
	}
}

nslobject *nslobject::findbyname(std::string name)
{
	// Firstly, is the name we are searching for my name? If so return myself
	if (name == get("name"))
		return this;

	// Iterate through each child and ask them to find by name

	nslobject *retval;
	std::vector<nslobject *>::iterator it = subobjects.begin();
/*	while(it != subobjects.end())
	{
		retval = (*it)->findbyname(name);
		// If it returned a non-zero, then assume that it is a pointer and pass it back up
		if (retval)
			return retval;
	}*/
	for (unsigned int i = 0; i < subobjects.size(); i++)
	{
		retval = subobjects[i]->findbyname(name);
		if (retval)
			return retval;
	}

	return 0;
}

nslobject *nslobject::findbytype(std::string type, int maxsteps)
{
	// First check for the (unlikely) case that we are the type
	if (isoftype(type))//gettype() == type)
		return this;

	// Now let us search our children for this type, if one is then return it
	for (unsigned int i = 0; i < subobjects.size(); i++)
		if (subobjects[i]->isoftype(type))
			return subobjects[i];

	// If we have run out of steps, return zero
	if (!maxsteps)
		return 0;

	//Otherwise pass control up to our parent to look, if we have one (if not,
	//we have hit the end of the line and return 0 anyway
	if (parent)
		return parent->findbytype(type, maxsteps - 1);
	else
		return 0;
}

void nslobject::regenerate ( std::ostream &outs )
{
	static int level = 0;


	/*for(Iter i = shapes.begin(); i != shapes.end(); i++)*/

	// Indenterizing
	for (int i = 0; i < level; i++)
		outs << "    ";

	// Write the initialiser
	outs << "add " << gettype() << " {" << endl;

	// Increase the indentation
	level++;

	// Iterate through all the properties and dump them out
	std::map<std::string, std::string>::iterator iter;
	for (iter = properties.begin(); iter != properties.end(); iter++)
	{
		// Indenterizing
		for (int i = 0; i < level; i++)
			outs << "    ";

		outs << "set " << iter->first << " \"" << iter->second << "\"" << endl;
	}

	// Now loop through all subobjects
	std::vector<nslobject*>::iterator sublist;
	for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
		(*sublist)->regenerate(outs);

	//Reduce the indentation level
	level--;
	// Indenterizing
	for (int i = 0; i < level; i++)
		outs << "    ";
	outs << "}" << endl;
}

void nslobject::prepare ( void )
{
	// Run down the tree first
	std::vector<nslobject*>::iterator sublist;
	for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
		(*sublist)->prepare();

	// Now prepare us!
	prepareobject();
	readsettings();
	
	initialised = true;
}

void nslobject::run ( void )
{
	// Declare the iterator for the subobject list
	std::vector<nslobject*>::iterator sublist;

	// For running, run us first then go down the tree. Don't continue if it rturns false.
	if (runobject())
		for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
			(*sublist)->run();
}

//template <int T>

long double nslobject::getlongdouble( std::string propertyname, long double defaultvalue )
{
	try {
		// If there is no value to get, default
		if (!isset(propertyname))
			return defaultvalue;

		// Get the value
		string invalue = get(propertyname);

		// Convert the value to a long double
		std::istringstream oStream(invalue);
		long double dOutput = 0;
		oStream >> dOutput;
		
		return dOutput;

	} catch (...) {
		cout << "Error attempting to convert property " << propertyname
			<< " to long double on object " << gettype() << " [" << get("name") 
			<< "]" << endl;
		throw;
	}
}

int nslobject::getint( std::string propertyname, int defaultvalue )
{
	try {
		// if no value, dfault
		if (!isset(propertyname))
			return defaultvalue;
			
		// Grabb the value
		string inval = get(propertyname);
		
		//Convert this to an int
		std::istringstream oStream(inval);
		int  dOutput = 0;
		oStream >> dOutput;
		
		return dOutput;

	} catch (...) {
		cout << "Error attempting to convert property " << propertyname
			<< " to integer on object " << gettype() << " [" << get("name") 
			<< "]" << endl;
		throw;
	}
}


long nslobject::getlong (std::string propertyname, long defaultvalue)
{
	try {
		// If there is no value to get, default
		if (!isset(propertyname))
			return defaultvalue;

		// Get the value
		string invalue = get(propertyname);

		// Convert the value to a long double
		std::istringstream oStream(invalue);
		long dOutput = 0;
		oStream >> dOutput;
		
		return dOutput;

	} catch (...) {
		cout << "Error attempting to convert property " << propertyname
			<< " to long on object " << gettype() << " [" << get("name") 
			<< "]" << endl;
		throw;
	}
}

bool nslobject::isoftype(std::string type_name)
{
	if (find(types.begin(), types.end(), type_name) == types.end()  )
		return false;
	else
		return true;
}

int nslobject::countchildren(std::string type)
{
	// Declare the iterator for the subobject list
	std::vector<nslobject*>::iterator sublist;
	int runningcount = 0;

	for (sublist = subobjects.begin(); sublist != subobjects.end(); sublist++)
		if ((*sublist)->isoftype(type))
			runningcount++;
	return runningcount;
}

void nslobject::reset( void )
{
	readsettings();

	// No do the same for each of our children
	BOOST_FOREACH( nslobject *ob, subobjects )
		ob->reset();
	
//	prepare();
}
