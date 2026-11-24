// complex_vec_mul_acc64_test.c -- vector multiply with ACMUL on the 32-bit-part accelerator
//
// Same job and same inputs as version 1's vector multiply test (z1[i] = i + 2i j,
// z2[i] = 2i + 3i j, i = 1..N), so the cycle counts can be compared with the original
// software baseline and with version 1. Parts are 32 bits, so the results are exact up to
// N = 5000 (largest part is about 175 million). Version 3 keeps ACMUL unchanged, so this
// same file is run on the version 2 simulator and on the version 3 simulator.
#include <stdio.h>

#if defined(__riscv)
   #include <riscv-pk/encoding.h>
   #include "../../metrics.h"
#endif

#include <stdint.h>
#include "complex_acc64.h"

#define MAX_VECT  5000

int main(void) {
  complex64_t z1[MAX_VECT];
  complex64_t z2[MAX_VECT];
  complex64_t result[MAX_VECT];

  for (int i = 1; i <= MAX_VECT; i++) {
    z1[i-1] = makeComplex64(i, i * 2);
    z2[i-1] = makeComplex64(i * 2, i * 3);
  }
#if defined(__riscv)
  hpm_setup();
#endif

  printf("########################################################################\n");
  printf("Accelerator v2/v3: vector multiply (ACMUL), 32-bit parts                \n");
  printf("########################################################################\n");

#if defined(__riscv)
  BENCH_BEGIN();
  for (int i = 0; i < MAX_VECT; i++) {
    result[i] = cMulAcc64(z1[i], z2[i]);
  }
  BENCH_END("acc_vec_mul64");
#else
  for (int i = 0; i < MAX_VECT; i++) {
    result[i] = cMulAcc64(z1[i], z2[i]);
  }
#endif

  int mismatches = 0;
  for (int i = 0; i < MAX_VECT; i++) {
    complex64_t expected = cMul64(z1[i], z2[i]);
    if (expected.packed64 != result[i].packed64) mismatches++;
  }
  printf("mismatches=%d\n", mismatches);
  return mismatches != 0;
}

