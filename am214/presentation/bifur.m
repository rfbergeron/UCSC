function bifur(fcn,x0,a0,af,N)

% Computes and draws a bifurcation diagram for the
% function fcn from parameter values a0 to af in N steps
% Calling sequence: bifur(@fcn,x0,a0,af,N)

% Clear screen and hold
clf 
hold on  

% Not required: select axes. Comment out if not needed
ymin = -5;
ymax = 5;
axis([a0 af ymin ymax])

% Calculate a-increment
deltaa = (af - a0)/N;

% Number of transients to throw out
ntrans = 400;

% Number of points to plot
nplot = 100;

% Starting with parameter at a = a_0
a = a0;
 
%increment the parameter value and repeat until done 
for j = 1:N

% Start at x=x0        
	x=x0;

% calculate the first ntrans iterates and throw them away
for i = 1:ntrans
x=feval(fcn,x,a);
end;

%compute and store the next nplot iterates
for i=1:nplot
x=feval(fcn,x,a);
b(i)=x;
end;

%plot these points
for i = 1:nplot
plot(a,b(i),'.','MarkerSize',1)
     end;
     drawnow

% increase a and repeat
a=a + deltaa;

end;

% Plot grid if necessary.
% grid on
 
 
 
 
