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

#ifndef ERRORS_H
#define ERRORS_H

//#include <sstream>
#include <stdexcept>

class Warning {
public:
	Warning(std::string);
	Warning(std::stringstream);
};

class Error : public std::exception {
public:
//	const char* const message;

	//Error(const char* const msg = "") : message(msg) {}
	Error(std::string msg = "") : std::exception(msg.c_str()) {}
};

class parse_error : public std::runtime_error {
public:
	//parse_error(const char* const msg = 0) : std::runtime_error(msg) {}
	parse_error(const std::string& msg = "") : std::runtime_error(msg) {}
	//parse_error(const std::exception msg) : std::logic_error(msg.what()) {}
};

class syntax_error : public parse_error {
	public:
		syntax_error(const std::string& msg = "") : parse_error(msg) {}
};

class unexpected_newline : public parse_error{
public:
	unexpected_newline(const std::string& msg = "") : parse_error(msg) {}
};
class unexpected_eof : public parse_error {
public:
	unexpected_eof(const std::string& msg = "") : parse_error(msg) {}
};

class unrecognised_token : public parse_error{
public:
	unrecognised_token(const std::string& msg = "") : parse_error(msg) {}
};

class unrecognised_objecttype : public parse_error{
public:
	unrecognised_objecttype(const std::string& msg = "") : parse_error(msg) {}
};


#endif