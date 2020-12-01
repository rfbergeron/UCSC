function dy = func(t,y,mu);
dy = [-y(2) + mu .* y(1) + y(1) .* y(2) .^ 2;
       y(1) + mu .* y(2) - y(1) .^ 2];
end 
