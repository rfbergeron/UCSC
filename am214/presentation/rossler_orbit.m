% the important part of this orbit is between 2.8 and 6.0
% rs = 0.126:0.001:0.432;
rs = 2.8:0.01:6.0;
Y0 = [0.1,0.1,0.1];
tspan = 0:0.001:400;

figure(3);
hold on;

% compute the maps in parallel because it's quite time-consuming
q = parallel.pool.DataQueue;
afterEach(q, @client_scatter);

% measuring byte i/o requires parallel computing package, but parfor
% does not (i think)
% ticBytes(gcp)
parfor i = 1:length(rs)
    r = rs(i);
    maxima = flowmap(@rossler,tspan,Y0,1,r);
    tkeep = floor(length(maxima) / 4);
%   send can only send a single parameter back to the client,
%   so we bundle the r values (the x-axis on the plot) and the
%   maxima (the y-axis on the plot) into a single array, and
%   send them to a wrapper
   	send(q,[r .* ones(1,tkeep); maxima(end-tkeep+1:end)]);
end
% tocBytes(gcp)

hold off;

function client_scatter (data)
    r = data(1,:);
    xs = data(2,:);
    hold on;
    scatter(r,xs);
end