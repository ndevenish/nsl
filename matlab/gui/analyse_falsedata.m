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
function output = analyse_falsedata(handles)

output = handles;

%Get the current data set
dataset = handles.selection;
curset = handles.datasets.sets{dataset};

%Grab the properties
radius = getproperty(curset, 'Bottle Radius');

%Grab the data
zpos_name = 'Dipole Z position';
dbdz_name = 'dip_dbzdz';
hgnedm_name = 'Hg-n edm';

[zpos_data, zpos_error] = getdata(zpos_name, curset);
[dbdz_data, dbdz_error] = getdata(dbdz_name, curset);
[hgnedm_data, hgnedm_error] = getdata(hgnedm_name, curset);
% Fix the scale
hgnedm_data = hgnedm_data * 1e-26;
hgnedm_error = hgnedm_error * 1e-26;
%Verify that these have been filled

%Generate expected data sets
%Define constants
% J = 0.5; %spin
% hbar = 1.0546e-34;
% gammahg = 7.5901309E+6;
% gamman = 29.1646870E+6; %(-)
% gammasq = gammahg*gamman * 4 * pi^2;
% c = 2.9979e8;

% %Generate constant part first
% df_extracorrection = 1/2;
% df_constpart = 1/4 * J * hbar * (1/c^2);
% df_configpart = gammasq * radius.^2 * dbdz_data;
% df_dipolepart = (1 + (radius^2 ./ zpos_data.^2));
% df_unitconversion = 1/(1.6022e-19 * 1e-2);
% fitted_df_data = df_extracorrection .* df_constpart .* df_configpart .* df_dipolepart .* df_unitconversion;
%fitted_nodipole = df_extracorrection .* df_constpart.* df_configpart .* df_unitconversion;
[fitted_df_data fitted_df_error] = predicted_dipole(radius, zpos_data, dbdz_data, dbdz_error);
rawestimate_data = fitted_df_data ./ (1 + (radius^2 ./ zpos_data.^2));

%Plot these data sets preliminarily
%plot(xdata, ydata, 'Marker', ticktype, 'LineStyle', linetype, 'Color', colorstyle, 'DisplayName', handles.datasets.sets{i}.name);
initialfit_fig = figure;
hold on;
set(gca, 'YScale', 'log'); set(gca, 'YGrid', 'on');
%['Color', curset.colour] to have original colour
%plot(zpos_data, hgnedm_data, 'b', 'Displayname', curset.name);
errorbar(zpos_data, hgnedm_data, hgnedm_error, 'b', 'Displayname', curset.name);
axis auto; a = axis;
%plot(zpos_data, fitted_df_data, 'r--', 'Displayname', 'Expected');
errorbar(zpos_data, fitted_df_data, fitted_df_error, 'r--', 'Displayname', 'Expected');
plot(zpos_data, rawestimate_data, 'g:', 'Displayname', 'Uncorrected');
axis(a);
xlabel 'Dipole z Position'; ylabel 'False Hg-n edm dipole effect';
legend('Location','best');
title 'Expected and simulated dalse dipole effects for magnetic dipoles'
%Attempt to correct the fit?
ret = 'No';
%ret = questdlg('Attempt fit correction through fit offset based on data?');
switch ret
    case 'Cancel',
        return;
end % switch

%Adjust data and add to plot if needed
if strcmp(ret, 'Yes'),
    offset = hgnedm_data(numel(zpos_data)) / fitted_df_data(numel(zpos_data))
    %Apply the offset to the data
    fitted_df_data = fitted_df_data * offset;
    hold on;
    plot(zpos_data, fitted_df_data, 'r--', 'Displayname', 'Expected (corrected)');
    axis(a);
    set(gca, 'YScale', 'log'); set(gca, 'YGrid', 'on'); set(gca, 'XGrid', 'on');
    xlabel 'Dipole z Position (m)'; ylabel 'False Hg-n edm dipole effect (e.cm)';
    legend('Location','best');
end

%Now generate plots on the ralations between these data sets
relative_data = hgnedm_data ./ fitted_df_data;
relative_error = relative_data .* sqrt( (hgnedm_error ./ hgnedm_data).^2 + (fitted_df_error ./ fitted_df_data).^2);
relative_fig = figure;

errorbar(zpos_data, relative_data, relative_error, 'r', 'Displayname', 'Simulated / predicted');
xlabel 'Dipole z Position (M)'; ylabel 'Ratio';
title 'Ratio between simulated and predicted false dipole moments at differing magnetic dipole positions';
set(gca, 'XGrid', 'on');
set(gca, 'YGrid', 'on');