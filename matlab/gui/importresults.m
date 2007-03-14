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
function [outputarray] = importresults( filename ) %, outputfile )

fid=fopen(filename);

filever = fileversion(filename);
if (filever == 1),
    tline = fgetl(fid); tline = fgetl(fid); tline = fgetl(fid);
    tline = fgetl(fid); tline = fgetl(fid);
    fclose(fid);
    headerlines = str2double(tline);
    %fprintf('Reading %d header lines\n', headerlines)

    outputarray = textread( filename,'','delimiter', '\t', 'headerlines', headerlines);
elseif (filever == 2),
    outputarray = textread( filename, '', 'commentstyle', 'shell');
end