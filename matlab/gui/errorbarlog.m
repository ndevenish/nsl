function hh = errorbarlog(varargin)
%ERRORBARLOG Error bar plot in log coordinates.
%   ERRORBARLOG(X,Y,E,...) plots the graph of vector X vs. vector Y with
%   error bars specified by the vector E in log coordinates.
%
%   The total length of the error bar is 2E. The bars are displayed
%   symmetrically about Y in log coord, ie: they are such that Y is the
%   geometric (instead of arithmetic) mean of the lower and upper bars. 
%
%   ERRORBARLOG has the same syntax as the original Matlab's ERRORBAR
%   function. The only difference is that while ERRORBAR displays the bars
%   symmetrically about Y in linear coord (ie: Y is the arithmetic mean of
%   the lower and upper bars), ERRORBARLOG displays them symmetrically
%   about Y in log coordinates.
%   
%   Example:
%      x=logspace(1,3,20);
%      y=5*(1 + 0.5*(rand(1,20)-0.5)).*x.^(-2);
%      errorbarlog(x,y,y/2,'o-');
%
%   F. Moisy
%   Revision: 1.00,  Date: 2005/07/28
%
%   See also ERRORBAR.


% History:
% 2005/05/28: v1.00, first version.

error(nargchk(3,inf,nargin));

y=varargin{2};
e=varargin{3};

% computes the upper and lower error bars
% ymax and ymin are such that ymax*ymin=y^2 and ymax-ymin=2e.
% u is ymax-y and l is y-ymin.
ymax = e.*(1 + (1+(y./e).^2).^(1/2));
u = ymax - y;
l = 2*e + y - ymax;

h = errorbar(varargin{1:2},l,u,varargin{4:end});

set(gca,'YScale','log'); % set the Y axis in log coordinates.

if nargout>0,
    hh = h;
else
    h=[];
end
