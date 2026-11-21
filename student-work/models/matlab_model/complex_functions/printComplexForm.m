function printComplexForm(z)
realPart = double(real(z));
imagPart = double(imag(z));

if imagPart < 0
    fprintf("%d - %di\n", realPart, -imagPart);
else
    fprintf("%d + %di\n", realPart, imagPart);
end
end