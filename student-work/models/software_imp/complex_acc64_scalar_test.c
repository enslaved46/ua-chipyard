// complex_acc64_scalar_test.c -- quick correctness test, run this before any timing
//
// Each line compares the accelerator result (hw) with a second answer (sw) and with
// the number worked out by hand (expRe, expIm). All three must agree.
#include <stdio.h>
#include <stdint.h>
#include "complex_acc64.h"

static int check(const char *name, complex64_t hw, complex64_t sw,
                 int32_t expRe, int32_t expIm) {
  int ok = (hw.packed64 == sw.packed64) &&
           (getReal64(hw) == expRe) && (getImag64(hw) == expIm);
  printf("%-38s re=%d im=%d %s\n", name, (int)getReal64(hw), (int)getImag64(hw),
         ok ? "OK" : "MISMATCH");
  return !ok;
}

int main(void) {
  int bad = 0;

  // ---- arithmetic with numbers too big for 16 bits ----------------------------
  complex64_t a = makeComplex64(100000, 200000);
  complex64_t b = makeComplex64(50000, 70000);
  bad += check("add wide", cAddAcc64(a, b), cAdd64(a, b), 150000, 270000);

  complex64_t p = makeComplex64(1000, 2000);
  complex64_t q = makeComplex64(3000, 4000);
  bad += check("mul wide (1000+2000j)*(3000+4000j)",
               cMulAcc64(p, q), cMul64(p, q), -5000000, 10000000);

  complex64_t m = makeComplex64(-3, 5);
  complex64_t n = makeComplex64(2, -4);
  bad += check("mul negative (-3+5j)*(2-4j)",
               cMulAcc64(m, n), cMul64(m, n), 14, 22);

  complex64_t big = makeComplex64(2000000000, 0);
  bad += check("add wrap 2,000,000,000 twice",
               cAddAcc64(big, big), cAdd64(big, big), -294967296, 0);

  // ---- storage slots ----------------------------------------------------------
  cAccWrite64(1, makeComplex64(5, 6));
  bad += check("slot 1 read after write",
               cAccRead64(1), makeComplex64(5, 6), 5, 6);
  bad += check("slot 1 accumulate +(10+20j)",
               cAccAdd64(1, makeComplex64(10, 20)), makeComplex64(15, 26), 15, 26);
  cAccWrite64(2, makeComplex64(1, 1));
  bad += check("slot 1 unchanged by slot 2",
               cAccRead64(1), makeComplex64(15, 26), 15, 26);
  bad += check("slot 2 holds (1+1j)",
               cAccRead64(2), makeComplex64(1, 1), 1, 1);

  // ---- multiply, then accumulate: (1+2j)(5+6j) + (3+4j)(7+8j) = -18+68j --------
  cAccWrite64(0, makeComplex64(0, 0));
  cAccAdd64(0, cMulAcc64(makeComplex64(1, 2), makeComplex64(5, 6)));
  cAccAdd64(0, cMulAcc64(makeComplex64(3, 4), makeComplex64(7, 8)));
  bad += check("dot product kept in slot 0",
               cAccRead64(0), makeComplex64(-18, 68), -18, 68);

  printf("scalar64 test: %d mismatches\n", bad);
  return bad;
}
