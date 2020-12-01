function YS = orbit(fcn,rs,x0,niter,nkeep)

% this function iterates fcn for each value in rs, with starting
% value x0, for niter iterations, and returns the tail end of the
% values computed

YS = zeros(nkeep,length(rs));

for i = 1:length(rs)
    Y = iterates(fcn,x0,niter,rs(i));
    YS(:,i) = Y(end-nkeep+1:end);
end