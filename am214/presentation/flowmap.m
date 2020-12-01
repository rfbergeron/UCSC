function maxima=flowmap(fcn,ts,y0,dim,par)
% this function applies lorenz's trick for getting a one-dimensional map
% from a flow; taking the local maxima along one dimension of the system
%
% ARGUMENTS
%   fcn:    System you wish to make a map out of.
%   ts:     Timespan to evaluate the system over.
%   y0:     Starting conditions for system.
%   dim:    Dimension you want the local maxima of. For example, if you
%           wanted the local maxima along the x-axis, you'd set this
%           argument to 1, for the y-axis set to 2, etc.
%   par:    Optional parameter to be supplied to the system defined by fcn,
%           usually one that changes behavior in interesting ways
%           (bifurcations) when varied. For the lorenz attractor, you would
%           most likely provide the value for 'r', and in the rossler
%           attractor, you would probably give the value for 'c'.
%
% OUTPUTS
%   maxima: succesive local maxima of the flow along the given axis

if nargin < 5
    [t,y] = ode45(fcn,ts,y0);
else
    fcn2 = @(t,y) fcn(t,y,par);
    [t,y] = ode45(fcn2,ts,y0);
end

ddimdt = diff(y(:,dim));

count = 1;

for i=1:length(ddimdt)-1
if( ddimdt(i)*ddimdt(i+1) < 0 & ddimdt(i)>0 )
     maxima(count) = y(i,dim);
     count = count+1;
end
end

end
