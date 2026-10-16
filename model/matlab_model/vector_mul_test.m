addpath('./complex_functions')
%cAdd(1+1i,2+2i);

%z6 = fi(200 + 200i, 1, 16, 0, 'OverflowAction', 'Wrap');
%z7 = fi(200 + 200i, 1, 16, 0, 'OverflowAction', 'Wrap');

%z8 = cMul(z6, z7);
%disp(z8);
% makding vector 1 D array with signed type 16 bits no fraction and ound
% when overflow

VECTOR_LEN = 10;
est = makeComplex(1, 2);
disp(test)
dbstop if error

z1 = fi(zeros(1, VECTOR_LEN), 1, 16, 0, 'OverflowAction', 'Wrap');
z2 =  fi(zeros(1, VECTOR_LEN), 1, 16, 0, 'OverflowAction', 'Wrap');
z =  fi(zeros(1, VECTOR_LEN), 1, 16, 0, 'OverflowAction', 'Wrap');
for i = 1:VECTOR_LEN
    z1(i) = makeComplex(i, i*2 );
    z2(i) = makeComplex(i*2, i*3 );

end

z = cMul( z1, z2);
z3 = z1 .* z2;

isequal(z, z3)