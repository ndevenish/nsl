% Copyright (c) 2006-2012 Nicholas Devenish
% 
% Permission is hereby granted, free of charge, to any person obtaining a copy of
% this software and associated documentation files (the "Software"), to deal in
% the Software without restriction, including without limitation the rights to
% use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
% the Software, and to permit persons to whom the Software is furnished to do so,
% subject to the following conditions:
% 
% The above copyright notice and this permission notice shall be included in all
% copies or substantial portions of the Software.
% 
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
% FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
% COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
% IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
% CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
% 
function [version] = fileversion( filename )
%FILEVERSION     Reads the specified file and returns what program version
%it is.
%           Returns: 0 - Invalid file
%                    1 - Old neutron data file
%                    2 - nsl neutron Data file
%                    3 - Preprocessed data file

%Check the file is valid
fid = fopen(filename);
if (fid < 0)
    version = 0;
    return;
end

%split up the file
[path, name, ext] = fileparts(filename);

% Do a very simple test for now - read the first line and see whether it
% contains a #
line = fgetl(fid);

regmatch = regexp(line, '^\s*[#]');

if (strfind(ext, 'ppf'))
    version = 3;
elseif (strfind(ext, 'txt')),
    if (regmatch),
        version = 2;
    else
        version = 1;
    end
end


fclose(fid);