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
function [xdata xerror xerrcolumn] = getdata(xaxis_name, curset)
    %Gets a specific set of data from a data set
    %curset = dataset;
    
    %Get the name of the x and y axis
    yaxis = xaxis_name;
    
    ycolumn = 0; yerrcolumn = 0;
    %Find the columns for this data set
    for j = 1:max(size(curset.headers)),
        if strcmpi(yaxis, curset.headers{j}),
            ycolumn = j;
            %Is the next column an error column?
            if (j ~= max(size(curset.headers)))
                %Is there an error set for this?
                if ( strcmp(curset.headers{j+1}, 'uncert') ),
                    %Get the error set for this value
                    yerrcolumn = j + 1;
                end
            end
        end
    end
    %Check that xcolumn and ycolumn are set
    if ( (ycolumn) == 0 ),
        error ('Column not found in the data set');
    end

    %Gather the x and y data
    xdata = curset.data(:,ycolumn); 

    if(yerrcolumn)
        xerror = curset.data(:,yerrcolumn);
    else
        xerror = xdata .* 0;
    end
    
    xerrcolumn = yerrcolumn;