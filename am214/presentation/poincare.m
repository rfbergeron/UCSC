[t,yint] = ode45(@rossler,tspan,ystart);
dim = 1;
plane = 0;
% we rotate the figure so that we get the poincare at a specific angle; in
% our case we want to rotate about the z-axis. Since y is already in vector
% form we'll just do it with a rotation matrix
theta = -4 * pi / 16;
rot = [cos(theta), -sin(theta), 0;
       sin(theta),  cos(theta), 0;
       0         ,  0         , 1];

% make y into a list of column vectors so that we can carry out the
% multiplication, then turn it back into row vectors
y = (rot * yint')';

for i = 100:length(y)-1
    if((y(i,dim) - plane) * (y(i+1,dim) - plane) < 0.)
        if(y(i+1,dim) - y(i,dim) > 0)
            plot(y(i,2),y(i,3),'ob'); hold on;
        else
            plot(y(i,2),y(i,3),'og'); hold on;
        end
    end
end

hold off;