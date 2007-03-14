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
function [ header ] = oldneutron_getheader( filename )
%getheader  REads a data file and returns a struct containing all of the
%pair values in the header
%
% This function only reads old style headers

%filename = '../results/dip_3d_diff_mercury.txt';

%Check the file is valid
fid = fopen(filename);
if (fid < 0)
    header = fid;
    return;
end

%Scan until we get to the token 'Initial Settings'
lookingforsettings = 1;
linecount = 0;
while(lookingforsettings)
    linne = fgetl(fid);
    linecount = linecount + 1;
    while( 1 )
        [tok linne] = strtok(linne, [':' char(9) char(13) char(10)]);
        if (isempty(tok))
            break;
        end
        if strcmpi(tok, 'Initial settings')
            lookingforsettings = 0;
            break;
        end
    end
end

%We shall assume that the next two lines are the header length
linne = fgetl(fid); linecount = linecount+1;
header.property{1} = strtok(linne, [':' char(9) char(13) char(10)]);
linne = fgetl(fid); linecount = linecount+1;
header.value{1} = strtok(linne, [':' char(9) char(13) char(10)]);
headerlength = str2double(header.value{1});

%Now read pairs in for the rest of this header, minus one line
for i=2:(headerlength-linecount)
    linne = fgetl(fid);
    if strfind(linne, ':'),
        [header.property{i} linne] = strtok(strtrim(linne), [':' char(9) char(13) char(10)]);
        [invalue linne] = strtok(strtrim(linne), [': ' char(9) char(13) char(10)]);
        %sscanf(b, '%f'
        [innumber, count] = sscanf(invalue, '%f');
        if (count)
            invalue = innumber;
        end
        header.value{i} = invalue;
    end
end


%Now read the column headings
%Now let's read in the column headers
headers = fgetl(fid);
column_headers = {};
count = 0;
%Now process these and output an array of them
while 1
    [curtok, headers] = strtok(headers, char(9));
    if(isempty(curtok)),
        break;
    end
    count = count + 1;
    header.column_headers{count} = curtok;
end
