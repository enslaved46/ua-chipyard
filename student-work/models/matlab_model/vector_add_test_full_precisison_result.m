clear;
clc;
addpath('./complex_functions')
%cAdd(1+1i,2+2i);

%z6 = fi(200 + 200i, 1, 16, 0, F);
%z7 = fi(200 + 200i, 1, 16, 0, F);

%z8 = cMul(z6, z7);
%disp(z8);
% makding vector 1 D array with signed type 16 bits no fraction and ound
% when overflow

VECTOR_LEN = 5000;
%est = makeComplex(1, 2);
%disp(test)
%dbstop if error

F = fimath( ...
    'OverflowAction', 'Wrap', ...
    'ProductMode',    'FullPrecision', ...
    'SumMode',        'FullPrecision');


z1 = fi(zeros(VECTOR_LEN,1), 1, 16, 0, F);
z2 =  fi(zeros(VECTOR_LEN,1), 1, 16, 0, F);


addVectRes = fi(zeros(VECTOR_LEN,1), 1, 16, 0, F);


for i = 1:VECTOR_LEN
    z1(i) = makeComplex(i, i*2 );
    z2(i) = makeComplex(i*2, i*3 );
    %i
end

addVectRes = cAdd( z1, z2)
% can use both ways
% z3 = z1 + z2;
% isequal(zPos, z3)

for i = 1:VECTOR_LEN
    val = i - (VECTOR_LEN / 2) - 1; % center test vetor around 0, -2500 to 2501
    z3(i) = makeComplex(-val*10, -val*20 );
    z4(i) = makeComplex(-val*20, -val*30 );
    %i
end

fid = fopen("complex_add_test_vectors.h", "w");
fprintf(fid, "#ifndef COMPLEX_ADD_TEST_VECTORS_H\n");
fprintf(fid, "#define COMPLEX_ADD_TEST_VECTORS_H\n\n");
fprintf(fid, "#include <stdint.h>\n\n");
fprintf(fid, "#define COMPLEX_TEST_COUNT %d\n\n", VECTOR_LEN);
val = i - (VECTOR_LEN / 2) - 1; 
writeComplexInput16(fid, "matlab_input_a", z1);
writeComplexInput16(fid, "matlab_input_b", z2);
writeComplexOut17(fid, "matlab_expected_out", addVectRes);

fprintf(fid, "#endif\n");
fclose(fid);


%writeComplexInput16(fid, "inputA", zPos)





