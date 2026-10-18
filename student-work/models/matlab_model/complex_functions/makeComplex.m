function cmplx  = makeComplex(r, im)
      cmplx = fi(complex(r,im), 1,16,0,'OverflowAction', 'Wrap');
     % disp(cmplx)
end

