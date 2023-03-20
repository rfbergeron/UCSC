function [r,lambda] = lyapunov(fcn,dfcn,x0,a0,af,N)

% Computes and draws the lyapunov exponent for the
% function fcn (which has derivative dfcn) 
% from parameter values a0 to af in N steps
% Calling sequence: lyapunov(@fcn,@dfcn,x0,a0,af,N)

% Clear screen and hold
clf 
hold on  

% Not required: select axes. Comment out if not needed
ymin = -4
ymax = 4
axis([a0 af ymin ymax])

% Calculate a-increment
deltaa = (af - a0)/N;

% Number of transients to throw out
ntrans = 300;

% Number of iterations to compute 
ntotal = 10000;

% Starting with parameter at a = a_0
a = a0;
 
%increment the parameter value and repeat until done 
for j = 1:N

% initialize lambda and r
lambda(j) = 0;
r(j) = a;

% Start at x=x0        
	x=x0;

% calculate the first ntrans iterates and throw them away
for i = 1:ntrans
x=feval(fcn,x,a);
end;

%compute and store the next nplot iterates
for i=1:ntotal
x=feval(fcn,x,a);
% calculates and adds to the lambda sum;
lambda(j) = lambda(j) + log(abs(feval(dfcn,x,a)));
end;

% normalize lambda
lambda(j) = lambda(j)./ntotal;

% increase a and repeat
a=a + deltaa;

end;

plot(r,lambda);
 
 
 
 
