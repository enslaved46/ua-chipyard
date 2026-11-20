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