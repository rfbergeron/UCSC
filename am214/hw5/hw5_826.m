% HW5 Analytical, Computer Portion, Problem 8.2.6
% Author: Robert Bergeron

tspan = [0:0.1:10];
xspan = [-2, 2];
yspan = [-2, 2];
npoints = 10;
[xgrid,ygrid] = meshgrid([-2:0.1:2],[-2:0.1:2]);

mu = -0.5;
func = @(t,y) p826(t, y, mu);
phaseportrait(func, tspan, xspan, yspan, npoints, 1);
q = quiver(xgrid, ygrid, xdot(xgrid,ygrid,mu), ydot(xgrid,ygrid,mu));
q.Color = 'black';
q.AutoScaleFactor = 1;
title('mu = -0.5');

mu = -0.25;
func = @(t,y) p826(t, y, mu);
phaseportrait(func, tspan, xspan, yspan, npoints, 2);
q = quiver(xgrid, ygrid, xdot(xgrid,ygrid,mu), ydot(xgrid,ygrid,mu));
q.Color = 'black';
q.AutoScaleFactor = 1;
title('mu = -0.25');

mu = 0;
func = @(t,y) p826(t, y, mu);
phaseportrait(func, tspan, xspan, yspan, npoints, 3);
q = quiver(xgrid, ygrid, xdot(xgrid,ygrid,mu), ydot(xgrid,ygrid,mu));
q.Color = 'black';
q.AutoScaleFactor = 1;
title('mu = 0');

mu = 0.25;
func = @(t,y) p826(t, y, mu);
phaseportrait(func, tspan, xspan, yspan, npoints, 4);
q = quiver(xgrid, ygrid, xdot(xgrid,ygrid,mu), ydot(xgrid,ygrid,mu));
q.Color = 'black';
q.AutoScaleFactor = 1;
title('mu = 0.25');

function dy = p826(t,y,mu)
    dy = [mu .* y(1) + y(2) - y(1) .^ 3;
          -y(1) + mu .* y(2) - 2 .* y(2) .^ 3];
end

function dyone = xdot(yone,ytwo,mu)
    dyone = mu .* yone + ytwo - yone .^ 3;
end

function dytwo = ydot(yone,ytwo,mu)
    dytwo = -yone + mu .* ytwo - 2 .* ytwo .^ 3;
end