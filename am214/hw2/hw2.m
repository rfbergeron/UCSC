%% AM214 HW2
%% Author: Robert Bergeron

num_iterations = 101;

%% Problem 1
bif = @(x) x.^4 - x.^2;
xstars = linspace(-2, 2, num_iterations);
rs = bif(xstars);
minr = floor(min(rs));
maxr = ceil(max(rs));
spanr = maxr - minr + 1;

% we can plot the inverse of r(xstar) by just swapping the points
% we just computed instead of actually inverting the function
figure(1);
plot(rs, xstars, 'k');
title('Problem 1');
xlabel('r','FontSize',13,'FontWeight','Bold');
ylabel('x-star','FontSize',13,'FontWeight','Bold');
xticks(linspace(minr, maxr, spanr));
yticks(linspace(-2, 2, 9));
set(gca, 'XAxisLocation', 'origin');
set(gca, 'YAxisLocation', 'origin');

% save values for reuse in problem 4
xstars_p1 = xstars;
rs_p1 = rs;

%% Problem 2
dxdt = @(t, x) -x + x.^3 - x.^5;
trange = [0, 10];
cond = 0.01;
[t, x] = ode15s(dxdt, trange, cond);

figure(2);
plot(t, x, 'b');
title('Problem 2');
xlabel('t','FontSize',13,'FontWeight','Bold');
ylabel('x','FontSize',13,'FontWeight','Bold');
set(gca, 'XAxisLocation', 'origin');
set(gca, 'YAxisLocation', 'origin');

%% Problem 2, AM214

% plot the approximate analytical solution at r = -1, xstar = 0
aprx_soln = @(t) 1 ./ (100 .* exp(t));
tspan = linspace(0, 10, num_iterations);
aprx_x = aprx_soln(tspan);
hold on;
plot(tspan, aprx_x, 'r--');
legend('Numerical', 'Analytical');

%% Problem 3
rs = linspace(-1, 1, num_iterations);
trange = [0, 10];
cond = 0.01;
xstars = zeros(1, num_iterations);

for i = 1:num_iterations
    dxdt = @(t, x) rs(i) .* x + x .^ 3 - x .^ 5;
    [t, x] = ode15s(dxdt, trange, cond);
    xstars(i) = x(end);
    cond = 0.01 + x(end);
end

figure(3);
plot(rs, xstars, 'b');
title('Problem 3&4');
xlabel('r','FontSize',13,'FontWeight','Bold');
ylabel('x-star','FontSize',13,'FontWeight','Bold');
set(gca, 'XAxisLocation', 'origin');
set(gca, 'YAxisLocation', 'origin');

%% Problem 4
rs = linspace(1, -1, num_iterations);
cond = 0.01;

for i = 1:num_iterations
    dxdt = @(t, x) rs(i) .* x + x .^ 3 - x .^ 5;
    [t, x] = ode15s(dxdt, trange, cond);
    xstars(i) = x(end);
    cond = 0.01 + x(end);
end

% reuse figure 3
hold on
plot(rs, xstars, 'r');
plot(rs_p1, xstars_p1, 'k');
xlim([-1, 1]);
ylim([0, 1.5]);
legend({'Increasing r', 'Decreasing r', 'Numerical'}, 'Location', 'northwest');
