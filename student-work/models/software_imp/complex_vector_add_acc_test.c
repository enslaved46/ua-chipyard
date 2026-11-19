/* Accelerator version of complex_vector_add_test.c.
 * Same inputs, same timed region (BENCH_BEGIN / BENCH_END), same N.
 * The only change: the loop calls the AADD instruction (cVectAddAcc).
 * After the timed region it checks every output against the software cAdd.
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
  complex_t addResult[MAX_VECT] = {0};

  for (int i = 1; i <= MAX_VECT; i++) {
    z1[i-1] = makeComplex(i, i*2);
    z2[i-1] = makeComplex(i*2, i*3);
  }

  printf("########################################################################\n");
  printf("Accelerator: Vector Addition of complex numbers (AADD)                  \n");
  printf("########################################################################\n");

#if defined(__riscv)
  BENCH_BEGIN();
  cVectAddAcc(z1, z2, addResult, MAX_VECT);
  BENCH_END("acc_vector_add");
#else
  cVectAddAcc(z1, z2, addResult, MAX_VECT);
#endif

  /* correctness: compare each element with the software cAdd */
  int mismatches = 0;
  for (int i = 0; i < MAX_VECT; i++) {
    complex_t expected = cAdd(z1[i], z2[i]);
    if (expected.packedComplex != addResult[i].packedComplex) mismatches++;
  }
  printf("mismatches=%d\n", mismatches);
  return mismatches != 0;
}
