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
function [ new_dataset ] = file_dataset( filename, name )
%FILE_DATASET   gets passed a filename and returns a dataset consisting of
%the following in a structure:
%           - name, filename and path
%           - a structure containing all of the header information,
%           suitable for reading with getproperty()
%           - a column headers and data set item, read with getxydata()

    if (~exist('filename')),
           filename = 'D:\Neutron\processing\results\readset.txt';
    end
    %Validate input
    if (~ischar(filename))
        error('Not passed string');
    end
    %split the path up
    [file_path,file_name] = fileparts(filename);
    
    %Firstly ensure the file exists
    fid = fopen([filename]);
    if (fid == -1), error('Not a valid filename');return;end;fclose(fid);
    fprintf('Reading in file %s\n', filename);

    %%%%%%%%%%%%
    % Is it a pre-processed file?
    if (fileversion(filename) == 3),
        %fprintf('..File is a Pre-processed output file\n');
        load ('-mat', filename);
        new_dataset = indata;
        return;
    end
    
    if (~exist('name')),
       name = strrep(file_name, '_', ' ');
    end
    %Start building the dataset
    new_dataset.filename = file_name;
    new_dataset.pathname = file_path;
    new_dataset.name = name;
    new_dataset.header = getheader(filename);
    new_dataset.headers = new_dataset.header.column_headers;
    
    new_dataset.data = importresults(filename);
    
        %Add the data size to the header
    headeritems = numel(new_dataset.header.property);
    new_dataset.header.property{headeritems+1} = 'Data Points';
    new_dataset.header.value{headeritems+1} = num2str(size(new_dataset.data,1));
    new_dataset.header.property{headeritems+2} = 'Data Columns';
    new_dataset.header.value{headeritems+2} = num2str(numel(new_dataset.headers));
