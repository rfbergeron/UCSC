a = 0.2;
b = 0.2;
c = 5.7;
disc = c^2 + 4 * a * b;
z = (c + disc^0.5) / (2*a);
x = (c + disc^0.5) / 2;
jac = [0, -1, -1;
       1,  a,  0;
       z,  0,  x-c];