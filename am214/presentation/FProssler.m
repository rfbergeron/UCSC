% compute the fixed points of the rossler attractor for a given set of
% parameters, with the intent of showing how the fixed point F+ lies
% beyond the attractor's region

% a = 0.1;b = 0.1;c = 14;
a = 0.2;b = 0.2;c = 5.7;
% c = 5.7;a = c / 2;b = c/2;
disc = c^2 - 4 * a * b;
fps = zeros(2,3);
fps(:,1) = [( c + disc^0.5) /  2   ,( c - disc^0.5) /  2];
fps(:,2) = [(-c - disc^0.5) / (2*a),(-c + disc^0.5) / (2*a)];
fps(:,3) = [( c + disc^0.5) / (2*a),( c - disc^0.5) / (2*a)];
figure(2);
hold on;
scatter3(fps(:,1),fps(:,2),fps(:,3),'*r');
hold off;