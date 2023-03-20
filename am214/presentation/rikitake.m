function dydt = rikitake(t,y)
a = 50;
v = 8;
dydt = [(-v.*y(1)) + (y(2).*y(3)); (-v .* y(2)) + y(1).*(y(3) - a); 1 - (y(1).*y(2))];
end
