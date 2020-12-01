% HW5 Analytical, Computer Portion, Problem 8.2.3
% Author: Robert Bergeron

tspan = [0:0.1:10];
xspan = [-2, 2];
yspan = [-2, 2];
npoints = 25;
[xgrid,ygrid] = meshgrid([-2:0.1:2],[-2:0.1:2]);

mu = -0.5;
func = @(t,y) p823(t, y, mu);
phaseportrait(func, tspan, xspan, yspan, npoints, 1);
q = quiver(xgrid, ygrid, xdot(xgrid,ygrid,mu), ydot(xgrid,ygrid,mu));
q.Color = 'black';
q.AutoScaleFactor = 1;
title('mu = -0.5');

mu = -0.25;
func = @(t,y) p823(t, y, mu);
phaseportrait(func, tspan, xspan, yspan, npoints, 2);
q = quiver(xgrid, ygrid, xdot(xgrid,ygrid,mu), ydot(xgrid,ygrid,mu));
q.Color = 'black';
q.AutoScaleFactor = 1;
title('mu = -0.25');

mu = 0;
func = @(t,y) p823(t, y, mu);
phaseportrait(func, tspan, xspan, yspan, npoints, 3);
q = quiver(xgrid, ygrid, xdot(xgrid,ygrid,mu), ydot(xgrid,ygrid,mu));
q.Color = 'black';
q.AutoScaleFactor = 1;
title('mu = 0');

mu = 1.0;
func = @(t,y) p823(t, y, mu);
phaseportrait(func, tspan, xspan, yspan, npoints, 4);
q = quiver(xgrid, ygrid, xdot(xgrid,ygrid,mu), ydot(xgrid,ygrid,mu));
q.Color = 'black';
q.AutoScaleFactor = 1;
title('mu = 0.25');

function dy = p823(t,y,mu)
    dy = [-y(2) + mu .* y(1) + y(1) .* y(2) .^ 2;
           y(1) + mu .* y(2) - y(1) .^ 2];
end

function dyone = xdot(yone,ytwo,mu)
    dyone = -ytwo + mu .* yone + yone .* ytwo .^2;
end

function dytwo = ydot(yone,ytwo,mu)
    dytwo = yone + mu .* ytwo - yone .^2;
end