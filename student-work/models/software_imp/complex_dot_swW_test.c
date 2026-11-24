// complex_dot_swW_test.c -- software baseline for version 3: exact dot product in plain C
//
// sum = z1[0]*z2[0] + z1[1]*z2[1] + ... with two int64 running totals (real and imaginary),
// so the software answer is exact too. No accelerator.
// SCALE multiplies every input. Leave it at 1 for the normal run (matches the earlier tests).
// Set it to 1000 to make the true sum bigger than 32 bits.
#include <stdio.h>

#if defined(__riscv)
   #include <riscv-pk/encoding.h>
   #include "../../metrics.h"
#endif

#include <stdint.h>
#include "complex_accw.h"

#define MAX_VECT  5000
#define SCALE     1

int main(void) {
  complex64_t z1[MAX_VECT];
  complex64_t z2[MAX_VECT];

  for (int i = 0; i < MAX_VECT; i++) {
    int k = ((i % 100) + 1) * SCALE;
    z1[i] = makeComplex64(k, 2 * k);
    z2[i] = makeComplex64(2 * k, 3 * k);
  }
#if defined(__riscv)
  hpm_setup();
#endif

  printf("########################################################################\n");
  printf("Software: exact dot product with 64-bit totals (version 3 baseline)     \n");
  printf("########################################################################\n");

  int64_t sumRe = 0;
  int64_t sumIm = 0;

#if defined(__riscv)
  BENCH_BEGIN();
  for (int i = 0; i < MAX_VECT; i++) {
    int64_t a = getReal64(z1[i]);
    int64_t b = getImag64(z1[i]);
    int64_t c = getReal64(z2[i]);
    int64_t d = getImag64(z2[i]);
    sumRe += a * c - b * d;
    sumIm += a * d + b * c;
  }
  BENCH_END("sw_dotW");
#else
  for (int i = 0; i < MAX_VECT; i++) {
    int64_t a = getReal64(z1[i]);
    int64_t b = getImag64(z1[i]);
    int64_t c = getReal64(z2[i]);
    int64_t d = getImag64(z2[i]);
    sumRe += a * c - b * d;
    sumIm += a * d + b * c;
  }
#endif

  printf("dot re=%ld im=%ld\n", (long)sumRe, (long)sumIm);
  return 0;
}
