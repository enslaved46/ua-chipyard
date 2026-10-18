addpath('./complex_functions')
%cAdd(1+1i,2+2i);

%z6 = fi(200 + 200i, 1, 16, 0, 'OverflowAction', 'Wrap');
%z7 = fi(200 + 200i, 1, 16, 0, 'OverflowAction', 'Wrap');

%z8 = cMul(z6, z7);
%disp(z8);
% makding vector 1 D array with signed type 16 bits no fraction and ound
% when overflow

VECTOR_LEN = 10;
%est = makeComplex(1, 2);
%disp(test)
%dbstop if error

z1 = fi(zeros(VECTOR_LEN,1), 1, 16, 0, 'OverflowAction', 'Wrap');
z2 =  fi(zeros(VECTOR_LEN,1), 1, 16, 0, 'OverflowAction', 'Wrap');
z1 = fi(zeros(VECTOR_LEN,1), 1, 16, 0, 'OverflowAction', 'Wrap');
z2 =  fi(zeros(VECTOR_LEN,1), 1, 16, 0, 'OverflowAction', 'Wrap');

zPos = fi(zeros(VECTOR_LEN,1), 1, 16, 0, 'OverflowAction', 'Wrap');
zNeg = fi(zeros(VECTOR_LEN,1), 1, 16, 0, 'OverflowAction', 'Wrap');

for i = 1:VECTOR_LEN
    z1(i) = makeComplex(i, i*2 );
    z2(i) = makeComplex(i*2, i*3 );
    %i
end

zPos = cAdd( z1, z2);
% can use both ways
% z3 = z1 + z2;
% isequal(zPos, z3)

for i = 1:VECTOR_LEN
    z3(i) = makeComplex(-i*10, -i*20 );
    z4(i) = makeComplex(-i*20, -i*30 );
    %i
end

zNeg = cAdd( z3, z4);
