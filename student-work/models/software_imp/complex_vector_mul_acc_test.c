/* Accelerator version of complex_vector_mul_test.c.
 * Same inputs, same timed region, same N. The loop calls ACMUL (cVectMulAcc).
 * After the timed region it checks every output against the software cMul.
 */
#include <stdio.h>

#if defined(__riscv)
   #include <riscv-pk/encoding.h>
   #include "../../metrics.h"
#endif

#include <stdint.h>
#include "complex.h"
#include "complex_acc.h"

#define MAX_VECT  50

int main(void) {
  complex_t z1[MAX_VECT] = {0};
  complex_t z2[MAX_VECT] = {0};
  complex_t mulResult[MAX_VECT] = {0};

  for (int i = 1; i <= MAX_VECT; i++) {
    z1[i-1] = makeComplex(i, i*2);
    z2[i-1] = makeComplex(i*2, i*3);
  }
#if defined(__riscv)
  hpm_setup();
#endif

  printf("########################################################################\n");
  printf("Accelerator: Vector Multiplication of complex numbers (ACMUL)           \n");
  printf("########################################################################\n");

#if defined(__riscv)
  BENCH_BEGIN();
  cVectMulAcc(z1, z2, mulResult, MAX_VECT);
  BENCH_END("acc_vector_mul");
#else
  cVectMulAcc(z1, z2, mulResult, MAX_VECT);
#endif

  int mismatches = 0;
  for (int i = 0; i < MAX_VECT; i++) {
    complex_t expected = cMul(z1[i], z2[i]);
    if (expected.packedComplex != mulResult[i].packedComplex) mismatches++;
  }
  printf("mismatches=%d\n", mismatches);
  return mismatches != 0;
}
