function cobweb(fcn,x0,N,xmin,xmax,par)

% fcn is the name of the function, x0 is the starting
% value, N is the number of iterates, xmin and xmax
% give the range of x-values to be plotted, and 
% par is an optional parameter vector.

% Calling sequence: cobweb(@fcn,x0,N,xmin,xmax) 
% or                cobweb(@fcn,x0,N,xmin,xmax,par) 


% Determines the range of x for the plotting
dx=(xmax-xmin)/1000;

% Creates vector x 
x=xmin:dx:xmax;

% If there are more than 5 arguments to cobweb, then call feval using additional parameters, otherwise don't
if nargin > 5
    y=feval(fcn,x,par);
else
    y=feval(fcn,x);
end;

% Plot: the function in blue, the axes in black, the straigth line x=y in green.
figure(1)
plot(x,y,'b')
hold on
plot(x,x,'g')
xlabel("x_m",'FontSize',16)
ylabel("x_{m+1}",'FontSize',16)
legend("Recurrence formula","y=x line",'FontSize',16)

% Computes the whole orbit for n=0 to N. Requires iterates.m
if nargin > 5
Y=iterates(fcn,x0,N,par);
else
    Y=iterates(fcn,x0,N);
end;

% Creates the lines corners. 
YY(1)=0;
for i=1:N
    XX(2*i-1)=Y(i);
    XX(2*i)=Y(i);
    YY(2*i)=Y(i+1);
    YY(2*i+1)=Y(i+1);
end;
XX(2*N+1)=Y(N+1);

% Connects the corners in red.

plot(XX,YY,'r',x0,0,'r*','HandleVisibility','off');
hold off

figure(2)
z=1:1:N+1
plot(z,Y',"r*-",'HandleVisibility','off')
xlabel("m",'FontSize',16)
ylabel("x_m",'FontSize',16)



