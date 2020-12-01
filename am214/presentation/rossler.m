function dydt = rossler(t,y,c)

if(nargin < 3)
%     c = 4;
%     c = 14;
    c = 5.7;
end

% cool values
% a=[0.126-0.432],b=2;
% a = 0.4;b = 2;
% a = c;c = 4;b = 2;
% a = 0.1;b = 0.1;
% a = 2.5; b = 2.5;
a = 0.2;b = 0.2;
% a = c / 2;b = c / 2;

dydt = [-y(2)-y(3); y(1) + a .* y(2); b + y(3) .* (y(1) - c)];
end
