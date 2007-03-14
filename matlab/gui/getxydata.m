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
function [xaxis, yaxis, xdata, ydata, yerror, yerrcolumn] = getxydata(handles, dataset)
    curset = handles.datasets.sets{dataset};
    
    %Get the name of the x and y axis
    strings = get(handles.xaxis_list, 'String');
    xaxis = strings{get(handles.xaxis_list, 'Value')};
    yaxis = strings{get(handles.yaxis_list, 'Value')};

%     xcolumn = 0; ycolumn = 0; yerrcolumn = 0;
%     %Find the x and y columns for this data set
%     for j = 1:max(size(curset.headers)),
%         if strcmp(xaxis, curset.headers{j}),
%             xcolumn = j;
%         end
%         if strcmp(yaxis, curset.headers{j}),
%             ycolumn = j;
%             %Is the next column an error column?
%             if (j ~= max(size(curset.headers)))
%                 %Is there an error set for this?
%                 if ( strcmp(curset.headers{j+1}, 'uncert') ),
%                     %Get the error set for this value
%                     yerrcolumn = j + 1;
%                 end
%             end
%         end
%     end
%     %Check that xcolumn and ycolumn are set
%     if ( (xcolumn * ycolumn) == 0 ),
%         error ('Neither x nor y column has been found in the data set');
%     end
% 
%     %Gather the x and y data
%     xdata = curset.data(:,xcolumn);
%     ydata = curset.data(:,ycolumn); 
% 
%     if(yerrcolumn)
%         yerror = curset.data(:,yerrcolumn);
%     else
%         yerror = ydata .* 0;
%     end

    [xdata] = getdata(xaxis, curset);
    [ydata yerror yerrcolumn] = getdata(yaxis, curset);
    %Are we supposed to reduce this?
    if handles.datasets.reduce
        %Which is larger
        if (numel(ydata(ydata>0)) > numel(ydata(ydata<0)))
            %More positive
            xdata = xdata(ydata>0);
            if(yerrcolumn)
                yerror = yerror(ydata>0);
            end
            ydata = ydata(ydata>0);
        else
            %More negative
            xdata = xdata(ydata<0);
            if(yerrcolumn)
                yerror = yerror(ydata<0);
            end
            ydata = ydata(ydata<0);
        end
    end

    %Check for limiting left or right
    if handles.datasets.limitleft
        ydata = ydata(xdata>handles.datasets.leftlimit);
        %if isempty(ydata)
        %    return;
        %end
        if(yerrcolumn)
            yerror = yerror(xdata>handles.datasets.leftlimit);
        end
        xdata = xdata(xdata>handles.datasets.leftlimit);
    end
    if handles.datasets.limitright
        ydata = ydata(xdata<handles.datasets.rightlimit);
        %if isempty(ydata)
        %    return;
        %end
        if(yerrcolumn)
            yerror = yerror(xdata<handles.datasets.rightlimit);
        end
        xdata = xdata(xdata<handles.datasets.rightlimit);
    end