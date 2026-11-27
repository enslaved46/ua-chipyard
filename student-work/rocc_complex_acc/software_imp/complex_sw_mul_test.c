#include <stdio.h>
#include <stdint.h>

#include "../software_includes/complex.h"
#include "../software_includes/complex_software.h"
#include "../software_includes/complex_mul_test_vector.h"
#include "../software_includes/complex_test_hlpr.h"

#if defined(__riscv)
  #include <riscv-pk/encoding.h>
  #include "../software_includes/benchmark_dependencies/metrics.h"
#endif

#define TEST_CMUL 1
#define PRINT_MUL_RESULT 0

#define ARRAY_BASED_SCALAR_SW_VECTOR_MUL_TEST 1
#define ELEMENT_WISE_SCALAR_SW_VECTOR_MUL_TEST 1

int main(void) {
    hpm_setup();
  printf("########################################################################\n");
  printf("Welcome to Complex Number Scalar Arithmetic Test : Software Baseline    \n");
  printf("########################################################################\n");
  fflush(stdout);

  // --- Complex Multiplication Variables & Golden Vector Loading ---
  int8_t scalarMulFailure = 0, totalScalarMulRun = 0;
  int32_t elementWiseScalarMulFailCnt = 0;
  int32_t arrayBasedScalarMulFailCnt  = 0;

  // Use 'static' so large arrays live in .bss RAM and don't overflow the bare-metal stack
  static complex_t matlabMulInputA[MAX_VECT];
  static complex_t matlabMulInputB[MAX_VECT];
  static complex_mul_result_t matlabMulExpected[MAX_VECT];

  for (int i = 0; i < MAX_VECT; i++) {
    matlabMulInputA[i]   = cmplxFrmPacked(MATLAB_MUL_INPUT_A[i]);
    matlabMulInputB[i]   = cmplxFrmPacked(MATLAB_MUL_INPUT_B[i]);
    matlabMulExpected[i] = MATLAB_MUL_EXPECTED_OUT[i];
  }

  const int16_t MAX_POS = 32767;
  const int16_t MAX_NEG = -32768;

  // Extreme corner test points
  const complex_t z2 = makeComplex16(MAX_POS, MAX_POS); // (32767 + 32767j) -> 32-bit boundary
  const complex_t z4 = makeComplex16(MAX_NEG, MAX_NEG); // (-32768 - 32768j) -> 33-bit exact max

  // Standard quadrant test points
  complex_t z6 = makeComplex16(  50,   50); // 1st quadrant
  complex_t z7 = makeComplex16(-500,  500); // 2nd quadrant
  complex_t z8 = makeComplex16(-100, -500); // 3rd quadrant

  // 1. Scalar Software Complex Multiplication Test
  if (TEST_CMUL == 1) {
    if (DEBUG_PRINT) {
      printf("########################################################################\n");
      printf("Software Implementation : Running Scalar Test for Complex Multiplication\n");
      printf("########################################################################\n");
    }

    BENCH_BEGIN();
    volatile complex_mul_result_t m10 = cSwMul(z6, z6); // (50 + 50j)*(50 + 50j) = 0 + 5000j
    BENCH_END("scalar_complex_mul_sw");

    complex_mul_result_t m11 = cSwMul(z7, z7); // (-500 + 500j)*(-500 + 500j) = 0 - 500000j
    complex_mul_result_t m12 = cSwMul(z8, z8); // (-100 - 500j)*(-100 - 500j) = -240000 + 100000j
    complex_mul_result_t m13 = cSwMul(z2, z2); // (32767 + 32767j)^2 = 0 + 2147352578j
    complex_mul_result_t m00 = cSwMul(z4, z4); // (-32768 - 32768j)^2 = 0 + 2147483648j

    if (PRINT_MUL_RESULT) {
      printComplexMulResult(m10); printf("\n");
      printComplexMulResult(m11); printf("\n");
      printComplexMulResult(m12); printf("\n");
      printComplexMulResult(m13); printf("\n");
      printComplexMulResult(m00); printf("\n");
    }

    complex_mul_result_t mulTst1Res = makeComplex33(0, 5000);
    complex_mul_result_t mulTst2Res = makeComplex33(0, -500000);
    complex_mul_result_t mulTst3Res = makeComplex33(-240000, 100000);
    complex_mul_result_t mulTst4Res = makeComplex33(0, 2147352578LL);
    complex_mul_result_t mulTst5Res = makeComplex33(0, 2147483648LL);

    int8_t mr1 = checkMulResults("Software Scalar Mul :           ", z6, "*", z6, m10, mulTst1Res);
    int8_t mr2 = checkMulResults("Software Scalar Mul :           ", z7, "*", z7, m11, mulTst2Res);
    int8_t mr3 = checkMulResults("Software Scalar Mul :           ", z8, "*", z8, m12, mulTst3Res);
    int8_t mr4 = checkMulResults("Software Scalar Mul : 32b Max   ", z2, "*", z2, m13, mulTst4Res);
    int8_t mr5 = checkMulResults("Software Scalar Mul : 33b Exact ", z4, "*", z4, m00, mulTst5Res);

    scalarMulFailure  = mr1 + mr2 + mr3 + mr4 + mr5;
    totalScalarMulRun = 5;
  }

  // 2. Whole-Array Software Vector Multiplication Test
  if (ARRAY_BASED_SCALAR_SW_VECTOR_MUL_TEST) {
    static complex_mul_result_t mulResArr[MAX_VECT];

    if (DEBUG_PRINT) {
      printf("########################################################################\n");
      printf("Array Based Vector Mul Test : Software Implementation                   \n");
      printf("Running Vector Test for first %d Vectors                                \n", MAX_VECT);
      printf("########################################################################\n");
    }

    BENCH_BEGIN();
    cVectMul(matlabMulInputA, matlabMulInputB, mulResArr, MAX_VECT);
    BENCH_END("array_based_vector_mul_sw");

    for (int i = 0; i < MAX_VECT; i++) {
      arrayBasedScalarMulFailCnt += checkMulResults(
          "Software Implementation : Array-Based Vector Mul Result :      ",
          matlabMulInputA[i], "*", matlabMulInputB[i],
          mulResArr[i], matlabMulExpected[i]
      );
    }
  }

  // 3. Element-Wise Software Vector Multiplication Test
  if (ELEMENT_WISE_SCALAR_SW_VECTOR_MUL_TEST) {
    static complex_mul_result_t swElemMulResults[MAX_VECT];

    if (DEBUG_PRINT) {
      printf("########################################################################\n");
      printf("Element Wise Vector Mul Test : Software Implementation                  \n");
      printf("Running Vector Test for first %d Vectors                                \n", MAX_VECT);
      printf("########################################################################\n");
    }

    BENCH_BEGIN();
    for (int i = 0; i < MAX_VECT; i++) {
      swElemMulResults[i] = cSwMul(matlabMulInputA[i], matlabMulInputB[i]);
    }
    BENCH_END("element_wise_vector_mul_sw");

    for (int i = 0; i < MAX_VECT; i++) {
      elementWiseScalarMulFailCnt += checkMulResults(
          "Software Implementation : Element-Wise Vector Mul Result :     ",
          matlabMulInputA[i], "*", matlabMulInputB[i],
          swElemMulResults[i], matlabMulExpected[i]
      );
    }
  }

  printf("======================================================================\n");
  printf("  TEST SUMMARY REPORT\n");
  printf("======================================================================\n");
  printTestSummary("Software Implementation : Scalar Complex Mul", scalarMulFailure, totalScalarMulRun);
  printTestSummary("Software Implementation : Element Wise Vector Mul Result, Whole Array : ", arrayBasedScalarMulFailCnt, MAX_VECT);
  printTestSummary("Software Implementation : Element Wise Vector Mul Result, One Element at a Time : ", elementWiseScalarMulFailCnt, MAX_VECT);

  return 0;
}
