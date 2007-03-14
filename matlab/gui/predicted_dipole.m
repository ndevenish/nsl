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
function [ df df_error ] = predicted_dipole( radius, zpos_data, dbdz_data, dbdz_error, donot_dipole )
%predicted_dipole   returns the predicted dipole for mercury.
%       predicted_dipole(radius, zdip, dbdz, dbdz_error)
%       where
%           zdip, dbdz must be vectors of same dimension

if ~exist('donot_dipole')
    donot_dipole = 0;
end
%Define constants
J = 0.5; %spin
hbar = 1.054571596e-34;
gammahg = 7.5901309E+6;
gamman = 29.1646870E+6; %(-)
gammasq = gammahg*gamman * 4 * pi^2;
c = 2.9979e8;
e = 1.6022e-19;

csquared = c^2;

if ~exist('radius'),
    gammahg = 7.59e6;
    gamman = -2.9165e7;
    
    radius = 0.234;
    csquared = 9e16;
end
    

%Generate constant part first
df_extracorrection = 1/2;
df_constpart = 1/4 * J * hbar * (1/csquared);
df_configpart = gammasq .* radius.^2;
df_dipolepart = (1 + (radius.^2 ./ zpos_data.^2));
if donot_dipole,
    df_dipolepart = 1;
    warning('Dipole contribution temporarily removed');
end
df_unitconversion = 1/(e * 1e-2);
df_rest = df_extracorrection .* df_constpart .* df_configpart .* df_dipolepart .* df_unitconversion;
df = df_rest .* dbdz_data;
%df = df_rest .* analytic_dbdz_quick(zpos_data, radius);
df_error = df_rest .* dbdz_error * 0;
%fitted_nodipole = df_extracorrection .* df_constpart.* df_configpart .* df_unitconversion;

function dbdz = analytic_dbdz_quick(z1, R);
p = 1e-5 * 1e-7;
height = 0.12;
z2 = z1 + height;
dbdz = (p / 4*pi) * (-2 / height);
dbdz = dbdz * ( (1./(z1.^2+R.^2).^(3/2)) - (1./(z2.^2+R.^2).^(3/2)) );