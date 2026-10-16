function z3 = cAdd(z1 , z2)
  z3 =  fi(z1+z2, 1, 16, 0, z1.fimath);
  %z3 =  fi(z1+z2,1,16,0,'OverflowAction', 'Wrap');
end