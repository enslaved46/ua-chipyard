clear;
clc;

N = 256;
rng(562);  % Reproducible vectors

MIN16 = -32768;
MAX16 = 32767;

% Generate random signed 16-bit components.
aReal = randi([MIN16, MAX16], N, 1);
aImag = randi([MIN16, MAX16], N, 1);
bReal = randi([MIN16, MAX16], N, 1);
bImag = randi([MIN16, MAX16], N, 1);

% Explicit boundary cases.
aReal(1) =  MAX16; aImag(1) =  MAX16;
bReal(1) =  1;     bImag(1) =  1;

aReal(2) =  MIN16; aImag(2) =  MIN16;
bReal(2) = -1;     bImag(2) = -1;

aReal(3) =  MAX16; aImag(3) =  MIN16;
bReal(3) =  1;     bImag(3) = -1;

aReal(4) =  MIN16; aImag(4) =  MAX16;
bReal(4) = -1;     bImag(4) =  1;

% Signed 16-bit wrapping.
wrap16 = @(x) mod(double(x) + 32768, 65536) - 32768;

expectedReal = wrap16(aReal + bReal);
expectedImag = wrap16(aImag + bImag);

% Pack real[31:16], imaginary[15:0].
packComplex = @(r, im) ...
    bitor( ...
    bitshift(uint32(mod(double(r), 65536)), 16), ...
    uint32(mod(double(im), 65536)));

inputA = packComplex(aReal, aImag);
inputB = packComplex(bReal, bImag);
expected = packComplex(expectedReal, expectedImag);

fid = fopen("complex_add_test_vectors.h", "w");

fprintf(fid, "#ifndef COMPLEX_ADD_TEST_VECTORS_H\n");
fprintf(fid, "#define COMPLEX_ADD_TEST_VECTORS_H\n\n");
fprintf(fid, "#include <stdint.h>\n\n");
fprintf(fid, "#define COMPLEX_TEST_COUNT %d\n\n", N);

writeCArray(fid, "matlab_input_a", inputA);
writeCArray(fid, "matlab_input_b", inputB);
writeCArray(fid, "matlab_expected", expected);

fprintf(fid, "#endif\n");
fclose(fid);

fprintf("Generated %d MATLAB test vectors\n", N);

function writeCArray(fid, name, values)
fprintf(fid, "static const uint32_t %s[%d] = {\n", ...
    name, numel(values));

for i = 1:numel(values)
    fprintf(fid, "    0x%08Xu", values(i));

    if i < numel(values)
        fprintf(fid, ",");
    end

    if mod(i, 4) == 0 || i == numel(values)
        fprintf(fid, "\n");
    else
        fprintf(fid, " ");
    end
end

fprintf(fid, "};\n\n");
end