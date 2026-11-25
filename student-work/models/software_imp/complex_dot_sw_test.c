// complex_dot_sw_test.c -- version 1 software dot product, plain C, 16-bit parts (cAdd + cMul)
#include <stdio.h>

#if defined(__riscv)
   #include <riscv-pk/encoding.h>
   #include "../../metrics.h"
#endif

#include <stdint.h>
#include "complex.h"

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
  printf("Software v1: dot product (cAdd + cMul), 16-bit parts, wraps             \n");
  printf("########################################################################\n");

  complex_t sum = makeComplex(0, 0);

#if defined(__riscv)
  BENCH_BEGIN();
  for (int i = 0; i < MAX_VECT; i++) {
    sum = cAdd(sum, cMul(z1[i], z2[i]));
  }
  BENCH_END("sw_dot_v1");
#else
  for (int i = 0; i < MAX_VECT; i++) {
    sum = cAdd(sum, cMul(z1[i], z2[i]));
  }
#endif

  printf("packed sum=0x%08x\n", (unsigned)sum.packedComplex);
  return 0;
}

