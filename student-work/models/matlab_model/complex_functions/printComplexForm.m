function printComplexForm(z)
realPart = double(real(z));
imagPart = double(imag(z));

if imagPart < 0
    fprintf("%d - %di", realPart, -imagPart);
else
    fprintf("%d + %di", realPart, imagPart);
end
end