// complex_dot_acc64_test.c -- accelerator dot product using a storage slot
//
// Same inputs and same sum as complex_dot_sw64_test.c, but each product comes from
// ACMUL and each add goes into slot 0 with ACACC. The running total never leaves the
// accelerator until the end, when ACRD reads it back.
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

  // Reference answers, computed before timing starts and not timed.
  //   swSum = the plain C result (wraps at 32 bits)
  //   exRe, exIm = the true sum with no wrapping (64-bit)
  complex64_t swSum = makeComplex64(0, 0);
  int64_t exRe = 0;
  int64_t exIm = 0;
  for (int i = 0; i < MAX_VECT; i++) {
    swSum = cAdd64(swSum, cMul64(z1[i], z2[i]));
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
  printf("Accelerator: dot product of complex numbers (ACMUL + ACACC in slot 0)   \n");
  printf("########################################################################\n");

  complex64_t hwSum;

#if defined(__riscv)
  BENCH_BEGIN();
  cAccWrite64(0, makeComplex64(0, 0));
  for (int i = 0; i < MAX_VECT; i++) {
    complex64_t prod = cMulAcc64(z1[i], z2[i]);
    cAccAdd64(0, prod);
  }
  hwSum = cAccRead64(0);
  BENCH_END("acc_dot64");
#else
  cAccWrite64(0, makeComplex64(0, 0));
  for (int i = 0; i < MAX_VECT; i++) {
    complex64_t prod = cMulAcc64(z1[i], z2[i]);
    cAccAdd64(0, prod);
  }
  hwSum = cAccRead64(0);
#endif

  int mismatches = (hwSum.packed64 != swSum.packed64);
  int wrapped = (exRe != (int64_t)getReal64(hwSum)) || (exIm != (int64_t)getImag64(hwSum));

  printf("dot re=%d im=%d\n", (int)getReal64(hwSum), (int)getImag64(hwSum));
  printf("mismatches=%d\n", mismatches);
  printf("wrapped=%d\n", wrapped);
  return mismatches != 0;
}
