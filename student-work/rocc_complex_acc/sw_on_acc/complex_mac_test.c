#include <stdio.h>
#include <stdint.h>

#include "../software_includes/complex.h"
#include "../software_includes/rocc_complex.h"
#include "../software_includes/complex_mul_test_vector.h"
#include "../software_includes/complex_test_hlpr.h"

#if defined(__riscv)
  #include <riscv-pk/encoding.h>

  #include "../software_includes/benchmark_dependencies/metrics.h"
#endif

#define TEST_CMAC 1
#define PRINT_MAC_RESULT 0

#define ARRAY_BASED_SCALAR_ACC_VECTOR_MAC_TEST 1
#define ELEMENT_WISE_SCALAR_ACC_VECTOR_MAC_TEST 1

int main(){
  const int16_t MAX_POS = 32767;
  const int16_t MAX_NEG = -32768;
  // check corner cases
 complex_t cornerA[16] = {
        // Seq 1: small positive -> Expected: -7 + 24j
        makeComplex16(1, 2),
        makeComplex16(2, 1),
        makeComplex16(3, 0),
        makeComplex16(0, 2),

        // Seq 2: mixed sign & zero cancellation -> Expected: 400 - 400j
        makeComplex16(-10,  20),
        makeComplex16( 10, -20),
        makeComplex16(-50, -50),
        makeComplex16( 25,  25),

        // Seq 3: 33-bit positive overflow -> Expected: 0 + 8589934592j
        makeComplex16(MAX_NEG, MAX_NEG),
        makeComplex16(MAX_NEG, MAX_NEG),
        makeComplex16(MAX_NEG, MAX_NEG),
        makeComplex16(MAX_NEG, MAX_NEG),

        // Seq 4: 33-bit negative accumulation -> Expected: -8589541380 - 131068j
        makeComplex16(MAX_NEG, MAX_POS),
        makeComplex16(MAX_NEG, MAX_POS),
        makeComplex16(MAX_NEG, MAX_POS),
        makeComplex16(MAX_NEG, MAX_POS)
   };

  complex_t cornerB[16] = {
        // Seq 1
        makeComplex16(3, 4),
        makeComplex16(1, 2),
        makeComplex16(2, 3),
        makeComplex16(0, 4),

        // Seq 2
        makeComplex16( 5, -2),
        makeComplex16( 5, -2),
        makeComplex16(-2,  4),
        makeComplex16(-4, -8),

        // Seq 3
        makeComplex16(MAX_NEG, MAX_NEG),
        makeComplex16(MAX_NEG, MAX_NEG),
        makeComplex16(MAX_NEG, MAX_NEG),
        makeComplex16(MAX_NEG, MAX_NEG),

        // Seq 4
        makeComplex16(MAX_POS, MAX_POS),
        makeComplex16(MAX_POS, MAX_POS),
        makeComplex16(MAX_POS, MAX_POS),
        makeComplex16(MAX_POS, MAX_POS)
    };

    complex_mul_result_t cornerExpected[4] = {
        makeComplex33(-7LL,          24LL),
        makeComplex33(400LL,        -400LL),
        makeComplex33(0LL,           8589934592LL),
        makeComplex33(-8589541380LL, -131068LL)
    };

  // MAC TEST, group of 4
  if (TEST_CMUL == 1) {
    if (DEBUG_PRINT) {
      printf("########################################################################\n");
      printf("Hardware Implementation : Running Complex MAC TEST                      \n");
      printf("########################################################################\n");
    }

    BENCH_BEGIN();
    volatile  complex_mul_result_t m10 = roccCmplxMul(z6, z6); // (50 + 50j)*(50 + 50j) = 0 + 5000j
    BENCH_END("scalar_complex_mul");
    cornerFailures += checkSingleMac("Corner Seq 1 (Small Pos)   :", seq1Res, cornerExpected[0]);
    cornerFailures += checkSingleMac("Corner Seq 2 (Zero Cancel) :", seq2Res, cornerExpected[1]);
    cornerFailures += checkSingleMac("Corner Seq 3 (>33b Pos)    :", seq3Res, cornerExpected[2]);
    cornerFailures += checkSingleMac("Corner Seq 4 (>33b Neg)    :", seq4Res, cornerExpected[3]);
}
