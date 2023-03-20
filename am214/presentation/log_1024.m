function xnp1 = log_1024(x,r)
% approaches zero rapidly for positive r
% try 0 < r < 1
% or try r < 0
     xnp1 = x .* exp(-1 .* r .* (1 - x));
end
