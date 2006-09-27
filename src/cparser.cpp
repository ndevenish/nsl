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

// contains the code for a parser class

#include "cparser.h"
#include <string>
#include <cctype>
#include <iostream>
#include <fstream>
#include "errors.h"


using namespace std;
cparser::cparser( std::istream *inputstream )
{
	createdstream = false; // We did not create this stream... do not destroy it
	instream = inputstream; // make our stream the read-in stream
	// set a default name
	name("instream");
	initparser();
}

cparser::cparser( std::string inputfile )
{

	ifstream *loadfile = new ifstream(inputfile.c_str());
	if (!loadfile)
	{
		cout << "Error: Reading file \"" << inputfile << "\"" << endl;
		throw Error("File not found");
	}
	
	// We created the stream so set a flag saying we can destroy it
	createdstream = true;
	name(inputfile.c_str()); // name the stream
	instream = loadfile;
	
	initparser();
}

void cparser::initparser()
{
	linecount = 0;
	stringizer = '"';
	endoffile = 0;
	punctuation = ".+-_";
}

cparser::~cparser()
{
	if (createdstream)
	{
		delete (instream);
		instream = 0;
	}
}

/*char cparser::get()
{
	return (*instream).get();
}*/

std::string cparser::gettoken( void )
{
	int tokenlength = 0;
	char inchar, nextchar;
	std::string currenttoken;
	
	//Firstly strim off any whitespace at the beginning of the stream
	while (strchr(" \t", nextchar =((*instream).peek())))
		(*instream).ignore();

	//Is the next character the stringizer?
	if (stringizer && (nextchar == stringizer))
	{
		//ignore it as we know it is there
		(*instream).ignore();
		//Let's grab a string - this cannot span multiple lines
		while (nextchar =((*instream).peek()))
		{
			if (isreturn(nextchar))
			{
				cout << "Error reading line " << linecount << " - stringization hit end of line" << endl;
				break;
			}

			(*instream).ignore();
			if (nextchar == stringizer)
				break;
			currenttoken += nextchar;
		}

		return currenttoken;
	}

	// Let us now grab a normal token
	// Now grab characters until we have the first non-alphanumerical
	do {
		inchar = (*instream).get();
		//nextchar = 
		
		// If it is a return, increase the linecount and convert it to a 
		// \n for compatability
		if (isreturn(inchar))
		{
			linecount++;
			inchar = '\n';
		}
			
		currenttoken += inchar;
		tokenlength++;
	// Loop while this and the next character is valid
	} while (isvalnum(inchar) && isvalnum((*instream).peek()));


	// If the token is an EOF, set the EOF flag now
	if (inchar == EOF)
		endoffile = true;

	// Force all newline returns to be \n to avoid messiness
/*	if (isreturn(inchar))
		inchar = '\n'; */
	return currenttoken;

}


cparser& operator>>(cparser& p, std::string& str)
{
	str = p.gettoken();
	return p;
}

std::string cparser::name( const char* newname )
{
	if (newname != 0)
		streamname = newname;
	return streamname;
}


// Tests to see if a character is a valid input one. To replace
// isalnum() to accomodate punctuation.
bool cparser::isvalnum(char inchar)
{
	//if (inchar == '.')
		//cout << "[.]";
	// Test for various cases, return true if they apply otherwise return false.
	
	// Firstly, is it alphanumeric?
	if (isalnum(inchar))
		return true;

	// Secondly, is it punctuation?
	if (punctuation.find(inchar) != string::npos)
		return true;

	return false;
}