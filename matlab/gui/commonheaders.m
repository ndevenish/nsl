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
function [ common_headers ] = commonheaders( headerset1, headerset2 )
%commonheaders - takes two string-cell arrays and returns a cell
%               array with the common headers.
%               Also removes 'uncert' headers
common = 0;
set1size = prod(size(headerset1));
set2size = prod(size(headerset2));

common_headers = [];
%Loop through each entry in headerset1, and if there is an identical one in
%set 2 then add it to our list of headers
for i=1:set1size,
    string1 = headerset1{i};
    for j=1:set2size,
        string2 = headerset2{j};
        if(strcmpi(string1, string2)),
            if (strcmpi(string1, 'uncert') == 0)
                common = common+1;
                common_headers{common} = headerset1{i};
                break;
            end
        end
    end
end