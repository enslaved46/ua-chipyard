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


z1 = fi(zeros(5,1), 1, 16, 0, F);
z2 =  fi(zeros(5,1), 1, 16, 0, F);

% Force exact corner cases in the first 4 elements to guarantee 33-bit boundary coverage
z1(1) = makeComplex(-32768, -32768);  z2(1) = makeComplex(-32768,  32767); % Real = +2,147,483,648 (max 33-bit!)
z1(2) = makeComplex(-32768,  32767);  z2(2) = makeComplex( 32767,  32767); % Imag = -2,147,483,648 (min 33-bit!)
z1(3) = makeComplex( 32767,  32767);  z2(3) = makeComplex( 32767,  32767); % Real = 0, Imag = +2,147,352,578
z1(4) = makeComplex(-32768, -32768);  z2(4) = makeComplex(-32768, -32768); % Real = 0, Imag = +2,147,483,648

% Compute full-precision 33-bit complex product (do NOT pre-allocate as 16-bit fi!)
zMulRes = cMul(z1, z2);
for i = 1:numel(zMulRes)
 printComplexForm(zMulRes(i));
end

z3 = fi(zeros(VECTOR_LEN,1), 1, 16, 0, F);
z4 =  fi(zeros(VECTOR_LEN,1), 1, 16, 0, F);

mulVecRes = fi(zeros(VECTOR_LEN,1), 1, 33, 0, F);

% Sweep evenly across the entire signed 16-bit range (-32768 to +32767)
sweepVals = round(linspace(-32768, 32767, VECTOR_LEN));
for i = 1:VECTOR_LEN
    v1 = sweepVals(i);
    v2 = sweepVals(VECTOR_LEN - i + 1); % Reverse sweep for diverse +/- combinations
    z3(i) = makeComplex(v1, v2);
    z4(i) = makeComplex(v2, -v1);
end


% can use both ways
% z3 = z1 + z2;
% isequal(zPos, z3)


mulVecRes = cMul( z3, z4);

fid = fopen("complex_mul_test_vectors.h", "w");
fprintf(fid, "#ifndef COMPLEX_MUL_TEST_VECTORS_H\n");
fprintf(fid, "#define COMPLEX_MUL_TEST_VECTORS_H\n\n");
fprintf(fid, "#include <stdint.h>\n\n");
fprintf(fid, "#define COMPLEX_TEST_COUNT %d\n\n", VECTOR_LEN);
val = i - (VECTOR_LEN / 2) - 1; 


writeComplexInput16(fid, "MATLAB_MUL_INPUT_A", z3);
writeComplexInput16(fid, "MATLAB_MUL_INPUT_B", z4);
writeComplexMulResultHex(fid, "MATLAB_MUL_EXPECTED_OUT", mulVecRes);

fprintf(fid, "#endif\n");
fclose(fid);


%writeComplexInput16(fid, "inputA", zPos)





