function writeComplexMulResultHex(fid, varName, arrayOfData)
    fprintf(fid, "static const complex_mul_result_t %s[%d] = {\n", varName, numel(arrayOfData));

    for i = 1:numel(arrayOfData)
        % Reinterpret the 64-bit two's complement signed bits as unsigned 64-bit for hex printing
        realHex = typecast(int64(real(arrayOfData(i))), 'uint64');
        imagHex = typecast(int64(imag(arrayOfData(i))), 'uint64');

        fprintf(fid, "    {(int64_t)0x%016XULL, (int64_t)0x%016XULL}", realHex, imagHex);

        if i < numel(arrayOfData)
            fprintf(fid, ",");
        end

        if mod(i, 2) == 0 || i == numel(arrayOfData)
            fprintf(fid, "\n");
        else
            fprintf(fid, " ");
        end
    end

    fprintf(fid, "};\n\n");
end
