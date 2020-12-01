% the important parts of the orbit diagrams are in the ranges [0.7:1] and
% [2.8:4] for the sine and logistic map, respectively

rs = 2.8:0.001:4;
x0 = 0.5;
N = 200;
K = 50; % 'keep' parameter, for use with orbit2
X = orbit(@logistic,rs,x0,N,K);

figure(1);
for i = 1:length(rs)
    scatter(rs(i) .* ones(K,1), X(:,i));
    hold on;
end
hold off;

rs = 0.7:0.001:1;
X = orbit(@sine,rs,x0,N,K);

figure(2);
for i = 1:length(rs)
    scatter(rs(i) .* ones(K,1), X(:,i));
    hold on;
end
hold off;