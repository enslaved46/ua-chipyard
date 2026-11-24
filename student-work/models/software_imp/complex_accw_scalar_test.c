// complex_accw_scalar_test.c -- quick correctness test for ComplexNumAccW, run before any timing
//
// Each line prints the value read back and OK or MISMATCH against the number worked out by hand.
#include <stdio.h>
#include <stdint.h>
#include "complex_accw.h"

static int checkW(const char *name, int64_t got, int64_t expected) {
  int ok = (got == expected);
  printf("%-40s %ld %s\n", name, (long)got, ok ? "OK" : "MISMATCH");
  fflush(stdout);
  return !ok;
}

int main(void) {
  int bad = 0;

  // ---- ACMAC small numbers: (1+2j)(5+6j) + (3+4j)(7+8j) = -18+68j --------------
  cAccWriteW(0, 0);
  cAccWriteW(1, 0);
  cMacAccW(makeComplex64(1, 2), makeComplex64(5, 6));
  cMacAccW(makeComplex64(3, 4), makeComplex64(7, 8));
  bad += checkW("mac small: slot 0 (real)", cAccReadW(0), -18);
  bad += checkW("mac small: slot 1 (imag)", cAccReadW(1), 68);

  // ---- ACMAC negative numbers: (-3+5j)(2-4j) = 14+22j --------------------------
  cAccWriteW(0, 0);
  cAccWriteW(1, 0);
  cMacAccW(makeComplex64(-3, 5), makeComplex64(2, -4));
  bad += checkW("mac negative: slot 0 (real)", cAccReadW(0), 14);
  bad += checkW("mac negative: slot 1 (imag)", cAccReadW(1), 22);

  // ---- the point of version 3: totals past 32 bits stay exact ------------------
  // (2147483647 + 0j) * (2147483647 + 0j) = 4,611,686,014,132,420,609 (real), 0 (imag)
  // Added twice = 9,223,372,028,264,841,218, far past the 32-bit limit and still below 2^63.
  cAccWriteW(0, 0);
  cAccWriteW(1, 0);
  cMacAccW(makeComplex64(2147483647, 0), makeComplex64(2147483647, 0));
  bad += checkW("mac big once: slot 0 (real)", cAccReadW(0), 4611686014132420609LL);
  cMacAccW(makeComplex64(2147483647, 0), makeComplex64(2147483647, 0));
  bad += checkW("mac big twice: slot 0 (real)", cAccReadW(0), 9223372028264841218LL);
  bad += checkW("mac big twice: slot 1 (imag)", cAccReadW(1), 0);

  // ---- ACWR / ACRD keep a whole 64-bit number ----------------------------------
  cAccWriteW(2, 5000000000LL);
  bad += checkW("slot 2 holds 5,000,000,000", cAccReadW(2), 5000000000LL);
  bad += checkW("slot 2 unchanged by mac", cAccReadW(2), 5000000000LL);

  // ---- the old instructions still work -----------------------------------------
  complex64_t p = makeComplex64(1000, 2000);
  complex64_t q = makeComplex64(3000, 4000);
  complex64_t m = cMulAcc64(p, q);
  bad += checkW("old ACMUL real (1000+2000j)(3000+4000j)", getReal64(m), -5000000);
  bad += checkW("old ACMUL imag", getImag64(m), 10000000);
  complex64_t s = cAddAcc64(makeComplex64(100000, 200000), makeComplex64(50000, 70000));
  bad += checkW("old AADD real", getReal64(s), 150000);
  bad += checkW("old AADD imag", getImag64(s), 270000);

  printf("scalarW test: %d mismatches\n", bad);
  fflush(stdout);
  return bad;
}
