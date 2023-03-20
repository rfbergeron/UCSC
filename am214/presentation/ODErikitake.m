% Write here the initial conditions
ystart = [0.1,0.1,56]

% Write here the vector tspan as tinit : dt : tfinal
tspan = 0:0.001:500;

% This solves the ODE system defined in the sub-function "dydt.m" and 
% returns the vector y(t,i) (for i = 1 to the number of dependent variables) 
% at the times t defined in the vector tspan. Note that if you set 
% tspan = [tinit tfinal] without specifying dt then the solver chooses
% the times of output and returns them in the vector t

[t,y] = ode45(@rikitake,tspan,ystart);

% This draws the trajectory in phase space. 
% Example for 2 variables

% This plots y(t)
% Example to plot y_1(t), y_2(t) and y_3(t) on the same plot
figure(1)
plot(t,y(:,1),'r')
hold on
plot(t,y(:,2),':b')
plot(t,y(:,3),'k')
xlabel("t")
legend("x","y","z")

hold off

% Example to plot y_3(t) vs y_1(t) and y_2(t) (Butterfly diagram).
figure(2)
hold off
plot3(y(:,1),y(:,2),y(:,3), 'b')
view([20, -30, 40])
xlabel("x")
ylabel("y")
zlabel("z")