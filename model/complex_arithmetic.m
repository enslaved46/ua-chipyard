%{
% complex number in Cartesian
z1 = a+bi; |z| = sqrt(a^2 + b^2)
z2 = c+di;

z3 = z1 + z2 = (a+c) + (b+d)i;
z4 = z1* z2 =(ac + adi + bci + bdi^2) = (ac -bd) + (ad +bc)i

Polar form
z1 = r1e^(j\theta1)
z2 = r2e^(j\theta2)
z3 = z1+z2 = r1e^(j\theta1) + r2e^(j\theta2) 
    = r1{(cos(theta1) + i sin(theta1)} + r2{(cos(theta2) + i sin(theta2)}
    = {r1 cos(theta1) + r2 cos(theta2)} + j{sin(theta1) + sin(theta2)}
z4 = z1 - z2
    = {r1 cos(theta1) - r2 cos(theta2)} + j{sin(theta1) - sin(theta2)}
z5 = z1 z2
   = r1e^(j\theta1) r2e^(j\theta2)
   = r1r2 e^j(theta1 theta2)
z6 = z1/z2 
  = r1e^(j\theta1) / r2e^(j\theta2)
  = r1/r2 e^j(theta1 - theta2)
  = r3 e^j(theta3) ; r1/r2 = r3; theta1 -theta2 = theta3

conj(z1) for congugate
r = abs(z1); for |z1|
theta = angle(z1); for angle

z = r*exp(j*theta); writng in polar form

%}

% all the data types are double pricission, meaning 64 bits
% so 64 for real and 64 for imag
%  Name      Size            Bytes  Class     Attributes
%  z1        1x1                16  double    complex 

clear;
clc;

close all;

z1 = 15 + 15i;
% z1 = single(z1); % to 32 bits each real and imag, total 32 +32 = 64 bits
% z1 = half(z1); % to 16 bits' real and imag total 32 bits

z2 = 200 - 15i;

cAdd(z1,z2);
cMul(z1,z2);

polar(z1);
polar(z2);

function z3 = cAdd(z1 , z2)
    z3 = z1 + z2;
    fprintf ("z1 + z2 = %d + %di \n", real(z3), imag(z3))
end

function z3 = cMul(z1 , z2)
    z3 = z1 * z2;
    fprintf ("z1 * z2 = %d + %di\n", real(z3), imag(z3))
end

function r = polar( z)
  r = abs(z);
  theta = angle(z);
  fprintf("z = %.2f exp(j%.2f)\n", r, theta);
end