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
function [Par,Par_Err,Cov,Chi2,Freedom,Resid]=fitpoly(X,Y,DelY,Deg,SigClip,PlotPar);
% FITPOLY FUNCTION
%
%[Par,Par_Err,Cov,Chi2,Freedom,Resid]=fitpoly(X,Y,DelY,Deg,SigClip,PlotPar);
%
% Input  : - Column vector of the independent variable.
%          - Column Vector of the dependent variable.
%          - ERROR
%          - Degree of polynomial. (Default is 1).
% Output : - Fitted parameters [a0,a1,...]
%          - Fitted errors in the parameters [Da0,Da1,...]
%          - The covariance matrix.
%          - Chi2 of the fit.
%          - Degrees of freedom.
MaxNIter = 5;   % maximum number of sigma-clipping iterations
if (nargin<4),
   Deg = 1;
   SigClip = NaN;
elseif (nargin<5),
   SigClip = NaN;
else
   % do nothing
end

N_X  = length(X);
N_Y  = length(Y);
N_DY = size(DelY,1);
N_DX = size(DelY,2);
if (N_DX==2),
   DelX = DelY(:,2);
   DelY = DelY(:,1);
else
   DelX = zeros(N_DY,1);
end
if (N_X~=N_Y),
   error('X and Y must have the same length');
end
if (N_X~=N_DY),
   if (N_DY==1),
      % take equal weights
      if (DelY<=0),
         error('DelY must be positive');
      else
         DelY = DelY.*ones(N_X,1);
      end
   else
      error('Y and DelY must have the same length');
   end
end

Resid = zeros(size(DelY));
if (isnan(SigClip)),
   MaxNIter = 1;
end

Iter = 0;
while (Iter<MaxNIter & (max(abs(Resid)>DelY | Iter==0))),
   Iter = Iter + 1;

   % sigma clipping
   if (isnan(SigClip)),
      % do not sigma clip
   else
      SCInd = find((abs(Resid)./(SigClip.*DelY))<1);  % find non-outlayers
      X    = X(SCInd);
      Y    = Y(SCInd);
      DelY = DelY(SCInd);
      DelX = DelX(SCInd);
      
      N_X  = length(X);
      N_Y  = length(Y);
      N_DY = length(DelY);  
   end

   % degree of freedom
   Freedom = N_X - (Deg + 1);
   
   % building the H matrix
   H = zeros(N_X,Deg+1);
   H(:,1) = ones(N_X,1);
   for Ind=2:1:Deg+1,
      H(:,Ind) = X.^(Ind-1);
   end
   
   % building the Covariance matrix
   B       = 0;
   DelXY   = sqrt(DelY.^2 + (B.*DelX).^2);
   V       = diag(DelXY.^2);
   
   % Old - Memory consuming
   Cov     = inv(H'*inv(V)*H);
   Par     = Cov*H'*inv(V)*Y;
   Par_Err = sqrt(diag(Cov));
   
   
   %'Number of degree of freedom :', Freedom
   Resid = Y - H*Par;
   Chi2  = sum((Resid./DelXY).^2);

   %Chi2/Freedom
   %sqrt(2/Freedom)
   
   NB_Iter = 1;
   while (abs(B-Par(2))>Par_Err(2).*1e-8),
      % iterate with DelX (B~=0)
      B = Par(2);   
      % building the Covariance matrix
      DelXY   = sqrt(DelY.^2 + (B.*DelX).^2);
      V       = diag(DelXY.^2);
      % Old - Memory consuming
      Cov     = inv(H'*inv(V)*H);
      Par     = Cov*H'*inv(V)*Y;
      Par_Err = sqrt(diag(Cov));
      %'Number of degree of freedom :', Freedom
      Resid = Y - H*Par;
      Chi2  = sum((Resid./DelXY).^2);
      NB_Iter = NB_Iter + 1;
   end   
   
end

if (nargin==6),
   % plot results
   length(PlotPar);
   if (length(PlotPar)==0),
      PlotPar(1) = 'o';
      PlotPar(2) = 'r';
   end
   if (length(PlotPar)==1),
      PlotPar(2) = 'r';
   end
   figure(1);
   plot(X,Y,PlotPar(1));
   hold on;
   plot(X,H*Par,PlotPar(2));
   xlabel('X');
   ylabel('Y');
   hold off;
   if (length(PlotPar)==3),
      % plot histogram of residuals
      figure(2);
      [Hist_X,Hist_N]=realhist(sort(abs(Resid)),str2num(PlotPar(3)),[0,max(abs(Resid)).*1.0001]);
      bar(Hist_X,Hist_N);
      axis([0,max(abs(Resid)).*1.0001,0,max(Hist_N)+1]);
      xlabel('X');
      ylabel('Number');
   end
end


%errorxy([X,Y,DelY],[1 2 3],'.');
%hold on;
%plot(X,H*Par,'r');

