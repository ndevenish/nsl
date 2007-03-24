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
function varargout = editdata(varargin)
% EDITDATA M-file for editdata.fig
%      EDITDATA, by itself, creates a new EDITDATA or raises the existing
%      singleton*.
%
%      H = EDITDATA returns the handle to a new EDITDATA or the handle to
%      the existing singleton*.
%
%      EDITDATA('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in EDITDATA.M with the given input arguments.
%
%      EDITDATA('Property','Value',...) creates a new EDITDATA or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before editdata_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to editdata_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Copyright 2002-2003 The MathWorks, Inc.

% Edit the above text to modify the response to help editdata

% Last Modified by GUIDE v2.5 04-Feb-2006 22:37:17

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @editdata_OpeningFcn, ...
                   'gui_OutputFcn',  @editdata_OutputFcn, ...
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


% --- Executes just before editdata is made visible.
function editdata_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to editdata (see VARARGIN)

% Choose default command line output for editdata
handles.output = 0;
% Update handles structure
guidata(hObject, handles);

%Check we have a dataset
if (~isempty(varargin) && isfield(varargin{1}, 'count'))
    handles.datasets = varargin{1};
    if (handles.datasets.count)
        %If we are here, we have a proper dataset. Look to see if we have a
        %default value
        if (max(size(varargin)) >= 2)
            if (isnumeric(varargin{2}))
                handles.selection = varargin{2};
                guidata(hObject, handles);
            else
                handles.selection = 1;
            end
        end
        
        %Update the gui
        updategui(handles);
        
        % UIWAIT makes editdata wait for user response (see UIRESUME)
        %only UIWAIT if we have a need to, which is why it is nested
        uiwait(handles.figure1);
    end
end


% --- Outputs from this function are returned to the command line.
function varargout = editdata_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

    % Return the dataset we were working with as an output value, but only if
    % we want to output
    if (handles.output)
        if (isfield(handles, 'datasets'))
            handles.output = handles.datasets;
        else
            handles.output = -1;
        end
    end
    varargout{1} = handles.output;
    % The figure can be deleted now
    delete(handles.figure1);

% --- Executes on selection change in data_list.
function data_list_Callback(hObject, eventdata, handles)
% hObject    handle to data_list (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns data_list contents as cell array
%        contents{get(hObject,'Value')} returns selected item from data_list
    handles.selection = get(handles.data_list, 'Value');
    %set(handles.data_list, 'Value', handles.selection);
    % Update handles structure
    guidata(hObject, handles);
    
    %Update the GUI
    updategui(handles);

% --- Executes during object creation, after setting all properties.
function data_list_CreateFcn(hObject, eventdata, handles)
% hObject    handle to data_list (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called
    % Hint: listbox controls usually have a white background on Windows.
    %       See ISPC and COMPUTER.
    if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
        set(hObject,'BackgroundColor','white');
    end


% --- Executes on button press in ok_pushbutton.
function ok_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to ok_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    handles.output = 1;
    % Update handles structure
    guidata(hObject, handles);
    uiresume(handles.figure1);

% --- Executes on button press in cancel_pushbutton.
function cancel_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to cancel_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    uiresume(handles.figure1);



% --- Executes when user attempts to close figure1.
function figure1_CloseRequestFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

if isequal(get(handles.figure1, 'waitstatus'), 'waiting')
    % The GUI is still in UIWAIT, us UIRESUME
    uiresume(handles.figure1);
else
    % The GUI is no longer waiting, just close it
    delete(handles.figure1);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function updategui( handles )
% This function updates all the gui stuff

%Firstly, fill the data_list
    for i = 1:handles.datasets.count,
        %sources{i} = handles.datasets.sets{i}.filename;
        sources{i} = handles.datasets.sets{i}.name;
    end

    %What is our current data set?
    curset = handles.selection;
    set(handles.data_list, 'String', sources);
    set(handles.data_list, 'Value', curset);

    %Set the name box
    set(handles.name_edit, 'String', handles.datasets.sets{curset}.name);
    
    %Set the filename box
    set(handles.filename_edit, 'String', handles.datasets.sets{curset}.filename);
    set(handles.filename_edit, 'BackgroundColor', get(handles.figure1, 'Color'));
    
    %Set the colour button to the colour of the selection
    set(handles.color_pushbutton, 'BackgroundColor', handles.datasets.sets{curset}.colour);
    
    %Fill the list boxes
    %Find the maximum length of a property
    %maxsize = 0;
    displaystring = [];
    
    %numel(handles.datasets.sets{curset}.header.property)
    %handles.datasets.sets{curset}.header
    %handles.datasets.sets{curset}.header.property
    
    for i=1:numel(handles.datasets.sets{curset}.header.property)
        %Convert to a string if needed
        property = handles.datasets.sets{curset}.header.property{i};
        if ~isstr(property)
            property = num2str(property,3);
        end
        %And the value
        value = handles.datasets.sets{curset}.header.value{i};
        if ~isstr(value)
            value = num2str(value,3);
        end

        displaystring = strvcat(displaystring, sprintf('%-23.23s  %-10s', property, value));
    end
    if (isempty(displaystring))
        displaystring = 'No data properties present';
    end
    %check that the displaystring is not shorter than the current list
    %value
    if (size(displaystring,1) < get(handles.constants_list, 'Value'))
        set(handles.constants_list, 'Value', 1);
    end
    %set(handles.constants_list, 'String', handles.datasets.sets{curset}.header.property);
    set(handles.constants_list, 'String', displaystring);
    %set(handles.constantvals_list, 'String', handles.datasets.sets{curset}.header.value);
    %set(handles.constants_list, 'String', [char(handles.datasets.sets{curset}.header.property) char(handles.datasets.sets{curset}.header.value)]);
    
    %Set the plot properties
    if (handles.datasets.sets{curset}.linetype)
        set(handles.line_check, 'Value', 1);
        if (handles.datasets.sets{curset}.linetype > 1)
            set(handles.errorline_check, 'Value', 1);
        end
    else
        set(handles.line_check, 'Value', 0);
    end
    if (handles.datasets.sets{curset}.ticktype)
        set(handles.tickmarks_check, 'Value', 1);
    else
        set(handles.tickmarks_check, 'Value', 0);
    end

    set(handles.errorbars_check, 'Value', handles.datasets.sets{curset}.errorbars);

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function name_edit_Callback(hObject, eventdata, handles)
% hObject    handle to name_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of name_edit as text
%        str2double(get(hObject,'String')) returns contents of name_edit as a double
    %Don't do anything if we have no sources
    if (handles.datasets.count == 0)
        return;
    end
    newname = get(hObject,'String');
    %Make sure we have a valid name
    if (isempty(newname))
        return;
    end

%    dataset = get(handles.selected);
    handles.datasets.sets{handles.selection}.name = newname;
    
    %Update the handles object
    guidata(hObject, handles);

    updategui(handles);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% --- Executes during object creation, after setting all properties.
function name_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to name_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% --- Executes on button press in color_pushbutton.
function color_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to color_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    newcolour = uisetcolor(handles.datasets.sets{handles.selection}.colour);
    %if (numel(newcolour) == 0)
    %    return;
    %end
    handles.datasets.sets{handles.selection}.colour = newcolour;
    % Update handles structure
    guidata(hObject, handles);
    updategui(handles);




% --- Executes on key press over name_edit with no controls selected.
function name_edit_KeyPressFcn(hObject, eventdata, handles)
% hObject    handle to name_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)




% --- Executes on selection change in constants_list.
function constants_list_Callback(hObject, eventdata, handles)
% hObject    handle to constants_list (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns constants_list contents as cell array
%        contents{get(hObject,'Value')} returns selected item from constants_list
%    set(handles.constantvals_list, 'Value', get(hObject,'Value'));
%    set(handles.constantvals_list, 'ListboxTop', get(hObject,'ListboxTop'));
         if strcmp(get(handles.figure1,'SelectionType'),'open')
            namepropindex = get(handles.constants_list, 'Value');
            nameprop = handles.datasets.sets{handles.selection}.header.property{namepropindex};

            %Name this set after the property
            value = handles.datasets.sets{handles.selection}.header.value{namepropindex};
            if ~isstr(value)
                value = num2str(value,3);
            end
            handles.datasets.sets{handles.selection}.name = [ shortname(nameprop) ' = ' value];

            guidata(hObject, handles);
            updategui(handles);
         end

% --- Executes during object creation, after setting all properties.
function constants_list_CreateFcn(hObject, eventdata, handles)
% hObject    handle to constants_list (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in constantvals_list.
function constantvals_list_Callback(hObject, eventdata, handles)
% hObject    handle to constantvals_list (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns constantvals_list contents as cell array
%        contents{get(hObject,'Value')} returns selected item from constantvals_list
%    set(handles.constants_list, 'Value', get(hObject,'Value'));
%    set(handles.constants_list, 'ListboxTop', get(hObject,'ListboxTop'));


% --- Executes during object creation, after setting all properties.
function constantvals_list_CreateFcn(hObject, eventdata, handles)
% hObject    handle to constantvals_list (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end





function filename_edit_Callback(hObject, eventdata, handles)
% hObject    handle to filename_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of filename_edit as text
%        str2double(get(hObject,'String')) returns contents of filename_edit as a double


% --- Executes during object creation, after setting all properties.
function filename_edit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to filename_edit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end




% --- Executes on button press in nameall_pushbutton.
function nameall_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to nameall_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

    %Get the title of what we want to name everything after
    namepropindex = get(handles.constants_list, 'Value');
    nameprop = handles.datasets.sets{handles.selection}.header.property{namepropindex};
    
    %Loop through each data set
    for i=1:handles.datasets.count
        %First, make sure it has this initial setting
        setpropcol = 0;
        for j=1:numel(handles.datasets.sets{i}.header.property)
            if strcmpi(handles.datasets.sets{i}.header.property{j}, nameprop)
                setpropcol = j;
                break;
            end
        end
        %Jump to next loop if this element does not have this property
        if (~setpropcol) continue; end
        
        %Name this set after the property
        value = handles.datasets.sets{i}.header.value{setpropcol};
        if ~isstr(value)
            value = num2str(value,3);
        end
        handles.datasets.sets{i}.name = [ shortname(nameprop) ' = ' value];
    end
    
    guidata(hObject, handles);
    updategui(handles);


% --- Executes on button press in tickmarks_check.
function tickmarks_check_Callback(hObject, eventdata, handles)
% hObject    handle to tickmarks_check (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of tickmarks_check
    handles.datasets.sets{handles.selection}.ticktype = get(hObject,'Value');
    guidata(hObject, handles);
    
% --- Executes on button press in line_check.
function line_check_Callback(hObject, eventdata, handles)
% hObject    handle to line_check (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of line_check
    handles.datasets.sets{handles.selection}.linetype = get(hObject,'Value');
    if (get(hObject, 'Value') == 0)
        set(handles.errorline_check, 'Value', 0);
    end
    guidata(hObject, handles);



% --- Executes on button press in errorline_check.
function errorline_check_Callback(hObject, eventdata, handles)
% hObject    handle to errorline_check (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of errorline_check
    val = get(hObject, 'Value');
    if (val)
       handles.datasets.sets{handles.selection}.linetype = 2;
       set(handles.line_check, 'Value', 1);
    else
       handles.datasets.sets{handles.selection}.linetype = 1;    
    end
    guidata(hObject, handles);
    
function newname = shortname( property )
%returns the shorter version of a name, for keying purposes
switch(lower(property))
    case 'volume averaged dipole dbzdz'
        newname = '<dB/dz>';
    case 'magnetic dipole moment'
        newname = 'Dipole Strength';
    otherwise
        newname = property;
end


% --- Executes on button press in dataup_pushbutton.
function dataup_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to dataup_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    %Do nothing if at the top
    if (handles.selection == 1)
        return;
    end
    buffer = handles.datasets.sets{handles.selection-1};
    handles.datasets.sets{handles.selection-1} = handles.datasets.sets{handles.selection};
    handles.datasets.sets{handles.selection} = buffer;
    
    handles.selection = handles.selection - 1;
    guidata(hObject, handles);
    
    updategui(handles);
    
        

% --- Executes on button press in datadown_pushbutton.
function datadown_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to datadown_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    if (handles.selection == handles.datasets.count)
        return;
    end
    buffer = handles.datasets.sets{handles.selection+1};
    handles.datasets.sets{handles.selection+1} = handles.datasets.sets{handles.selection};
    handles.datasets.sets{handles.selection} = buffer;
    
    handles.selection = handles.selection + 1;
    guidata(hObject, handles);
    
    updategui(handles);

% --- Executes on button press in sortdata_pushbutton.
function sortdata_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to sortdata_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

    %sort the data!
    
    %First make an array of name and index number
    for i=1:handles.datasets.count,
        sortarray{i,1} = handles.datasets.sets{i}.name;
        sortarray{i,2} = i;
    end
    sortarray = sortrows(sortarray,1);
    
    %Construct a copy of datasets in a new place, in order
    for i=1:handles.datasets.count,
        newset{i} = handles.datasets.sets{sortarray{i,2}};
    end
    
    %copy the old to the new
    handles.datasets.sets = newset;
    
    guidata(hObject, handles);
    
    updategui(handles);


% --- Executes on button press in errorbars_check.
function errorbars_check_Callback(hObject, eventdata, handles)
% hObject    handle to errorbars_check (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of errorbars_check
    handles.datasets.sets{handles.selection}.errorbars = get(hObject,'Value');
    guidata(hObject, handles);
