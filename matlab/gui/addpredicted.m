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
function output = addpredicted( handles )
%Adds a basic non-adiabatic line to a plot

radius = getproperty(handles.datasets.sets{handles.selection}, 'Bottle Radius');
dbdz = getproperty(handles.datasets.sets{handles.selection}, 'dBdz');

%Fill in values...
J = 0.5; %spin
hbar = 1.0546e-34;
gammahg = 7.5901309E+6
gamman = -29.1646870E+6 %(-)
%#define GAMMA_HG	7.5901309E+6
%#define GAMMA_N		-29.1646870E+6
gammasq = gammahg*gamman * 4 * pi^2;
csq = 2.99e8^2;

df = 1/4 * (J*hbar) * gammasq * radius^2 * (1/csq) * dbdz;

%Now convert this to the proper units
df = df / 1.6e-19; %to e m
df = df / 1e-2;

df

[xaxis, yaxis, xdata, ydata, yerror, yerrcolumn] = getxydata(handles, handles.selection);

curset = handles.datasets.sets{handles.selection};
%Now make a data set with this
    newdataset.data = [xdata, (xdata * 0)+df*1e26];
    
    %Let's start constructing a new data set to contain our information. It
    %will have three columns of data.
    newdataset.filename = curset.filename;
    newdataset.pathname = '';
    newdataset.name = ['Predicted non-adiabatic false dipole'];
    newdataset.order = handles.datasets.order + 1;
    newdataset.ticktype = 0;
    newdataset.linetype = 2;
    newdataset.errorbars = 0;
    newdataset.colour = curset.colour;

    %Now start filling the data-related values
    newdataset.headers = {xaxis yaxis};
        
    %Fill the 'header' information
    newdataset.header.column_headers = newdataset.headers;
    newdataset.header.property{1} = 'False Dipole';
        newdataset.header.value{1} = df;
    newdataset.header.property{2} = '---BELOW ARE ORIGINAL---';
        newdataset.header.value{2} = 0;
    %Now copy over all the old header values
    for i=1:numel(curset.header.property)
        newdataset.header.property{numel(newdataset.header.property)+1} = curset.header.property{i};
        newdataset.header.value{numel(newdataset.header.property)} = curset.header.value{i};
    end
    %Now insert this into our data sets
    handles.datasets.sets{handles.datasets.count+1} = newdataset;
    handles.datasets.count = handles.datasets.count + 1;


output = handles;