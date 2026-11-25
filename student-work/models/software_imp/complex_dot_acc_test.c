// complex_dot_acc_test.c -- version 1 accelerator, dot product (ACMUL then AADD per element)
//
// Same inputs as complex_dot_sw64_test.c, but the parts are only 16 bits wide, so the
// answer WRAPS. Use this test for CYCLE COUNTS ONLY; the printed values are not the true sums.
#include <stdio.h>

#if defined(__riscv)
   #include <riscv-pk/encoding.h>
   #include "../../metrics.h"
#endif

#include <stdint.h>
#include "complex.h"
#include "complex_acc.h"

#define MAX_VECT  5000

int main(void) {
  complex_t z1[MAX_VECT];
  complex_t z2[MAX_VECT];

  for (int i = 0; i < MAX_VECT; i++) {
    int k = (i % 100) + 1;
    z1[i] = makeComplex(k, 2 * k);
    z2[i] = makeComplex(2 * k, 3 * k);
  }
#if defined(__riscv)
  hpm_setup();
#endif

  printf("########################################################################\n");
  printf("Accelerator v1: dot product (ACMUL + AADD), 16-bit parts, wraps         \n");
  printf("########################################################################\n");

  complex_t sum = makeComplex(0, 0);

#if defined(__riscv)
  BENCH_BEGIN();
  for (int i = 0; i < MAX_VECT; i++) {
    sum = cAddAcc(sum, cMulAcc(z1[i], z2[i]));
  }
  BENCH_END("acc_dot_v1");
#else
  for (int i = 0; i < MAX_VECT; i++) {
    sum = cAddAcc(sum, cMulAcc(z1[i], z2[i]));
  }
#endif

  // Reference using the plain software functions, same 16-bit wrap-around.
  complex_t ref = makeComplex(0, 0);
  for (int i = 0; i < MAX_VECT; i++) {
    ref = cAdd(ref, cMul(z1[i], z2[i]));
  }
  int mismatches = (sum.packedComplex != ref.packedComplex);

  printf("packed sum=0x%08x (wrapped, not the true sum)\n", (unsigned)sum.packedComplex);
  printf("mismatches vs 16-bit software=%d\n", mismatches);
  return mismatches != 0;
}

