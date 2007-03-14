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
function [ header ] = getheader( filename )
%getheader  REads a data file and returns a struct containing all of the
%pair values in the header
%
% NOTE: This function will read both old neutron and nslneutron data files.

%Test - is this an old style neutron data file? If so direct to a different
%function
if (fileversion(filename) == 1),
    header = oldneutron_getheader( filename );
    return;
end

%Check the file is valid
fid = fopen(filename);
if (fid < 0)
    header = fid;
    return;
end
    
%Scan until we get to the token 'Initial Settings'
linecount = 0;

% Assume that the first line gives the header, for what type it is
linne = fgetl(fid);
header.property{1} = 'Header';
header.value{1} = strtrim(strtok(linne, ['#']));


%Now read the column headings

% Find the last line starting with # - this will be the headings
%Test for commented: regexp(string, '^\s*s[#]')
while 1
    newline = fgetl(fid);
    if newline == -1,
        break;
    end
    %Does this new line match? If so, test the next line
    a = regexp(newline, '^\s*[#]')
    if (a),
        linne = newline;
        continue;
    end
    break;
end
%linne now contains a list of headers
headers = linne;
%Now let's read in the column headers
column_headers = {};
count = 0;
%Now process these and output an array of them
while 1
    [curtok, headers] = strtok(headers, [' ', char(9), '    ', '#']);
    if(isempty(curtok)),
        break;
    end
    count = count + 1;
    header.column_headers{count} = curtok;
end

fclose(fid);