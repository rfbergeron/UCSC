%% Problem 3
num_iterations = 101
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
title('Problem 3');
plot(rs, xstars);
xlabel('r','FontSize',13,'FontWeight','Bold');
ylabel('x-star','FontSize',13,'FontWeight','Bold');
%xticks(linspace(minr, maxr, spanr));
%yticks(linspace(-2, 2, 9));
set(gca, 'XAxisLocation', 'origin');
set(gca, 'YAxisLocation', 'origin');
