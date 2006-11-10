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
function [ output_args ] = readbounce( input_args )
%READBOUNCE     Reads a file consisting of location triplets
%               and renders it against a bottle volume
filename = '../poslog.txt';
    [bounce, x, y, z] = textread(filename, '','delimiter', ',');

    subplot(1,2,1);
        plot(x, y);
        axis equal;
        hold on;
        circle(0, 0, 0.235);
        circle(0.3, 0, 0.235);
        hold off;

    subplot(1,2,2);
        plot3(x, y, z);
        axis equal;

function circle(x, y, rad)
% Draw a circle
    %rad = 0.235;
    cx = []; cy = [];
    for (i=linspace(0, 2*pi, 1000))
        cx = [cx ; (rad.*cos(i))];
        cy = [cy ; (rad.*sin(i))];
    end
    cx = cx + x; cy = cy + y;
    plot(cx, cy, 'r');