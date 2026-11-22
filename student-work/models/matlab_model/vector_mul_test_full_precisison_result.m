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

%
%% MAC
%
zProd   = mulVecRes; %cMul(z1, z2);          % 33-bit full-precision products
macRealCum   = cumsum(int64(real(zProd)));     % 64-bit integer running sum of real part
macImagCum   = cumsum(int64(imag(zProd)));     % 64-bit integer running sum of imag part
zzz = fi(complex(macRealCum, macImagCum), 1, 64, 0, F);
fid = fopen("complex_mac_test_vectors.h", "w");
fprintf(fid, "#ifndef COMPLEX_MAC_TEST_VECTORS_H\n#define COMPLEX_MAC_TEST_VECTORS_H\n\n");
fprintf(fid, "#include <stdint.h>\n\n");

writeComplexMulResultHex(fid, "MATLAB_MAC_CUMSUM_OUT", zzz);
% fprintf(fid, "static const complex_mul_result_t MATLAB_MAC_CUMSUM_OUT[%d] = {\n", numel(zProd));
% for i = 1:numel(zProd)
%     fprintf(fid, "    {%dLL, %dLL}", macRealCum(i), macImagCum(i));
%     if i < numel(zProd), fprintf(fid, ","); end
%     if mod(i, 2) == 0 || i == numel(zProd), fprintf(fid, "\n"); else, fprintf(fid, " "); end
% end
% fprintf(fid, "};\n\n#endif\n");
% fclose(fid);


%writeComplexInput16(fid, "inputA", zPos)


addpath('./complex_functions')

F = fimath( ...
    'OverflowAction', 'Wrap', ...
    'ProductMode',    'FullPrecision', ...
    'SumMode',        'FullPrecision');


% Pre-allocate 16-element fixed-point complex arrays
z5 = fi(zeros(16, 1), 1, 16, 0, F);
z6 = fi(zeros(16, 1), 1, 16, 0, F);

% --- Sequence 1 (Indices 1..4): Small Positive Sanity Check ---
% Running MAC: (-5 + 10j) -> (-5 + 15j) -> (1 + 24j) -> Final: (-7 + 24j)
z5(1) = makeComplex(1, 2);      z6(1) = makeComplex(3, 4);
z5(2) = makeComplex(2, 1);      z6(2) = makeComplex(1, 2);
z5(3) = makeComplex(3, 0);      z6(3) = makeComplex(2, 3);
z5(4) = makeComplex(0, 2);      z6(4) = makeComplex(0, 4);

% --- Sequence 2 (Indices 5..8): Mixed-Sign & Zero-Cancellation ---
% Running MAC: (-10 + 120j) -> (0 + 0j) -> (300 - 100j) -> Final: (400 - 400j)
z5(5) = makeComplex(-10,  20);  z6(5) = makeComplex( 5, -2);
z5(6) = makeComplex( 10, -20);  z6(6) = makeComplex( 5, -2);
z5(7) = makeComplex(-50, -50);  z6(7) = makeComplex(-2,  4);
z5(8) = makeComplex( 25,  25);  z6(8) = makeComplex(-4, -8);

% --- Sequence 3 (Indices 9..12): 33-Bit Positive Overflow Accumulation ---
% Running MAC: (0 + 2147483648j) -> (0 + 4294967296j) -> (0 + 6442450944j) -> Final: (0 + 8589934592j)
z5(9)  = makeComplex(-32768, -32768);  z6(9)  = makeComplex(-32768, -32768);
z5(10) = makeComplex(-32768, -32768);  z6(10) = makeComplex(-32768, -32768);
z5(11) = makeComplex(-32768, -32768);  z6(11) = makeComplex(-32768, -32768);
z5(12) = makeComplex(-32768, -32768);  z6(12) = makeComplex(-32768, -32768);

% --- Sequence 4 (Indices 13..16): Maximum Negative Accumulation ---
% Running MAC: (-2147385345 - 32767j) -> (-4294770690 - 65534j) -> (-6442156035 - 98301j) -> Final: (-8589541380 - 131068j)
z5(13) = makeComplex(-32768, 32767);   z6(13) = makeComplex(32767, 32767);
z5(14) = makeComplex(-32768, 32767);   z6(14) = makeComplex(32767, 32767);
z5(15) = makeComplex(-32768, 32767);   z6(15) = makeComplex(32767, 32767);
z5(16) = makeComplex(-32768, 32767);   z6(16) = makeComplex(32767, 32767);

% Compute products and per-block 4-element MAC results
zzProd = cMul(z5, z6);
for blk = 1:4
    idx = (blk-1)*4 + (1:4);
    blkReal = sum(int64(real(zzProd(idx))));
    blkImag = sum(int64(imag(zzProd(idx))));
    fprintf("Sequence %d Final MAC = %d + %dj\n", blk, blkReal, blkImag);
end
