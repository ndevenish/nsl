

NSL - Neutron Scripting Language
================================

> A simulation of neutron resonance and polarisation  
> by **Nicholas Devenish**.

Overview
--------

This project was written as part of a masters thesis investigating the polarization over time of ultra-cold neutrons inside a storage bottle, and the resonance effects involved with stray magnetic fields (specifically, the presence of a magnetic dipole).

Previously, a custom piece of software written by the professor was used. This was written in plain c, completely configured by compilation of header files which made it difficult to reliably reproduce results and settings by the fact that the program needed to be modified for every configuration. This implementation aimed to avoid that issue by creating a configuration file format "NSL", standing for **N** eutron **S** cripting **L** anguage. Examples of these script files can be found in `scripts/*.nsl`, and documentation of the file format properties can be found in `doc/nslprops.txt`.

*Disclaimer: This was written quite some time ago, when I was certainly not as well versed with programming practices as I am now. I therefore take no responsibility for awful code that may be contained therein. Certainly, I learnt an awful lot about C++ whilst writing this project.*

Requirements
------------
Several pieces of software are required to build this package:

* `Boost 1.34` (minimum) is required for the `thread`, `filesystem`, `system` and `program_options` modules. It may be possible to rewrite a couple of these dependencies, but threading is pretty ingrained.
* `CMake 2.6` is used to generate the makefiles, to build the program.
* (Optionally) `Doxygen` to build the documentation.

Building
--------

To build, you can follow the standard CMake prescription for an out-of-source build:

    $ mkdir build
    $ cd build
    $ cmake ..
    ... CMake output ...
    $ make

And then the binaries `neutron` and `nsltest` will be deposited in the `build/src` subdirectory. By default, this will also build the class documentation, if `Doxygen` is installed.

License
-------
This software is released under the MIT licence, notice seen below. If this isn't acceptable for you, I am happy to discuss licencing on a case-by-case basis.

> Copyright (c) 2006-2012 Nicholas Devenish
> 
> Permission is hereby granted, free of charge, to any person obtaining a copy of
> this software and associated documentation files (the "Software"), to deal in
> the Software without restriction, including without limitation the rights to
> use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
> the Software, and to permit persons to whom the Software is furnished to do so,
> subject to the following conditions:
> 
> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.
> 
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
> FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
> COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
> IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
> CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
