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

/* TO ADD NEW CLASSES TO THE FACTORY:
	include nslobjectfactory.h
	Include the following in the new class:
		class Factory : public nslobjectfactory {
			nslobject *create() { return new <your object type>; }
		};
	Place the following code somewhere:
		nslobjectfactory::add("<object name>", new <object type>::Factory);

*/


#ifndef NSLOBJECTFACTORY_H
#define NSLOBJECTFACTORY_H

#include <string>
#include <map>
#include "errors.h"

class nslobject;
//class nslobjectfactoryinitializer;

/** The object factory class to create objects from their
* associated string names.
* Each class that derived from nslobject should define a subclass
* that inherits from this class. The virtual create() function should
* then be overloaded to return a pointer to the new object. Then add()
* should be called to add it to the list.
*
* createobject() can then be used to retrieve a pointer to a new instance of
* that class.*/
class nslobjectfactory {
//	friend class nslobjectfactoryinitialiser;

	/** Virtual function to overload, to return a new object.
	* This function shold be overloaded in new classes to return a
	* pointer to that class.
	* @return a pointer to a new object of the inherited class's type.*/
	virtual nslobject* create() = 0;
	static std::map<std::string, nslobjectfactory*> factories; ///< A list of names and associated factory objects
	
public:
	virtual ~nslobjectfactory() {}
	friend class nslobjectfactoryinitializer;
	/*class BadShapeCreation : public logic_error {
	public:
	BadShapeCreation(string type)
	: logic_error("Cannot create type " + type) {}
	};*/

	/** Creates an object from it's string description.
	* As long as an nslobjectfactory object has been associated with a string via the
	* add() function, this will return a pointer to a new object type of that specified.
	* It throws an exception if the object type is unrecognised.
	* @param id The string ID of the classtype that is wished to be returned.
	* @return A pointer to a new nslobject (or derivative).
	* @see add(), create() */
	static nslobject* createobject(const std::string& id);

	/** Adds an association between string ID and a class factory. */
	static void add( std::string newob, nslobjectfactory* newfac);

	/// Removed a specified string ID and associated factory from the list.
	static void remove ( std::string objet );

	/// Prints a list of recognised (string) object type names.
	static void list ( void );

	/// Returns the number of objects recognised.
	static int count ( void ) { return (int)factories.size(); }

		/*for ( i = factories.begin(); i != factories.end();) {
			if (i->first = object) {
				i = inventory.erase(i);
			} else {
				++i;
			}*/
};


#endif