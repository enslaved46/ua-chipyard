// complex_dot_accW_test.c -- accelerator dot product with ACMAC (version 3)
//
// Same inputs as complex_dot_swW_test.c. Each element is ONE instruction (ACMAC) that
// multiplies and adds into slot 0 (real total) and slot 1 (imaginary total), both full
// 64-bit. At the end ACRD reads the two totals back.
// SCALE must match complex_dot_swW_test.c (1 normally, 1000 for the big-number run).
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

  // Reference answer (exact, 64-bit), computed before timing starts and not timed.
  int64_t exRe = 0;
  int64_t exIm = 0;
  for (int i = 0; i < MAX_VECT; i++) {
    int64_t a = getReal64(z1[i]);
    int64_t b = getImag64(z1[i]);
    int64_t c = getReal64(z2[i]);
    int64_t d = getImag64(z2[i]);
    exRe += a * c - b * d;
    exIm += a * d + b * c;
  }

#if defined(__riscv)
  hpm_setup();
#endif

  printf("########################################################################\n");
  printf("Accelerator: dot product with ACMAC, 64-bit totals (version 3)          \n");
  printf("########################################################################\n");

  int64_t hwRe;
  int64_t hwIm;

#if defined(__riscv)
  BENCH_BEGIN();
  cAccWriteW(0, 0);
  cAccWriteW(1, 0);
  for (int i = 0; i < MAX_VECT; i++) {
    cMacAccW(z1[i], z2[i]);
  }
  hwRe = cAccReadW(0);
  hwIm = cAccReadW(1);
  BENCH_END("acc_dotW");
#else
  cAccWriteW(0, 0);
  cAccWriteW(1, 0);
  for (int i = 0; i < MAX_VECT; i++) {
    cMacAccW(z1[i], z2[i]);
  }
  hwRe = cAccReadW(0);
  hwIm = cAccReadW(1);
#endif

  int mismatches = (hwRe != exRe) || (hwIm != exIm);

  printf("dot re=%ld im=%ld\n", (long)hwRe, (long)hwIm);
  printf("mismatches=%d\n", mismatches);
  return mismatches != 0;
}
