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
function value = getproperty(dataset, propertyname)
%GETPROPERTY scans a header array and returns the value for the property,
%or empty if the property does not exist
    if ~isfield(dataset.header, 'property'),
        error('Incorrect structure format (no property)');
    end
    if ~isfield(dataset.header, 'value'),
        error('Incorrect structure format (no value)');
    end
    
    value = [];
    for i=1:numel(dataset.header.property)
        if strcmpi(dataset.header.property{i}, propertyname)
            value = dataset.header.value{i};
        end
    end