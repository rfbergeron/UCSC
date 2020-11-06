r = -0.125
cond_p = [linspace(-1, 1, 11), (1/sqrt(2)), (-1/sqrt(2))];
trange_p = [0, 20];
dxdt_p = @(t, x) (r * x) + x .^ 3 - x .^ 5;
[t_p, x_p] = ode15s(dxdt_p, trange_p, cond_p);
figure(3);
title('Testing convergence on saddle-node fixed point');
plot(t_p, x_p, 'b');
line([0,20],[1/sqrt(2),1/sqrt(2)]);
