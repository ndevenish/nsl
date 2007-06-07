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


#include "variable.h"

namespace nsl {
	
variable variable::operator+=(const variable& r)
{
	// Combine two variables!
	value += r.value;
	error = sqrt(error*error + r.error*r.error);
	return *this;
}

variable variable::operator-=(const variable& r)
{
	value -= r.value;
	error = sqrt(error*error + r.error*r.error);
	
	return *this;	
}

variable variable::operator+=(const long double& r)
{
	value += r;
	return *this;
}


variable variable::operator-=(const long double& r)
{
	value -= r;	
	return *this;
}

variable variable::operator*=(const long double& r)
{
	value *= r;
	error *= r;
	return *this;
}

variable variable::operator/=(const long double& r)
{
	value /= r;
	error /= r;
	return *this;
}

variable variable::operator*=(const variable& r)
{
	// computer part of the error to update without introducing a median variable
	error = sqrt( powl( error / value, 2.) + powl( r.error / r.value, 2.0) );
	value *= r.value;
	error *= value;
	return *this;
}

variable variable::operator/=(const variable& r)
{
	error = sqrt( powl( error / value, 2.) + powl( r.error / r.value, 2.0) );
	value /= r.value;
	error *= value;
	return *this;
}

};


nsl::variable operator-(const long double &lt, const nsl::variable&rt)
{
	return nsl::variable(lt - rt.value, rt.error);
}

