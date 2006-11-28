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

#ifndef NSLOBJECT_H
#define NSLOBJECT_H

#include <iosfwd>
#include <string.h>
#include <map>
#include <vector>

#include "nslobjectfactory.h"
#include "errors.h"


class cparser;

/** A list of command tokens.
* These tokens whatever blah */
enum tokens {
	token_eof,
	token_stop,
	token_add,
	token_load,
	token_set,
	token_closebrace
};

/** The base class from which all objects should be derived from. */
class nslobject {
public:
	nslobject();
	virtual ~nslobject();

	////////////////////////////////////////////////
	// Functions to set and get properties
	
	/** Checks to see if a property has been set.
	* @param propertyname The property to check
	* @return True or false
	* @see set(), get() */
	bool isset( std::string propertyname );
	
	/** Sets a property value.
	* @param propertyname The name of the property to set
	* @param value The (string) value to set the property to
	* @see get(), isset()*/
	void set ( std::string propertyname, std::string value );
	
	/** Retrieves a property value.
	* This function retrieves a string-property from the objects property list. If the
	* property has not been set, it returns a default value.
	* @param propertyname The name of the property to retrieve
	* @param defaultval The value to return in the event that the property has not been set.
	*                   See isset() to check this.
	* @return string containing either the property value, or the default.
	* @see getlongdouble() to extract a number from a propery. */
	std::string get ( std::string propertyname, std::string defaultval = "" );
	
	/** Reads a real number from a property.
	* This function reads a specified property value, attempts to convert it to a number
	* and returns the result. In the event the property is not set, it returns a specified
	* default value.
	* @param propertyname The name of the property to retrieve
	* @param defaultvalue The default value in the even the property is not set
	* @return The property converted to a number (or the default)
	* @see get() to retrieve the string value of a property */
	long double getlongdouble( std::string propertyname, long double defaultvalue );

	/** Reads an integer from a property
	* See getlongdouble() */
	int getint( std::string propertyname, int defaultvalue );
	/** Reads a long from a property
	* See getlongdouble() */
	long getlong (std::string propertyname, long defaultvalue);
	
	/** Ensures a property has been set.
	* If the specified property has not been set, this function throws a runtime_error
	* exception. Note that it does not return the value, only checks it exists.
	* @param requiredproperty The property to ensure is set
	* @see set(), isset() */
	void require( std::string requiredproperty );

	/** Adds a child to the objects family tree.
	* This function adds a child to the internal list of children. It also sets the 
	* current object as the childs parent.
	* @param newobject A reference to the object to attach */
	void add ( nslobject & newobject);
	
	/** Retrieve the object type.
	* @return A string of the objecttype, as set by the derived class
	* @see objecttype */
	std::string gettype() { return objecttype; };

	/** Checks to see if an object is, or has been of a specified type.
	* This function checks the class history to see if it is derived from the
	* specified class type.
	* It is the responsibility of each derived class to add itself to the list of
	* classes, by pushing back onto the nslobject::types vector.
	* @param type_name The classname to check
	* @return True or false */
	bool isoftype(std::string type_name);

	/** Prints an object tree.
	* This function prints a representation of the object tree to STDOUT. It does
	* this by recursing itself.
	* @param level The level of indentation, this increases with each recursion. */
	void tree(int level = 1);

	/** Sets the parents object.
	* @param newparent The object to use as a parent */
	void setparent (nslobject &newparent) { parent = &newparent; };

	/** Searches the object tree (downwards) for a name.
	* This will firstly check the current object to check for a name match, then
	* will call all of it's subobjects which will do the same. 
	* @param name The name to search for
	* @return A pointer to the object with desired name, else 0 if there is none. */
	nslobject *findbyname(std::string name);

	/** Searches the tree (upwards) for a specific object type.
	* This function will search the object tree upwards for an object of particular type. It
	* will Firstly check itself (and, not, it's own children) then calls it's parent object
	* which will do the same check.
	* It uses the isoftype() function, so objects derived from the desired classtype will
	* satisfy the search criteria and be returned.
	* @param type The object type to look for
	* @param maxsteps The maximum number of steps upwards to look. Set to a negative number
	*                 for infinite searching.
	* @return A pointer to the first object of desired type encountered, else 0.
	* @see isoftype() */
	nslobject *findbytype(std::string type, int maxsteps = -1);

	/** Counts the number of child ofjects of specified type.
	* This function searches only the children of the current object, and counts
	* the number of them with a certain type (as reported by isoftype()). */
	int countchildren(std::string type);

	////////////////////////////////////////////////////////////////////////////////
	// Three overloaded functions to initiate parsing from various sources.
	// Those that are not the cparser form call the cparser form (after making
	// a cparser)
	
	/** Parses an object tree from an arbitrary input stream.
	* This function creates a cparser object and hands off control to the
	* parse(cparser&) form of the function.
	* @param instream A pointer to an input stream to read from.
	* @see parse(std::string), parse(cparser&) */
	tokens parse ( std::istream *instream );
	
	/** Parses an object tree from an input file.
	* This function creates a cparser object to read from the specified input file and
	* then hands off control to the parse(cparser&) overloaded form of the function.
	* @param infile A pointer to a file to open and parse
	* @see parse(std::istream), parse(cparser&) */
	tokens parse ( std::string infile );

	/** Parses an object tree from a parser object.
	* This function initiates parsing an object tree from a cparser object. All the parsing
	* is handled by the parseparser() function, however this function handles any errors that
	* may arise from parsing.
	* This approach ensures that errors are handled correctly and that appropriate information
	* can be handed out when they happen. 
	* @param inparser Reference to an initialised parser object from which to read tokens from.
	* @return An enum indicating the last type of command token retrieved. */
	tokens parse ( cparser & inparser );

