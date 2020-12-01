function Y=iterates(fcn,x0,N,par)

% fcn is the name of the function, x0 is the starting
% value, N is the number of iterates, and par
% is an optional parameter vector.

Y=[x0];
x=x0;

if nargin < 4 % if there are no parameters
    for i=1:N
        y=feval(fcn,x);
        Y=[Y y];
        x=y;
    end;
else
    for i=1:N
        y=feval(fcn,x,par);
        Y=[Y y];
        x=y;
    end;   
end;