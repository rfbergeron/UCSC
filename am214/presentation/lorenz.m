function dydt = lorenz(t,y,r)
b = 8.0/3.0;
sigma = 10;
if(nargin < 3)
    r = 25;
end
dydt = [sigma*(y(2)-y(1)); r*y(1) - y(2) - y(1)*y(3); y(1)*y(2) - b*y(3)];
end
