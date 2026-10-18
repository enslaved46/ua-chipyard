addpath('./complex_functions')
clear;
%cAdd(1+1i,2+2i);

%z6 = fi(200 + 200i, 1, 16, 0, 'OverflowAction', 'Wrap');
%z7 = fi(200 + 200i, 1, 16, 0, 'OverflowAction', 'Wrap');

% makding vector 1 D array with signed type 16 bits no fraction and ound
% when overflow

VECTOR_LEN = 10;

z1 = fi(zeros(1,VECTOR_LEN), 1, 16, 0, 'OverflowAction', 'Wrap');
z2 =  fi(zeros(1,VECTOR_LEN), 1, 16, 0, 'OverflowAction', 'Wrap');
zRes =  fi(zeros(1, VECTOR_LEN), 1, 16, 0, 'OverflowAction', 'Wrap');



for i = 1:VECTOR_LEN
    z1(i) = makeComplex(i, i*2 );
    z2(i) = makeComplex(i*2, i*3 );

end

zRes = cMul( z1, z2);
z3 = z1 .* z2;

isequal(zRes, z3)