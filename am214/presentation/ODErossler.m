% Write here the initial conditions
ystart = [0.1,0.1,0.1]
% ystart = [5,10,10]
% ystart = [5,-28,28]

% Write here the vector tspan as tinit : dt : tfinal
tspan = 0:0.01:300;

% plotstart = 0.25 * (length(tspan) - 1);
plotstart = 1;

% This solves the ODE system defined in the sub-function "dydt.m" and 
% returns the vector y(t,i) (for i = 1 to the number of dependent variables) 
% at the times t defined in the vector tspan. Note that if you set 
% tspan = [tinit tfinal] without specifying dt then the solver chooses
% the times of output and returns them in the vector t

rossler2 = @(t,y) rossler(t,y,4);
[t,ytmp] = ode45(rossler2,tspan,ystart);
theta = -pi / 4;
rot = [cos(theta), -sin(theta), 0;
       sin(theta),  cos(theta), 0;
       0         ,  0         , 1];
y = (rot * ytmp')';
% y = ytmp;

% This draws the trajectory in phase space. 
% Example for 2 variables
% This plots y(t)
% Example to plot y_1(t), y_2(t) and y_3(t) on the same plot
figure(1)
hold on
plot(t(plotstart:end),y(plotstart:end,1),'b')
plot(t(plotstart:end),y(plotstart:end,2),'r')
plot(t(plotstart:end),y(plotstart:end,3),'--k')
xlabel("t")
legend("x","y","z")

% Example to plot y_3(t) vs y_1(t) and y_2(t) (Butterfly diagram).
figure(2)
hold off
plot3(y(plotstart:end,1),y(plotstart:end,2),y(plotstart:end,3), 'b')
% view([20, -30, 40])
xlabel("x")
ylabel("y")
zlabel("z")