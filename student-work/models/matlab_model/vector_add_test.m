clear;
clc;
addpath('./complex_functions')
%cAdd(1+1i,2+2i);

%z6 = fi(200 + 200i, 1, 16, 0, 'OverflowAction', 'Wrap');
%z7 = fi(200 + 200i, 1, 16, 0, 'OverflowAction', 'Wrap');

%z8 = cMul(z6, z7);
%disp(z8);
% makding vector 1 D array with signed type 16 bits no fraction and ound
% when overflow

VECTOR_LEN = 5000;
%est = makeComplex(1, 2);
%disp(test)
%dbstop if error

z1 = fi(zeros(VECTOR_LEN,1), 1, 16, 0, 'OverflowAction', 'Wrap');
z2 =  fi(zeros(VECTOR_LEN,1), 1, 16, 0, 'OverflowAction', 'Wrap');
z1 = fi(zeros(VECTOR_LEN,1), 1, 16, 0, 'OverflowAction', 'Wrap');
z2 =  fi(zeros(VECTOR_LEN,1), 1, 16, 0, 'OverflowAction', 'Wrap');

zPosRes = fi(zeros(VECTOR_LEN,1), 1, 16, 0, 'OverflowAction', 'Wrap');
zNegRes = fi(zeros(VECTOR_LEN,1), 1, 16, 0, 'OverflowAction', 'Wrap');

for i = 1:VECTOR_LEN
    z1(i) = makeComplex(i, i*2 );
    z2(i) = makeComplex(i*2, i*3 );
    %i
end

zPosRes = cAdd( z1, z2)
% can use both ways
% z3 = z1 + z2;
% isequal(zPos, z3)

for i = 1:VECTOR_LEN
    z3(i) = makeComplex(-i*10, -i*20 );
    z4(i) = makeComplex(-i*20, -i*30 );
    %i
end

zNegRes = cAdd( z3, z4);

fid = fopen("complex_add_test_vectors.h", "w");
fprintf(fid, "#ifndef COMPLEX_ADD_TEST_VECTORS_H\n");
fprintf(fid, "#define COMPLEX_ADD_TEST_VECTORS_H\n\n");
fprintf(fid, "#include <stdint.h>\n\n");
fprintf(fid, "#define COMPLEX_TEST_COUNT %d\n\n", VECTOR_LEN);

writeCArrayHex(fid, "matlab_input_a_pos", z1);
writeCArrayHex(fid, "matlab_input_b_pos", z2);
writeCArrayHex(fid, "matlab_expected_pos", zPosRes);


writeCArrayHex(fid, "matlab_input_a_neg", z3);
writeCArrayHex(fid, "matlab_input_b_neg", z4);
writeCArrayHex(fid, "matlab_expected_neg", zNegRes);


fprintf(fid, "#endif\n");
fclose(fid);


%writeCArrayHex(fid, "inputA", zPos)

function writeCArrayHex(fid, varName, arrayOfData)
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


