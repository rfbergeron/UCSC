% Creates the Lorenz map by calculating a trajectory, 
%finding the maxima of z(t) by calculating the derivative 
%of z(t) and then finding the points where it crosses 0. 
%Then plots the map.

y0 = [8 9 10];
tspan = 0:0.001:1000;
[t,y] = ode45(@lorenz,tspan,y0);

dzdt = diff(y(:,3));

count = 1;

for i=1:length(dzdt)-1
if((dzdt(i)*dzdt(i+1) < 0) & (dzdt(i)>0))
     z(count) = y(i,3);
     count = count+1;
end
end
figure(3)
for j=100:count-2
plot(z(j),z(j+1),'o');
hold on
end

title('The Lorenz Map','FontSize',20)
xlabel('z_n','FontSize',20)
ylabel('z_{n+1}','FontSize',20)

hold off
figure(2) 
plot(t,y(:,3))
