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

zNegRes = cAdd( z3, z4);

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

function writeComplexInput16(fid, varName, arrayOfData)
%disp(arrayOfData)
fprintf(fid, "static const uint32_t %s[%d] = {\n",varName, numel(arrayOfData));

for i = 1:numel(arrayOfData)
    realPart = double(real(arrayOfData(i)));
    imagPart = double(imag(arrayOfData(i)));

    realBits = uint32(mod(realPart, 65536));
    imagBits = uint32(mod(imagPart, 65536));


    %fprintf("loop %d : %d\n", i,realBits);
    packedValue = bitor(bitshift(realBits, 16),imagBits);

    fprintf(fid, "    0x%08X", packedValue);
    

    if i < numel(arrayOfData)
        fprintf(fid, ",");
    end

    if mod(i, 4) == 0 || i == numel(arrayOfData)
        fprintf(fid, "\n");
    else
        fprintf(fid, " ");
    end
end

fprintf(fid, "};\n\n");
end



function writeComplexOut17(fid, varName, arrayOfData)
%disp(arrayOfData)
fprintf(fid, "static const uint32_t %s[%d] = {\n",varName, numel(arrayOfData));

for i = 1:numel(arrayOfData)
    realPart = double(real(arrayOfData(i)));
    imagPart = double(imag(arrayOfData(i)));
    
    % 131072 = 2^17; converts signed 17-bit (-65536..65534) to 17-bit two's complement
    realBits = uint64(mod(realPart, 131072));
    imagBits = uint64(mod(imagPart, 131072));
 fprintf("loop %d : %d\n", i,realBits);

    % Pack real into bits [33:17] and imag into bits [16:0]
    packedValue = bitor(bitshift(realBits, 17), imagBits);
   fprintf(fid, "     0x%016X", packedValue);

    if i < numel(arrayOfData)
        fprintf(fid, ",");
    end

    if mod(i, 4) == 0 || i == numel(arrayOfData)
        fprintf(fid, "\n");
    else
        fprintf(fid, " ");
    end
end

fprintf(fid, "};\n\n");
end