// complex_vec_add_acc64_test.c -- vector add with AADD on the 32-bit-part accelerator
//
// Same job and inputs as version 1's vector add test (z1[i] = i + 2i j, z2[i] = 2i + 3i j,
// i = 1..N), so the cycle counts can be compared with the original software add baseline and
// with version 1. Like version 1's add test, it does not call hpm_setup, so the extra
// counters (branch, load, cache) read 0; cycles and instructions are valid.
// AADD is unchanged in version 3, so this one file runs on the version 2 and version 3 simulators.
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

  printf("########################################################################\n");
  printf("Accelerator v2/v3: vector add (AADD), 32-bit parts                      \n");
  printf("########################################################################\n");

#if defined(__riscv)
  BENCH_BEGIN();
  for (int i = 0; i < MAX_VECT; i++) {
    result[i] = cAddAcc64(z1[i], z2[i]);
  }
  BENCH_END("acc_vec_add64");
#else
  for (int i = 0; i < MAX_VECT; i++) {
    result[i] = cAddAcc64(z1[i], z2[i]);
  }
#endif

  int mismatches = 0;
  for (int i = 0; i < MAX_VECT; i++) {
    complex64_t expected = cAdd64(z1[i], z2[i]);
    if (expected.packed64 != result[i].packed64) mismatches++;
  }
  printf("mismatches=%d\n", mismatches);
  return mismatches != 0;
}

