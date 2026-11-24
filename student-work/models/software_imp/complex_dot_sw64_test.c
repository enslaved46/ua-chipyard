// complex_dot_sw64_test.c -- software baseline: dot product (sum of complex products)
//
// sum = z1[0]*z2[0] + z1[1]*z2[1] + ... done with plain C, no accelerator.
// The inputs stay small (k runs 1 to 100) so the true sum fits in 32 bits and does not wrap.
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

  for (int i = 0; i < MAX_VECT; i++) {
    int k = (i % 100) + 1;
    z1[i] = makeComplex64(k, 2 * k);
    z2[i] = makeComplex64(2 * k, 3 * k);
  }
#if defined(__riscv)
  hpm_setup();
#endif

  printf("########################################################################\n");
  printf("Software: dot product of complex numbers (cMul64 + cAdd64)              \n");
  printf("########################################################################\n");

  complex64_t sum = makeComplex64(0, 0);

#if defined(__riscv)
  BENCH_BEGIN();
  for (int i = 0; i < MAX_VECT; i++) {
    sum = cAdd64(sum, cMul64(z1[i], z2[i]));
  }
  BENCH_END("sw_dot64");
#else
  for (int i = 0; i < MAX_VECT; i++) {
    sum = cAdd64(sum, cMul64(z1[i], z2[i]));
  }
#endif

  printf("dot re=%d im=%d\n", (int)getReal64(sum), (int)getImag64(sum));
  return 0;
}