	/** Generates the script required to duplicate the object tree.
	* This function iterates through itself and all subobjects and sends to the
	* specified output stream a generation of the script that would recreate the tree
	* exactly.
	* @param outstream An output stream to send the generated script to, i.e. cout */
	// Function to 'regenerate' the object tree
	void regenerate ( std::ostream &outstream );

	//////////////////////////////////////////////////////////////////////////////////
	// Two 'active' function - the first is to prepare for running (initialising data
	// members etc) and the second is for actually running the chain.

	/** Prepares all objects in the object tree.
	* This function initialises the whole object tree. It first calls the prepare function
	* for all it's children, then calls it's own internal prepareobject() function. */
	void prepare ( void );

	/** Runs all objects in the object tree.
	* This function firstly calls the current object's own runobject() function, then if
	* runobject() returns true it continues calling run() for all of it's children. Through
	* this process every object in the tree is run (unless the process down a particular
	* branch is halted). */
	void run ( void );

	/** Check if we can safely delete this object.
	* If this object has been created at runtime, it should have been set as
	* deleteable by the candelete( bool ) function. If it has, then we can safely
	* delete this class without causing a memory violation.
	* @return True or false indicating if it is safe to delete the current object*/
	bool candelete( void ) { return candeletethis; }

	/** Set whether we can safely delete this object.
	* In the destructor ~nslobject(), all child objects are deleted. If the
	* object has not been created dynamically then this can cause a violation.
	* Therefore, there is an internal flag that if set, the object will not be
	* deleted. This flag defaults to false (the object cannot be deleted) and can
	* be changed by calling this function.
	* @param cd True if the object can safely be delete automatically, false otherwise */
	void candelete( bool cd ) { candeletethis = cd; }

	/** Resets the class back to it's initial state.
	* This function resets the class back to it's initial state, by recalling prepareobject().
	* This can work because prepareobject() should only base it's initial configuration off of
	* the internal parameter variables. */
	void reset ( void );
	
	// The local instance of the object factory
	class Factory : public nslobjectfactory {
		nslobject *create() { return new nslobject; }
	};
protected:
	// String to hold a name for the type of object - protected so that
	// inherited classes can alter it directly
	/** String to hold the name for the type of object.
	* Protected so that inherited classes can alter it directly (and then add it to the
	* nslobject::types vector) */
	std::string objecttype;

	/** Vector to hold list of types.
	* This vector holds a list of object types that the current object is and is derived
	* from. It is used by the isoftype() function to determine if an object is based on another.
	* Every derived object contructor should, after assigning the nslobject::objecttype
	* variable, add it to this list.*/
	std::vector<std::string> types; // A list of the objects types
	bool initialised; ///< Flag to remember if prepare() has been called for this object yet

	// Two virtual functions for the prepare and run. Each returns true to continue down
	// this path of the chain.

	/** Sets up the object for running.
	* Setting properties using set() (or an input script file) is only part of initialisation -
	* in order to ensure that when running time isn't spent converting strings to numbers every
	* time one is needed, each derived class is given a chance to preload all values into
	* appropriate variables. This is done inside of this function, as is any other preperation.
	*
	* Note that this should only base off of parameter values and not precalculated variables as
	* the class may be called to reset() at some point.
	*
	* It is a virtual function, so any deriving class should implement one of it's own.
	* @returns A bool that currently represents nothing.*/
	virtual bool prepareobject ( void ) { return true; }
	
	/** Runs an object.
	* This virtual function is called when the object tree is run.
	* @return Returns true to continue running subobjects, otherwise false to fall back
	* to the parent run function. */
	virtual bool runobject ( void ) { return true; }

	/** A vector list of the object's children */
	std::vector<nslobject *> subobjects; // A list of the object's 'children

	/** An output object for logging */
	std::ostream &logger;
	
	/** Creates an object for adding. */
	nslobject *createadd(cparser &parser);
	
private:
	// Variables
	
	/** A virtual function to initialise values.
	* This is in order so that the class can be properly reset. */
	//virtual void initvals() {};
	
	/** Virtual function to read the settings in.
	* This should read in any settings from the parameters and set them - it should not
	* do any other initialisation. */
	virtual void readsettings ( void ) { } 
	
	std::map<std::string, std::string> properties; ///< A list of object 'properties'
	//std::vector<nslobject *> subobjects; // A list of the object's 'children

	nslobject *parent;	///< A pointer to the objects parent

	// A map for string types to object creation routines
	//static std::map<std::string, nslobject (nslobject::*)(std::string)> creationfuncs;
	
	/** Actually parses the stream from a cparser object.
	* This function is responsible for iterating through the tokens retrieved from
	* a cparser object. Any unidentified tokens are checked with the processtoken() function
	* prior to throwing a parsing error exception.
	* @param instream The cparser object to read tokens from.
	* @see processtoken() */
	tokens parseparser ( cparser &instream );

	/// loads a file and parses it by calling parse(cparser&)
	//- note: may be redundant now
	void load(std::string filename);

	/** Function to attempt identification of unidentified tokens. 
	* Virtual function to try and process tokens that are unrecognised - any tokens that
	* parseparser() reads that are not understood are first passed to this function, before
	* and errors are thrown. This enables subobjects to implement their own object-specific
	* command tokens.
	* @param token The unidentified token
	* @param tokenstream the cparser object to continue reading from if the token is recognised.
	* @return True if the token was identified and processed, otherwise false if it remains unrecognised. */
	virtual bool processtoken ( std::string token, cparser &tokenstream ) { return false; }

	// A bool to say whether or not this is a deleteable object
	bool candeletethis; ///< The internal flag used by candelete() and candelete(bool)

	/// Counts the number of variations being enacted
	static int variationcount;
	/// The number of the current run
	static int runid;
};

#endif

