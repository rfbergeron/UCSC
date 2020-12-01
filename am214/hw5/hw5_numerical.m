% HW5 Numerical
% Author: Robert Bergeron

tspan = [0:0.1:10];
xspan = [-2, 2];
yspan = [-2, 2];
npoints = 10;
[xgrid,ygrid] = meshgrid([-2:0.1:2],[-2:0.1:2]);

mu = ;
func = @(t,y) func2(t, y, mu);
phaseportrait(func, tspan, xspan, yspan, npoints, 4);
q = quiver(xgrid, ygrid, xdot(xgrid,ygrid,mu), ydot(xgrid,ygrid,mu));
q.Color = 'black';
q.AutoScaleFactor = 1;
title('');

function dy = func2(t,y,a,b)
    dy = [ y(1) .^ 2 .* y(2) - y(1) + a;
          -y(1) .* y(2) .^ 2 + b];
end

function dyone = xdot(yone,ytwo,a,b)
    dyone = yone .^2 .* ytwo - yone + a;
end

function dytwo = ydot(yone,ytwo,a,b)
    dytwo = -yone .* ytwo .^ 2 + b;
end