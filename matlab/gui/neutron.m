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
function varargout = neutron(varargin)
% NEUTRON M-file for neutron.fig
%      NEUTRON, by itself, creates a new NEUTRON or raises the existing
%      singleton*.
%
%      H = NEUTRON returns the handle to a new NEUTRON or the handle to
%      the existing singleton*.
%
%      NEUTRON('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in NEUTRON.M with the given input arguments.
%
%      NEUTRON('Property','Value',...) creates a new NEUTRON or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before neutron_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to neutron_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Copyright 2002-2003 The MathWorks, Inc.

% Edit the above text to modify the response to help neutron

% Last Modified by GUIDE v2.5 23-Mar-2007 23:42:54

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @neutron_OpeningFcn, ...
                   'gui_OutputFcn',  @neutron_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before neutron is made visible.
function neutron_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to neutron (see VARARGIN)

% Choose default command line output for neutron
handles.output = hObject;

handles.datasets.count = 0;
handles.datasets.order = 0;

handles.datasets.XLog = 0;
handles.datasets.YLog = 0;

handles.datasets.XGrid = 0;
handles.datasets.YGrid = 0;

handles.datasets.XError = 0;
handles.datasets.YError = 0;

handles.datasets.legend = 1;

handles.datasets.reduce = 0;

handles.datasets.tightscaling = 0;

handles.datasets.leftlimit = 0;
handles.datasets.limitleft = 0;

handles.datasets.limitright = 0;
handles.datasets.rightlimit = 0;

handles.selection = 1;

%handles.lastdir = [cd '/../results'];
handles.lastdir = [];

updatesourceslist(hObject, handles);

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes neutron wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = neutron_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on selection change in listbox1.
function listbox1_Callback(hObject, eventdata, handles)
% hObject    handle to listbox1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns listbox1 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from listbox1


% --- Executes during object creation, after setting all properties.
function listbox1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to listbox1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in opt_semilogx.
function opt_semilogx_Callback(hObject, eventdata, handles)
% hObject    handle to opt_semilogx (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of opt_semilogx


% --- Executes on button press in pushbutton3.
function pushbutton3_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in pushbutton4.
function pushbutton4_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on selection change in xaxis_list.
function xaxis_list_Callback(hObject, eventdata, handles)
% hObject    handle to xaxis_list (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns xaxis_list contents as cell array
%        contents{get(hObject,'Value')} returns selected item from xaxis_list
    handles.datasets.limitleft = 0;
    handles.datasets.limitright = 0;
    guidata(hObject, handles);
    set(handles.limitleft_check, 'Value', handles.datasets.limitleft);
    set(handles.limitright_check, 'Value', handles.datasets.limitright);

    replot(hObject, handles);

% --- Executes during object creation, after setting all properties.
function xaxis_list_CreateFcn(hObject, eventdata, handles)
% hObject    handle to xaxis_list (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in yaxis_list.
function yaxis_list_Callback(hObject, eventdata, handles)
% hObject    handle to yaxis_list (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns yaxis_list contents as cell array
%        contents{get(hObject,'Value')} returns selected item from yaxis_list
replot(hObject, handles);

% --- Executes during object creation, after setting all properties.
function yaxis_list_CreateFcn(hObject, eventdata, handles)
% hObject    handle to yaxis_list (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on selection change in list_datasources.
function list_datasources_Callback(hObject, eventdata, handles)
% hObject    handle to list_datasources (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns list_datasources contents as cell array
%        contents{get(hObject,'Value')} returns selected item from list_datasources
%Do nothing if no data sources
if (handles.datasets.count)
    
    selection = get(hObject,'Value');
    handles.selection = selection;
    guidata(hObject, handles);
    %contents = get(hObject,'String');
    %set(handles.sourcename_edit, 'String', contents{selection});

    %If they double clicked, open the data editor
    if strcmp(get(handles.figure1,'SelectionType'),'open')
        retval = editdata(handles.datasets, selection);
        if ~isnumeric(retval)
            handles.datasets = retval;
            %Update the handles object
            guidata(hObject, handles);
            %Update the Gui
            updatesourceslist(hObject, handles);
            %Might as well replot!
            replot(hObject, handles);
        end
    end

end

% --- Executes during object creation, after setting all properties.
function list_datasources_CreateFcn(hObject, eventdata, handles)
% hObject    handle to list_datasources (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in remove_pushbutton.
function remove_pushbutton_Callback(hObject, eventdata, handles)
    % hObject    handle to remove_pushbutton (see GCBO)
    % eventdata  reserved - to be defined in a future version of MATLAB
    % handles    structure with handles and user data (see GUIDATA)

    %If the handles list is empty, don't do anything
    if (handles.datasets.count == 0)
        return;
    end

    %Get the value of the list box selected item
    %strings = get(handles.list_datasources, 'String');
    %filename = strings{get(handles.list_datasources, 'Value')};
    %if (isempty(filename))
    %    return
    %end
    
    %Assume that the selected item in the listbox corresponds with the
    %entry in datasets
    itemtoremove = get(handles.list_datasources, 'Value');
    

    %Determing which entry in datasets this corresponds to
%     for i = 1:handles.datasets.count,
%         %handles.datasets.sets{i}.filename
%         if (strcmp(filename, handles.datasets.sets{i}.name)),
%             %We have a match! Move all the others up a notch
%             for j = i:handles.datasets.count,
%                 handles.datasets.sets{i} = handles.datasets.sets{j};
%             end
%             handles.datasets.count = handles.datasets.count - 1;
%         end
%     end
    for i=itemtoremove:(handles.datasets.count-1)
        handles.datasets.sets{i} = handles.datasets.sets{i+1};
    end
    handles.datasets.count = handles.datasets.count - 1;

    %Update the handle variable
    guidata(hObject, handles);

    %Update the list box
    updatesourceslist( hObject, handles );
    replot(hObject, handles);

% --- Executes on button press in opt_plot.
function opt_plot_Callback(hObject, eventdata, handles)
    % hObject    handle to opt_plot (see GCBO)
    % eventdata  reserved - to be defined in a future version of MATLAB
    % handles    structure with handles and user data (see GUIDATA)

    % Hint: get(hObject,'Value') returns toggle state of opt_plot

function readnewfile(filename, pathname, hObject, handles)
    %Reads in a file specified by filename

%     %Check if this file exists
%     fid = fopen([pathname filename]);
%     if (fid == -1),
%         error('Not a valid filename');
%         return
%     end
%     fclose(fid);
%     infile.filename = filename;
%     infile.pathname = pathname;
%     infile.name = strrep(filename, '_', ' ');
    %because I changed the way it treats paths and file, I must reassign
    %filename here to incldue both so tons of other things don't go wrong
    filename = [pathname filename];

    infile = file_dataset(filename);
    
%     infile.header = getheader(filename);
%     infile.headers = infile.header.column_headers;
%     infile.data = importresults(filename);

    infile.order = handles.datasets.order + 1;
    infile.ticktype = 1;
    infile.linetype = 1; %0 no line, 1 line, 2 line(even on errorplot)
    infile.errorbars = 1;
        
    %Assign the dataset a colour based on it's 'order'
    colour = mod(infile.order-1, 7) + 1;
    co = get(handles.axes1, 'ColorOrder');    %borrow the colour order from the chart
    infile.colour = co(colour, :);

    %Apply this dataset to the group
    count = handles.datasets.count;
    handles.datasets.count = count + 1;
    handles.datasets.order = handles.datasets.order + 1;
    handles.datasets.sets{count + 1} = infile;

    
    %Update handles structure
    guidata(hObject, handles);

    %Update the list box
    updatesourceslist( hObject, handles );
    replot(hObject, handles);

function updatesourceslist( hObject, handles )
%updatesourceslist is kind of a minsnomer, because it actually now updates
%the whole gui

    %Generate a list of sources from the data sets
    if (handles.datasets.count > 0)
        %Update the list box
        for i = 1:handles.datasets.count,
            %sources{i} = handles.datasets.sets{i}.filename;
            sources{i} = handles.datasets.sets{i}.name;
        end
    else
        sources{1} = 'No Sources Loaded';
        sources{2} = '';
        sources{3} = 'Load Sources from the file menu.';
    end

    %Make sure that the listbox doesn't have an erroneous value parameter
    listval = get(handles.list_datasources, 'Value');
    if (listval > handles.datasets.count)
        if (handles.datasets.count > 0)
            set(handles.list_datasources, 'Value', handles.datasets.count);
        else
            set(handles.list_datasources, 'Value', 1);
        end
    end
    %Update the list box
    set(handles.list_datasources, 'String', sources);

    %Now let's find and update the common headers
    if (handles.datasets.count > 0),
        common = handles.datasets.sets{1}.headers;
        for i = 1:handles.datasets.count,
            common = commonheaders(common, handles.datasets.sets{i}.headers);
        end
    else
        common{1} = 'No Sources Loaded';
    end
    handles.datasets.commonheaders = common;

    %Now update the selection boxes
    %Get the currently selected items
    oldlist = get(handles.xaxis_list, 'String');
    xsel = oldlist(get(handles.xaxis_list, 'Value'));
    ysel = oldlist(get(handles.yaxis_list, 'Value'));
    
    %Find what these are in the new common
    xval = 1; yval = 1;
    for i=1:numel(common)
        if strcmp(common{i}, xsel)
            xval = i;
        end
        if strcmp(common{i}, ysel)
            yval = i;
        end
    end
    
    set(handles.xaxis_list, 'Value', 1);
    set(handles.yaxis_list, 'Value', 1);
    set(handles.xaxis_list, 'String', common)
    set(handles.yaxis_list, 'String', common)
    set(handles.xaxis_list, 'Value', xval);
    set(handles.yaxis_list, 'Value', yval);
    
    %Update the handles object
    guidata(hObject, handles);
    
    %Now update the menu ticks
    if handles.datasets.legend
        set(handles.legend_menu, 'Checked', 'on');
    else
        set(handles.legend_menu, 'Checked', 'off');
    end
    if handles.datasets.tightscaling
        set(handles.autoaxis_menu, 'Checked', 'off');
        set(handles.tightaxis_menu, 'Checked', 'on');
    else
        set(handles.autoaxis_menu, 'Checked', 'on');
        set(handles.tightaxis_menu, 'Checked', 'off');
    end
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function replot(hObject, handles, targetaxes)
    %Replots (or plots) the graph

    %Are we using our axes or a different target axes?
    if (exist('targetaxes') == 0)
        axes(handles.axes1);
    else
        axes(targetaxes);
    end
    
    %Prepare our axes for drawing
    cla; hold on;
    
    %Should we be logarithmic?
    if(handles.datasets.XLog == 1)
        set(gca, 'XScale', 'log');
    else
        set(gca, 'XScale', 'linear');
    end
    if(handles.datasets.YLog == 1)
        set(gca, 'YScale', 'log');
    else
        set(gca, 'YScale', 'linear');
    end
    
    %return if we have no data - this after the clearing stuff so not to
    %leave a graph when all the data has been cleared
    if (handles.datasets.count == 0)
        return;
    end
    
%     %Get the name of the x and y axis
%     strings = get(handles.xaxis_list, 'String');
%     xaxis = strings{get(handles.xaxis_list, 'Value')};
%     yaxis = strings{get(handles.yaxis_list, 'Value')};
% 
%     % Loop through each data set
     for i = 1:handles.datasets.count,
        [xaxis, yaxis, xdata, ydata, yerror, yerrcolumn] = getxydata(handles, i);
        if isempty(xdata)
            continue;
        end
        xdata = xdata*modifier(xaxis);
        ydata = ydata*modifier(yaxis);
        yerror= yerror*modifier(yaxis);
   
         yerrcolumn = yerrcolumn * handles.datasets.sets{i}.errorbars;

        %Grab the colour
        colorstyle = handles.datasets.sets{i}.colour;

        %Set the line style
        if (handles.datasets.sets{i}.linetype)
            linetype = '-';
        else
            linetype = 'none';
        end
        if (handles.datasets.sets{i}.ticktype)
            ticktype = 'x';
        else
            ticktype = 'none';
        end
        %Now plot this - do we want error bars? then plot a special way
        if (~(yerrcolumn && handles.datasets.YError))
            plot(xdata, ydata, 'Marker', ticktype, 'LineStyle', linetype, 'Color', colorstyle, 'DisplayName', handles.datasets.sets{i}.name);
        else
            if (handles.datasets.sets{i}.linetype > 1)
                linetype = '-';
            else
                linetype = 'none';
            end
            %if handles.datasets.YLog
            %    errorbarlog(xdata, ydata, yerror, 'Marker', ticktype, 'LineStyle', linetype, 'Color', colorstyle, 'DisplayName', handles.datasets.sets{i}.name);
            %else
            errorbar(xdata, ydata, yerror, 'Marker', ticktype, 'LineStyle', linetype, 'Color', colorstyle, 'DisplayName', handles.datasets.sets{i}.name);
            %end
            if handles.datasets.XLog
                errorbarlogx;
            end
        end
        
        
        %Rotate the colours on this axis
        %rotatecolours;
    end

    %Set the Axis marks
    xlabel ([nameproperty(xaxis) ' ' units(xaxis)]); ylabel ([nameproperty(yaxis) ' ' units(yaxis)]);
    title ([nameproperty(yaxis) ' _{versus} ' nameproperty(xaxis)]);

    if (handles.datasets.tightscaling)
        axis tight;
    else
        axis auto;
    end
    %Set Gridlines?
    if handles.datasets.XGrid
        set(gca, 'XGrid', 'on');
    else
        set(gca, 'XGrid', 'off');
    end
    if handles.datasets.YGrid
        set(gca, 'YGrid', 'on');
    else
        set(gca, 'YGrid', 'off');
    end
    
    %Make a legend?
    if (handles.datasets.legend)
        legend off
        legend('Location','best')
    else
        legend off
    end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% --- Executes on button press in makefig_pushbutton.
function makefig_pushbutton_Callback(hObject, eventdata, handles)
    % hObject    handle to makefig_pushbutton (see GCBO)
    % eventdata  reserved - to be defined in a future version of MATLAB
    % handles    structure with handles and user data (see GUIDATA)
    figure;
    newaxes = axes;
    replot(hObject, handles, newaxes);


% --- Executes on button press in recolor_pushbutton.
function recolor_pushbutton_Callback(hObject, eventdata, handles)
    % hObject    handle to recolor_pushbutton (see GCBO)
    % eventdata  reserved - to be defined in a future version of MATLAB
    % handles    structure with handles and user data (see GUIDATA)

    for i=1:handles.datasets.count,
        handles.datasets.sets{i}.order = i;
            %Assign the dataset a colour based on it's 'order'
            colour = mod(i-1, 7) + 1;
            co = get(handles.axes1, 'ColorOrder');    %borrow the colour order from the chart
            handles.datasets.sets{i}.colour = co(colour, :);
    end
    handles.datasets.order = handles.datasets.count;

    %Update the handles object
    guidata(hObject, handles);

    replot(hObject, handles);


function returnval = modifier( property )
    %modifier - returns a multiplication constant because certain known values
    %come out of the simulation with adjusted values

    if (strcmpi(property, 'nedm'))
        returnval = 1e-26;
    elseif(strcmpi(property, 'Hg-n edm'))
        returnval = 1e-26;
    elseif(strcmpi(property, 'edm'))
        returnval = 1e-26;
    else
        returnval = 1;
    end

function returnval = units( property )
    %units - returns a string saying the units of a known named value
    if (strcmp(property, 'n vel'))
        returnval = '(ms^{-1})';
    elseif (strcmp(property, 'nedm'))
        returnval = '(e.cm)';
    elseif (strcmp(property, 'Hg-n edm'))
        returnval = '(e.cm)';
    elseif (strcmp(property, 'edm'))
        returnval = '(e.cm)';
    elseif (strcmp(property, 'radius'))
        returnval = '(m)';
    elseif (strcmpi(property, 'Dipole Z Position'))
        returnval = '(m)';
    elseif (strcmpi(property, 'Dipole Strength'));
        returnval = '(A.m^2)';
    else
        returnval = '';
    end

function returnval = nameproperty( property )
    if (strcmp(property, 'n vel'))
        returnval = 'Neutron Velocity';% units(property)];
    elseif (strcmp(property, 'nedm'))
        returnval = 'False EDM Effect';
    elseif (strcmp(property, 'Dipole Strength'))
        returnval = 'Magnetic dipole strength';
    elseif (strcmp(property, 'Dipole Z Position'))
        returnval = 'Magnetic Dipole Z position';
    elseif (strcmp(property, 'dip_dbzdz'))
        returnval = 'Volume Averaged dB/dz';
    elseif (strcmp(property, 'Hg-n edm'))
        returnval = 'False Hg-n EDM';
    elseif (strcmp(property, 'edm'))
        returnval = 'False Hg-n EDM';
    elseif (strcmp(property, 'radius'))
        returnval = 'Bottle Radius';
    %elseif (strcmp(property, ''))
    %    returnval = '';
    else
        returnval = [strrep(property, '_', ' ') ' ' units(property)];
    end




% --- Executes on button press in logx_pushbutton.
function logx_pushbutton_Callback(hObject, eventdata, handles)
    % hObject    handle to logx_pushbutton (see GCBO)
    % eventdata  reserved - to be defined in a future version of MATLAB
    % handles    structure with handles and user data (see GUIDATA)

    %Get the current case of x
    if (handles.datasets.XLog),
        set(handles.logx_pushbutton, 'String', 'Log');
        handles.datasets.XLog = 0;
    else
        set(handles.logx_pushbutton, 'String', 'Lin');
        handles.datasets.XLog = 1;
    end
    %Update the handles object
    guidata(hObject, handles);
    %replot
    replot(hObject, handles);

% --- Executes on button press in logy_pushbutton.
function logy_pushbutton_Callback(hObject, eventdata, handles)
    % hObject    handle to logy_pushbutton (see GCBO)
    % eventdata  reserved - to be defined in a future version of MATLAB
    % handles    structure with handles and user data (see GUIDATA)

    %Get the current case of y
    if (handles.datasets.YLog),
        set(handles.logy_pushbutton, 'String', 'Log');
        handles.datasets.YLog = 0;
    else
        set(handles.logy_pushbutton, 'String', 'Lin');
        handles.datasets.YLog = 1;
    end
    %Update the handles object
    guidata(hObject, handles);
    %replot
    replot(hObject, handles);


% --- Executes on mouse press over axes background.
function axes1_ButtonDownFcn(hObject, eventdata, handles)
% hObject    handle to axes1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)



% --- Executes on button press in logx_checkbox.
function logx_checkbox_Callback(hObject, eventdata, handles)
% hObject    handle to logx_checkbox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of logx_checkbox
    handles.datasets.XLog = get(hObject, 'Value');
    
    %Update the handles object
    guidata(hObject, handles);
    %replot
    replot(hObject, handles);


% --- Executes on button press in logy_checkbox.
function logy_checkbox_Callback(hObject, eventdata, handles)
% hObject    handle to logy_checkbox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of logy_checkbox
    handles.datasets.YLog = get(hObject, 'Value');
    
    %Update the handles object
    guidata(hObject, handles);
    %replot
    replot(hObject, handles);



% --- Executes on button press in yerrorbars_checkbox.
function yerrorbars_checkbox_Callback(hObject, eventdata, handles)
% hObject    handle to yerrorbars_checkbox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of yerrorbars_checkbox
    handles.datasets.YError = get(hObject, 'Value');
    
    %Update the handles object
    guidata(hObject, handles);
    %replot
    replot(hObject, handles);


% --------------------------------------------------------------------
function mnufile_Callback(hObject, eventdata, handles)
% hObject    handle to mnufile (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function new_file_menu_Callback(hObject, eventdata, handles)
% hObject    handle to new_file_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    handles.datasets.count = 0;
    handles.datasets.order = 0;
    
    handles.datasets.limitleft = 0;
    handles.datasets.limitright = 0;
    set(handles.limitleft_check, 'Value', 0);
    set(handles.limitright_check, 'Value', 0);
    
    
    %Update the handles object
    guidata(hObject, handles);
    
    updatesourceslist(hObject, handles);
    replot(hObject, handles);
    refresh;
    import_file_menu_Callback(hObject, eventdata, handles);

% --------------------------------------------------------------------
function open_file_menu_Callback(hObject, eventdata, handles)
% hObject    handle to open_file_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    %Get a filename to load
    %Basetype = 1337;
    [filename, path] = uigetfile('*.nds');
    if isnumeric(filename)
        return;
    end
    infile = load('-mat', [path filename]);
    if (isnumeric(infile))
        return;
    end

    %Make sure tht the infile data set has features we want
    if (~isfield(infile.datasets, 'reduce'))
        infile.datasets.reduce = 0;
    end
    if(~isfield(infile.datasets, 'limitleft'))
        %If it doesn't have this it won't have any limiting stuff
        infile.datasets.limitleft = 0;
        infile.datasets.limitright = 0;
        infile.datasets.leftlimit = 0;
        infile.datasets.rightlimit = 0;
    end
    %Make sure all the data sets have the wanted features
    for i=1:infile.datasets.count
        if ~isfield(infile.datasets.sets{i}, 'errorbars')
            infile.datasets.sets{i}.errorbars = 1;
        end
    end
    
    handles.datasets = infile.datasets;
    
    set(handles.logx_checkbox, 'Value', handles.datasets.XLog);
    set(handles.logy_checkbox, 'Value', handles.datasets.YLog);
    
%    set(handles.xerrorbars_checkbox, 'Value', handles.datasets.XError);
    set(handles.yerrorbars_checkbox, 'Value', handles.datasets.YError);
    
    set(handles.xgrid_check, 'Value', handles.datasets.XGrid);
    set(handles.ygrid_check, 'Value', handles.datasets.YGrid);
    
    set(handles.reduce_chk, 'Value', handles.datasets.reduce);
    
    set(handles.limitleft_check, 'Value', handles.datasets.limitleft);
    set(handles.limitright_check, 'Value', handles.datasets.limitright);
    
    updatesourceslist(hObject, handles);
    set(handles.xaxis_list, 'Value', infile.xaxis);
    set(handles.yaxis_list, 'Value', infile.yaxis);
    replot(hObject, handles);

% --------------------------------------------------------------------
function save_file_menu_Callback(hObject, eventdata, handles)
% hObject    handle to save_file_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    %do we have a filename already
    [filename, path] = uiputfile('*.nds');
    
    type = 1337;
    %saveobject.datasets = handles.datasets;
    datasets = handles.datasets;
    
    xaxis = get(handles.xaxis_list, 'Value');
    yaxis = get(handles.yaxis_list, 'Value');

    save ([path filename], 'datasets', 'type', 'xaxis', 'yaxis');



% --------------------------------------------------------------------
function import_file_menu_Callback(hObject, eventdata, handles)
% hObject    handle to import_file_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    %handles    
    curdir = cd;
    if (isempty(handles.lastdir) == 0)
        cd(handles.lastdir);
    end
    filename = cell(1,1);
    [filename, pathname] = uigetfile('*.txt;*.ppf', 'MultiSelect','on');
    cd (curdir);
    if(isnumeric(filename))
        return;
    end
    %Sort the filenames into order
    if (size(filename,1) > 1)
        filename = sort(filename);
    end
    handles.lastdir = pathname;

    %Update the handle variable
    guidata(hObject, handles);

    %filename
    %filename(1)
    %filename{1}

    fprintf('Reading in new data:\n');
    if (iscell(filename)),
        for i=1:prod(size(filename)),
            %fprintf('Reading in file %s%s\n', pathname, filename{i});
    %        fprintf('Data count: %d\n', handles.datasets.count);
            readnewfile(filename{i}, pathname, hObject, handles);
            %Subtle Error: We need to reload in handles array here
            handles = guidata(hObject);
        end
    else
        %fprintf('Reading in file %s%s\n', pathname, filename);
        readnewfile(filename, pathname, hObject, handles);
    end




% --------------------------------------------------------------------
function test_context_Callback(hObject, eventdata, handles)
% hObject    handle to test_context (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)





function sourcename_edit_Callback(hObject, eventdata, handles)
% hObject    handle to sourcename_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of sourcename_edit as text
%        str2double(get(hObject,'String')) returns contents of sourcename_edit as a double
    %Don't do anything if we have no sources
    if (handles.datasets.count == 0)
        return;
    end
    
    newname = get(hObject,'String');
    %Make sure we have a valid name
    if (isempty(newname))
        return;
    end

    dataset = get(handles.list_datasources, 'Value');
    handles.datasets.sets{dataset}.name = newname;
    
    %Update the handles object
    guidata(hObject, handles);
    
    updatesourceslist(hObject, handles);
    replot(hObject, handles);

    
    

% --- Executes during object creation, after setting all properties.
function sourcename_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to sourcename_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end





function edit3_Callback(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit3 as text
%        str2double(get(hObject,'String')) returns contents of edit3 as a double


% --- Executes during object creation, after setting all properties.
function edit3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end




% --------------------------------------------------------------------
function data_menu_Callback(hObject, eventdata, handles)
% hObject    handle to data_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function color_data_menu_Callback(hObject, eventdata, handles)
% hObject    handle to color_data_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    for i=1:handles.datasets.count,
        handles.datasets.sets{i}.order = i;
            %Assign the dataset a colour based on it's 'order'
            colour = mod(i-1, 7) + 1;
            co = get(handles.axes1, 'ColorOrder');    %borrow the colour order from the chart
            handles.datasets.sets{i}.colour = co(colour, :);
    end
    handles.datasets.order = handles.datasets.count;

    %Update the handles object
    guidata(hObject, handles);

    replot(hObject, handles);



% --------------------------------------------------------------------
function exportfig_file_menu_Callback(hObject, eventdata, handles)
% hObject    handle to exportfig_file_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    figure;
    newaxes = axes;
    replot(hObject, handles, newaxes);



% --- Executes on button press in import_pushbutton.
function import_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to import_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    import_file_menu_Callback(hObject, eventdata, handles);


% --------------------------------------------------------------------
function replot_menu_Callback(hObject, eventdata, handles)
% hObject    handle to replot_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    updatesourceslist(hObject, handles);
    replot(hObject, handles);



% --- Executes on button press in autoaxis_pushbutton.
function autoaxis_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to autoaxis_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in tightaxis_pushbutton.
function tightaxis_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to tightaxis_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function autoaxis_menu_Callback(hObject, eventdata, handles)
% hObject    handle to autoaxis_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    if strcmp(get(gcbo, 'Checked'),'off')
        set(gcbo, 'Checked', 'on');
        set(handles.tightaxis_menu, 'Checked', 'off');
        handles.datasets.tightscaling = 0;
        guidata(hObject, handles);
    end
    replot(hObject, handles);
    
% --------------------------------------------------------------------
function tightaxis_menu_Callback(hObject, eventdata, handles)
% hObject    handle to tightaxis_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    if strcmp(get(gcbo, 'Checked'),'off')
        set(gcbo, 'Checked', 'on');
        set(handles.autoaxis_menu, 'Checked', 'off');
        handles.datasets.tightscaling = 1;
        guidata(hObject, handles);
    end
    replot(hObject, handles);
    
% --------------------------------------------------------------------
function legend_menu_Callback(hObject, eventdata, handles)
% hObject    handle to legend_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    if handles.datasets.legend
        handles.datasets.legend = 0;
        set(gcbo, 'Checked', 'off');
    else
        handles.datasets.legend = 1;
        set(gcbo, 'Checked', 'on');
    end
    guidata(hObject, handles);
    replot(hObject, handles);

% --------------------------------------------------------------------
function Untitled_1_Callback(hObject, eventdata, handles)
% hObject    handle to Untitled_1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)




% --------------------------------------------------------------------
function fitlin_data_menu_Callback(hObject, eventdata, handles)
% hObject    handle to fitlin_data_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

%Make a data set that is a linear fit of another data set
    if(~handles.datasets.count)
        return;
    end

    %Firstly grab the data set we want to be using
    curset = handles.datasets.sets{handles.selection};
    
    [xaxis, yaxis, xdata, ydata, yerror, yerrcolumn] = getxydata(handles, handles.selection);


    %Now put these values through the polynomial fitting function
    [params, paramerr, cov, chi2, freedom] = fitpoly(xdata, ydata, yerror);
    
    %Assume that NaN = 0
    params(isnan(params)) = 0;
    paramerr(isnan(paramerr)) = 0;
    
    %grab the standard deviation of ydata
    %if (isnan(chi2))
    %    chi2 = (sum(ydata)/std(ydata)).^2;
    %end
    
    %Usr thest values for formulate a data set
    newdataset.data = [xdata, xdata.*params(2)+params(1), xdata.*paramerr(2)+paramerr(1)];
    
    %Let's start constructing a new data set to contain our information. It
    %will have three columns of data.
    newdataset.filename = curset.filename;
    newdataset.pathname = '';
    newdataset.name = ['Linear fit to ' curset.name];
    newdataset.order = handles.datasets.order + 1;
    newdataset.ticktype = 0;
    newdataset.linetype = 2;
    newdataset.errorbars = 0;
    newdataset.colour = curset.colour;

    %Now start filling the data-related values
    newdataset.headers = {xaxis yaxis 'uncert'};
        
    %Fill the 'header' information
    newdataset.header.column_headers = newdataset.headers;
    newdataset.header.property{1} = 'Gradient';
        newdataset.header.value{1} = params(2) * modifier(yaxis);
    newdataset.header.property{2} = 'Gradient Error';
        newdataset.header.value{2} = paramerr(2) * modifier(yaxis);
    newdataset.header.property{3} = 'Offset';
        newdataset.header.value{3} = params(1) * modifier(yaxis);
    newdataset.header.property{4} = 'Offset Error';
        newdataset.header.value{4} = paramerr(2) * modifier(yaxis);
    newdataset.header.property{5} = 'Chi-squared';
        newdataset.header.value{5} = chi2;%sprintf('%.2e', chi2);
    newdataset.header.property{6} = 'Degrees of Freedom';
        newdataset.header.value{6} = freedom;
    newdataset.header.property{7} = 'Data Points';
        newdataset.header.value{7} = size(newdataset.data,1);
    newdataset.header.property{8} = 'Data Columns';
        newdataset.header.value{8} = size(newdataset.data,2);
    newdataset.header.property{9} = '---BELOW ARE ORIGINAL---';
        newdataset.header.value{9} = 0;
    %Now copy over all the old header values
    for i=1:numel(curset.header.property)
        newdataset.header.property{numel(newdataset.header.property)+1} = curset.header.property{i};
        newdataset.header.value{numel(newdataset.header.property)} = curset.header.value{i};
    end
    %Now insert this into our data sets
    handles.datasets.sets{handles.datasets.count+1} = newdataset;
    handles.datasets.count = handles.datasets.count + 1;
    
    guidata(hObject, handles);
    
    %This will bring the shared variables down to 2, so set the list values
    %set(handles.xaxis_list, 'Value', 1);
    %set(handles.yaxis_list, 'Value', 2);
    
    %Refresh the display
    updatesourceslist(hObject, handles);
    replot(hObject, handles);
%        data: [100x13 double]



% --- Executes on button press in pushbutton25.
function pushbutton25_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton25 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
handles.datasets
handles.datasets.sets{1}
handles.datasets.sets{1}.header


% --------------------------------------------------------------------
function close_file_menu_Callback(hObject, eventdata, handles)
% hObject    handle to close_file_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    delete(handles.figure1);



% --- Executes on button press in makefigure_pushbutton.
function makefigure_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to makefigure_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    exportfig_file_menu_Callback(hObject, eventdata, handles);



% --------------------------------------------------------------------
function xgrid_menu_Callback(hObject, eventdata, handles)
% hObject    handle to xgrid_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function ygrid_menu_Callback(hObject, eventdata, handles)
% hObject    handle to ygrid_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in xgrid_check.
function xgrid_check_Callback(hObject, eventdata, handles)
% hObject    handle to xgrid_check (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of xgrid_check
    handles.datasets.XGrid = get(hObject, 'Value');
    guidata(hObject, handles);
    replot(hObject, handles);

% --- Executes on button press in ygrid_check.
function ygrid_check_Callback(hObject, eventdata, handles)
% hObject    handle to ygrid_check (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of ygrid_check
    handles.datasets.YGrid = get(hObject, 'Value');
    guidata(hObject, handles);
    replot(hObject, handles);



% --- Executes on button press in limitleft_check.
function limitleft_check_Callback(hObject, eventdata, handles)
% hObject    handle to limitleft_check (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of limitleft_check
    handles.datasets.limitleft = get(hObject, 'Value');
    guidata(hObject, handles);
    replot(hObject, handles);


% --- Executes on button press in limitright_pushbutton.
function limitright_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to limitright_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    [x y] = ginput(1);
    handles.datasets.rightlimit = x;
    handles.datasets.limitright = 1;
    set(handles.limitright_check, 'Value', 1);

    guidata(hObject, handles);
    replot(hObject, handles);
    
% --- Executes on button press in limitleft_pushbutton.
function limitleft_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to limitleft_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    [x y] = ginput(1);
    handles.datasets.leftlimit = x;
    handles.datasets.limitleft = 1;
    set(handles.limitleft_check, 'Value', 1);
    
    guidata(hObject, handles);
    replot(hObject, handles);



% --- Executes on button press in reduce_chk.
function reduce_chk_Callback(hObject, eventdata, handles)
% hObject    handle to reduce_chk (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of reduce_chk
    handles.datasets.reduce = get(hObject,'Value');
    guidata(hObject, handles);
    replot(hObject, handles);


% --- Executes on button press in limitright_check.
function limitright_check_Callback(hObject, eventdata, handles)
% hObject    handle to limitright_check (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of limitright_check
    handles.datasets.limitright = get(hObject, 'Value');
    guidata(hObject, handles);
    replot(hObject, handles);

    
    
% --------------------------------------------------------------------
function fitdbdz_menu_Callback(hObject, eventdata, handles)
% hObject    handle to fitdbdz_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%    [xaxis, yaxis, xdata, ydata, yerror, yerrcolumn] = getxydata(handles, handles.selection);

    x = 0;
   [x chi2] = fminsearch(@(x) chi2_fitdbdz(x,handles, handles.selection),1);
   
   [xaxis, yaxis, xdata, ydata, yerror, yerrcolumn] = getxydata(handles, handles.selection);
   
   curset = handles.datasets.sets{handles.selection};
   newdataset.data = [xdata xdata.*x];
       %Let's start constructing a new data set to contain our information. It
    %will have three columns of data.
    newdataset.filename = curset.filename;
    newdataset.pathname = '';
    newdataset.name = ['edm=<dbdz> fit to ' curset.name];
    newdataset.order = handles.datasets.order + 1;
    newdataset.ticktype = 0;
    newdataset.linetype = 2;
    newdataset.errorbars = 0;
    newdataset.colour = curset.colour;

    %Now start filling the data-related values
    newdataset.headers = {xaxis yaxis}% 'uncert'};
        
    %Fill the 'header' information
%     newdataset.header.column_headers = newdataset.headers;
    newdataset.header.property{1} = 'Modifying Constant';
        newdataset.header.value{1} = x;
     newdataset.header.property{2} = 'Gradient';
         newdataset.header.value{2} = x;
%     newdataset.header.property{2} = 'Gradient Error';
%         newdataset.header.value{2} = paramerr(2) * modifier(yaxis);
%     newdataset.header.property{3} = 'Offset';
%         newdataset.header.value{3} = params(1) * modifier(yaxis);
%     newdataset.header.property{4} = 'Offset Error';
%         newdataset.header.value{4} = paramerr(2) * modifier(yaxis);
     newdataset.header.property{3} = 'Chi-squared';
         newdataset.header.value{3} = chi2;%sprintf('%.2e', chi2);
%     newdataset.header.property{6} = 'Degrees of Freedom';
%         newdataset.header.value{6} = freedom;
     newdataset.header.property{4} = 'Data Points';
         newdataset.header.value{4} = size(newdataset.data,1);
     newdataset.header.property{5} = 'Data Columns';
         newdataset.header.value{5} = size(newdataset.data,2);
% 
    %Now insert this into our data sets
    handles.datasets.sets{handles.datasets.count+1} = newdataset;
    handles.datasets.count = handles.datasets.count + 1;
    
    guidata(hObject, handles);
    
    %Refresh the display
    updatesourceslist(hObject, handles);
    replot(hObject, handles);

   
function fitval = chi2_fitdbdz(x, handles, dataset)
    %Make
    [xaxis, yaxis, xdata, ydata, yerror, yerrcolumn] = getxydata(handles, dataset);
    yfit = xdata .* x *modifier(yaxis);
    std(ydata)
    chi2 = ((ydata*modifier(yaxis) - yfit) ./ (yerror*modifier(yaxis))).^2;
    fitval = sum(chi2);
        
% --------------------------------------------------------------------
function fitdbdzplus_menu_Callback(hObject, eventdata, handles)
% hObject    handle to fitdbdzplus_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

    %Grab the radius
    radius = [];
    for i=1:numel(handles.datasets.sets{handles.selection}.header.property)
        if strcmpi(handles.datasets.sets{handles.selection}.header.property{i}, 'Bottle radius')
            radius = handles.datasets.sets{handles.selection}.header.value{i};
        end
    end
    if isempty(radius)
        error 'Dataset has no bottle radius value';
        return;
    end
    
   [x chi2] = fminsearch(@(x) chi2_fitdbdzr(x,handles, handles.selection, radius),1);
   
   [xaxis, yaxis, xdata, ydata, yerror, yerrcolumn] = getxydata(handles, handles.selection);
   
   curset = handles.datasets.sets{handles.selection};
   height = 0.12;
   const = radius^2 / height^2;
   newdataset.data = [xdata xdata.*x.*(1+const)];
       %Let's start constructing a new data set to contain our information. It
    %will have three columns of data.
    newdataset.filename = curset.filename;
    newdataset.pathname = '';
    newdataset.name = ['edm=<dbdz>(1+r^2/z^2) fit to ' curset.name];
    newdataset.order = handles.datasets.order + 1;
    newdataset.ticktype = 0;
    newdataset.linetype = 2;
    newdataset.errorbars = 0;
    newdataset.colour = curset.colour;

    %Now start filling the data-related values
    newdataset.headers = {xaxis yaxis}% 'uncert'};
        
    %Fill the 'header' information
%     newdataset.header.column_headers = newdataset.headers;
    newdataset.header.property{1} = 'Modifying Constant';
        newdataset.header.value{1} = x;
     newdataset.header.property{2} = 'Gradient';
         newdataset.header.value{2} = x.*(1+const);
%     newdataset.header.property{2} = 'Gradient Error';
%         newdataset.header.value{2} = paramerr(2) * modifier(yaxis);
%     newdataset.header.property{3} = 'Offset';
%         newdataset.header.value{3} = params(1) * modifier(yaxis);
%     newdataset.header.property{4} = 'Offset Error';
%         newdataset.header.value{4} = paramerr(2) * modifier(yaxis);
     newdataset.header.property{3} = 'Chi-squared';
         newdataset.header.value{3} = chi2;%sprintf('%.2e', chi2);
%     newdataset.header.property{6} = 'Degrees of Freedom';
%         newdataset.header.value{6} = freedom;
     newdataset.header.property{4} = 'Data Points';
         newdataset.header.value{4} = size(newdataset.data,1);
     newdataset.header.property{5} = 'Data Columns';
         newdataset.header.value{5} = size(newdataset.data,2);
% 
    %Now insert this into our data sets
    handles.datasets.sets{handles.datasets.count+1} = newdataset;
    handles.datasets.count = handles.datasets.count + 1;
    
    guidata(hObject, handles);
    
    %Refresh the display
    updatesourceslist(hObject, handles);
    replot(hObject, handles);


function fitval = chi2_fitdbdzr(x, handles, dataset, radius)
    [xaxis, yaxis, xdata, ydata, yerror, yerrcolumn] = getxydata(handles, dataset);
    height = 0.12;
    const = radius^2 ./ height^2;
    yfit = xdata .* x .* (1+const) .* modifier(yaxis);
    std(ydata)
    chi2 = ((ydata*modifier(yaxis) - yfit) ./ (yerror*modifier(yaxis))).^2;
    fitval = sum(chi2);


% --------------------------------------------------------------------
function analysis_menu_Callback(hObject, eventdata, handles)
% hObject    handle to analysis_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function agradient_analysis_menu_Callback(hObject, eventdata, handles)
% hObject    handle to agradient_analysis_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    gradient_analysis( handles );



% --------------------------------------------------------------------
function analysedata_menu_Callback(hObject, eventdata, handles)
% hObject    handle to analysedata_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

    analyse_falsedata( handles );
    


% --------------------------------------------------------------------
function addpredicted_menu_Callback(hObject, eventdata, handles)
% hObject    handle to addpredicted_menu (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    handles = addpredicted(handles);
    
    guidata(hObject, handles);
    
    %Refresh the display
    updatesourceslist(hObject, handles);
    replot(hObject, handles);


% --------------------------------------------------------------------
function convert_data_Callback(hObject, eventdata, handles)
% hObject    handle to convert_data (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    convertfile;

