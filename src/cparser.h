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

// contains the definitions for a parser class
#ifndef CPARSER_H
#define CPARSER_H

//class string;
#include <string>
#include <sstream>

#include <iosfwd> // Forward references to stream classes
class cparser;

cparser& operator>>(cparser& p, std::string &str);

/** Parses a stream into individual tokens */
class cparser
{
//	friend cparser& operator>>(cparser& p, std::string &str);
public:
	/** Initialise a cparser object from an existing input stream.
	* @param inputstream Pointer to an existing input stream to associate with the parser. */
	cparser( std::istream *inputstream );
	/** Initialise a cparser object by opening a file.
	* @param inputfile A string with the location and name of the file to parse. */
	cparser( std::string inputfile );
	//cparser( std::istringstream *inputstream );
	
	~cparser();

//	char get( void );	// Retrieves a single character from the stream
	/** Retrieves a token from the stream */
	std::string gettoken( void ); //Retrieves a token
	
	/* Returns a whole code block.
	* Returns a whole code block, starting with opener and ending with closed. If
	* the block is unbalanced (i.e. closed without an associated opener) it will return anwyay. */
	std::string getblock( std::string opener = "{", std::string closer = "}");
	
	/** Returns the current linenumber the stream is parsing on. */
	int getline() { return linecount; }

	/** Check to see if the stream has hit the end of the file. */
	bool eof() { return endoffile; }	// returns the EOF case (has it hit end of file)
	
	/** The stringization character.
	* If this character is encountered, then all input up until this character is encountered
	* again will be read into the same token, whitespace or no. Strings cannot spread over
	* multiple lines. */
	char stringizer;	// The character that causes stringization
	/// A string of characters to consider as alphanumeric for token purposes.
	std::string punctuation; // characters to ignore as non-alphanumeric

	/** Sets and retrieves the stream name.
	* @param newname The new name to give the stream.
	* @return The name of the stream */
	std::string name(const char *newname = 0);

private:

	/// Initialise the values for the cparser object - called by both constructors
	void initparser();

	bool endoffile; ///< Flag to record whether EOF has been encountered
	bool createdstream; ///< Flag to record if the stream was object created (and so can be destroyed)

	long linecount; ///< Keeps track of the current linenumber
	std::string streamname; ///< The name of the stream
	std::istream *instream; ///< Pointer to the input stream to read from

	/** Tests to see if a character is a valid input one.
	* This is to replace isalnum() to accomodate cparser::punctuation. */
	bool isvalnum(char inchar);

};

inline const bool isreturn ( const char token )
{
	if ((token == '\r') || (token == '\n'))
		return true;
	return false;
}
#endif
