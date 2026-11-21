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

% -32768 to 32767 16 bit range
% Minimum: -32768 + -32768 = -65536
% Maximum:  32767 +  32767 =  65534
% Signed 32-bit range: -2147483648 to 2147483647
% A signed 17-bit value can represent:-65536 to 65535
%
%
% for mul
%     two 16 bit number produce gives : 32 bit out, for signed it is -2^32/2 to 2^32/2-1 complex gives 33 becuase we accumulate as well
%         so Maximum imaginary result = 2147483648 one greater than signed 32-bit maximum:
%         (-32768) × (-32768) =  1073741824  // Largest positive product
%         (-32768) × ( 32767) = -1073709056  // Most negative product
%
%         1073741824 + 1073741824        = 2147483648
% For the most negative imaginary result, both products use the most negative product:
% -1073709056 + -1073709056
% = -2147418112
%
%   Signed 32-bit range: -2147483648 to 2147483647
%
%
% 33 bit signed -4294967296 to 4294967295

clear;
clc;
close all;

%z1 = 15 + 15i;
% z1 = single(z1); % to 32 bits each real and imag, total 32 +32 = 64 bits
% z1 = half(z1); % to 16 bits' real and imag total 32 bits

%z2 = 200 - 15i;

MAX_POS_NUM = 32767; %//int16_t(65535/2 -1);
MAX_NEG_NUM = -32768;

%//-32767 is 0x8001
%// extreme test points

z0 = (0 + 0*i);
z1 = (1 + 1*i);
z_1 = (-1 - 1*i);

z2 = (MAX_POS_NUM + (MAX_POS_NUM)*1i);   %// first quard
z3 = (MAX_NEG_NUM + (MAX_POS_NUM)*1i);   %// 2nd quard
z4 = (MAX_NEG_NUM + (MAX_NEG_NUM)*1i); %// third quard
z5 = (MAX_POS_NUM + (MAX_NEG_NUM)*1i); %// 4th quard

z0  = fi(z0,  1, 16, 0, 'OverflowAction', 'Wrap');
z1  = fi(z1,  1, 16, 0, 'OverflowAction', 'Wrap');
z_1 = fi(z_1, 1, 16, 0, 'OverflowAction', 'Wrap');
z2  = fi(z2,  1, 16, 0, 'OverflowAction', 'Wrap');
z3  = fi(z3,  1, 16, 0, 'OverflowAction', 'Wrap');
z4  = fi(z4,  1, 16, 0, 'OverflowAction', 'Wrap');
z5  = fi(z5,  1, 16, 0, 'OverflowAction', 'Wrap');

z6 = fi(50 + 50i, 1, 16, 0, 'OverflowAction', 'Wrap');
z7 = fi(-500 + 500i, 1, 16, 0, 'OverflowAction', 'Wrap');
z8 = fi(-100 - 500i, 1, 16, 0, 'OverflowAction', 'Wrap');
z9 = fi(100 - 500i, 1, 16, 0, 'OverflowAction', 'Wrap');

fprintf("########################################################################\n");
fprintf("Running Test for Complex Addition                                       \n");
fprintf("Results are  around for big Number                                      \n");
fprintf("########################################################################\n");

a= cAdd(z6, z6);
cAdd(z7, z7);
cAdd(z8, z8);
cAdd(z2, z1);        %wraps around overflow max_val +1    , (32767 + 32767i) + (1 + 1i) =     -32768 - 32768i
cAdd(z4, z_1);%  // wraps around under - neg_max-1  neg underflow, (-32768 - 32768i) + (-1 - 1i) =32767 + 32767i



fprintf("##############################6##########################################\n");
fprintf("Running Test for Complex Multiplication                                 \n");
fprintf("Results are wrapped around for big Number                               \n");
fprintf("########################################################################\n");
cMul (z6, z6);
cMul (z7,z7);
cMul (z8,z8);
cMul (z9,z9);


%{
cAdd(z1,z2);
polar(z1);
polar(z2);
%}
function printComplexForm(z)
    realPart = double(real(z));
    imagPart = double(imag(z));

    if imagPart < 0
        fprintf("%d - %di", realPart, -imagPart);
    else
        fprintf("%d + %di", realPart, imagPart);
    end
end

function z3 = cAdd(z1 , z2)
    a = real(z1); b = imag(z1);
    c = real(z2); d = imag(z2);

    % compute discretely, i coudn't do for individual 16 bits by bitwise op
    realComp = (a + c);
    imagComp = (b + d);
    % forcethe outputs to wrap around into 16 bit signed
    realWrapped = fi(realComp, 1, 17, 0, z1.fimath);
    wrappedImag = fi(imagComp, 1, 17, 0, z1.fimath);

    % recombine but no need , may use it if we retun form MAC
    z3 = realWrapped + wrappedImag*1i;
    fprintf("(");  printComplexForm(z1); fprintf(")");
    fprintf(" + ");
    fprintf("(");  printComplexForm(z2); fprintf(")");
    fprintf(" = ");
    printComplexForm(z3);
    fprintf("\n");
    % if (wrappedImag >= 0)
    %
    %     fprintf("z1 + z2 = %d + %di\n", real((z3)), imag((z3)));
    % else
    %     fprintf("z1 + z2 = %d  %di\n", real((z3)), imag((z3)));
    % end
end

function z3 = cMul(z1 , z2)
    a = real(z1); b = imag(z1);
    c = real(z2); d = imag(z2);

    % compute discretely, i coudn't do for individual 16 bits by bitwise op
    realComp = (a * c) - (b * d);
    imagComp = (a * d) + (b * c);
    % forcethe outputs to wrap around into 16 bit signed
    realWrapped = fi(realComp, 1, 33, 0, z1.fimath);
    wrappedImag = fi(imagComp, 1, 33, 0, z1.fimath);

    % recombine but no need , may use it if we retun form MAC
    z3 = realWrapped + wrappedImag*1i;
    fprintf("(");  printComplexForm(z1); fprintf(")");
    fprintf(" * ");
    fprintf("(");  printComplexForm(z2); fprintf(")");
    fprintf(" = ");
    printComplexForm(z3);
    fprintf("\n");
    % if (wrappedImag >= 0)
    %
    %     fprintf("z1 * z2 = %d + %di\n", real((z3)), imag((z3)));
    % else
    %     fprintf("z1 * z2 = %d  %di\n", real((z3)), imag((z3)));
    % end
end

function r = polar( z)
  r = abs(z);
  theta = angle(z);
  fprintf("z = %.2f exp(j%.2f)\n", r, theta);
end
