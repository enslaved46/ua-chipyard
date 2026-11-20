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