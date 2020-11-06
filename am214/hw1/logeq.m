nhatstart = linspace(0, 2, 11);
trange = [0, 5];
tspan = linspace(0, 5, 21);

%% Problem 3
function dnhatdthat = altlogeq(t, nhat)
    dnhatdthat = -nhat.^2 + nhat;
end

[t, n] = ode15s(@altlogeq, trange, nhatstart);

figure(1)
plot(t,n,'b')
xlabel("t-hat")
ylabel("N-hat")
title("Problem 3")

%% Problem 4
function nhat = anl_soln(that, nhatstart)
    nhat = nhatstart .* exp(that) ./ (nhatstart .* (exp(that) - 1) + 1);
end

[that, nhat_num] = ode15s(@altlogeq, tspan, 0.5);
nhat_anl = anl_soln(tspan', 0.5);

figure(2)
plot(tspan, nhat_anl, '--k');
hold on
plot(tspan, nhat_num, 'r');
xlabel("t-hat")
ylabel("N-hat")
title("Problem 4")
legend("Analytical", "Numerical")
