/* complex_acc_scalar_test.c -- quick correctness check of ACMUL / AADD
 * Each line prints the accelerator result next to the software result.
 * "OK" means they match. Run on ComplexNumAccConfig.
 */
#include <stdio.h>
#include <stdint.h>
#include "complex.h"
#include "complex_acc.h"

static int check(const char *name, complex_t hw, complex_t sw) {
  int ok = (hw.packedComplex == sw.packedComplex);
  printf("%-28s hw=0x%x sw=0x%x %s\n", name,
         (unsigned)hw.packedComplex, (unsigned)sw.packedComplex,
         ok ? "OK" : "MISMATCH");
  return !ok;
}

int main(void) {
  int bad = 0;
  complex_t a = makeComplex(150, 250);
  complex_t m = makeComplex(-3, 5);
  complex_t p = makeComplex(1, 2);
  complex_t q = makeComplex(3, 4);
  complex_t big = makeComplex(20000, 20000);

  bad += check("add (150+250j)+(150+250j)", cAddAcc(a, a), cAdd(a, a));
  bad += check("mul (150+250j)*(150+250j)", cMulAcc(a, a), cMul(a, a));
  bad += check("add (-3+5j)+(-3+5j)",       cAddAcc(m, m), cAdd(m, m));
  bad += check("mul (1+2j)*(3+4j)",         cMulAcc(p, q), cMul(p, q));
  bad += check("add wrap 20000+20000",      cAddAcc(big, big), cAdd(big, big));

  printf("scalar test: %d mismatches\n", bad);
  return bad;
}
