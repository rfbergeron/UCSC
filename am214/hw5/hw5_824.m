% HW5 Analytical, Computer Portion, Problem 8.2.4
% Author: Robert Bergeron

tspan = [0:0.1:10];
xspan = [-2, 2];
yspan = [-2, 2];
npoints = 10;
[xgrid,ygrid] = meshgrid([-2:0.1:2],[-2:0.1:2]);

func = @(t,y) p824(t, y, -0.5);
phaseportrait(func, tspan, xspan, yspan, npoints, 1);
q = quiver(xgrid, ygrid, xdot(xgrid,ygrid,-0.5), ydot(xgrid,ygrid,-0.5));
q.Color = 'black';
q.AutoScaleFactor = 1;

func = @(t,y) p824(t, y, 0);
phaseportrait(func, tspan, xspan, yspan, npoints, 2);

func = @(t,y) p824(t, y, 0.25);
phaseportrait(func, tspan, xspan, yspan, npoints, 3);

function dy = p824(t,y,mu)
    dy = [-y(2) + mu .* y(1) + y(1) .* y(2) .^ 2;
           y(1) + mu .* y(2) - y(1) .^ 2];
end

function dyone = xdot(yone,ytwo,mu)
    dyone = -ytwo + mu .* yone + yone .* ytwo .^2;
end

function dytwo = ydot(yone,ytwo,mu)
    dytwo = yone + mu .* ytwo - yone .^2;
end