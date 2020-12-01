
tspan = 0:0.01:10;

xmax = 2.0;
xmin = -2.0;
ymax = 2.0;
ymin = -2.0;

% This defines and labels the axes of the phase portrait
axis([xmin xmax ymin ymax]);
     xlabel('x','FontSize',20);
     ylabel('y','FontSize',20);
hold on


npoints = 10;

for i=0:npoints
    x0 = xmin + i*(xmax-xmin)/npoints
    for j=0:npoints
            y0 = ymin + j*(ymax-ymin)/npoints
            [t,y] = ode15s(@func,tspan,[x0;y0]);
            plot(y(:,1),y(:,2));
    end
end




