function dydt = lorenz(t,y)
b = 8.0/3.0;
sigma = 10;
r = 100.5;
dydt = [sigma.*(y(2)-y(1)); y(1).*(r - y(3)) - y(2); y(1).*y(2) - b.*y(3)];
end
