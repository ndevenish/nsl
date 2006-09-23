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
#include <iostream>

#include "nslobjectfactory.h"
#include "nslobject.h"

using namespace std;

map<string, nslobjectfactory*> nslobjectfactory::factories;

// Singleton to initialize the ShapeFactory:
class nslobjectfactoryinitializer {
  static nslobjectfactoryinitializer si;
  nslobjectfactoryinitializer() {
    nslobjectfactory::factories["nslobject"]= new nslobject::Factory;
    //ShapeFactory::factories["Square"]= new Square::Factory;
  }
  ~nslobjectfactoryinitializer() {
    map<string, nslobjectfactory*>::iterator it =
      nslobjectfactory::factories.begin();
    while(it != nslobjectfactory::factories.end())
      delete it++->second;
  }
};
 
// Static member definition:
nslobjectfactoryinitializer nslobjectfactoryinitializer::si;

void nslobjectfactory::list ( void ) {
	int count = 0;
	std::map<std::string, nslobjectfactory*>::iterator facit;
	cout << "List of recognized object types:" << endl;
	for (facit = factories.begin(); facit != factories.end(); facit++)
	{
		cout << "\t" << count++ << "\t" << facit->first << endl;
	};
}

void nslobjectfactory::add( std::string newob, nslobjectfactory* newfac)
{
	// check to see if it already exists
	std::map<std::string, nslobjectfactory*>::iterator facit;
	facit = factories.find(newob);
	if (facit == factories.end())
		factories[newob]= newfac;
}

void nslobjectfactory::remove ( std::string objet )
{
	std::map<std::string, nslobjectfactory*>::iterator facit;
	facit = factories.find(objet);
	if (facit != factories.end())
	{
		// Delete the pointer
		delete facit->second;
		facit->second = 0;
		factories.erase(facit);
	}
}

nslobject* nslobjectfactory::createobject(const std::string& id)
{
	nslobject *creation;
	if(factories.find(id) != factories.end())
	  creation = factories[id]->create();
	else
	  throw unrecognised_objecttype(id);
	creation->candelete(true);
	return creation;
}