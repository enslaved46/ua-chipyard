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



#define TEST_CMUL 1
#define PRINT_MUL_RESULT 0

#define ARRAY_BASED_SCALAR_ACC_VECTOR_MUL_TEST 1
#define ELEMENT_WISE_SCALAR_ACC_VECTOR_MUL_TEST 1

int main(){
  // --- Complex Multiplication Variables & Golden Vector Loading ---
  int8_t scalarMulFailure = 0, totalScalarMulRun = 0;
  int32_t elementWiseScalarMulFailCnt = 0;
  int32_t arrayBasedScalarMulFailCnt  = 0;

  complex_t matlabMulInputA[MAX_VECT] = {0};
  complex_t matlabMulInputB[MAX_VECT] = {0};
  complex_mul_result_t matlabMulExpected[MAX_VECT] = {0};

  const int16_t MAX_POS = 32767;
  const int16_t MAX_NEG = -32768;

  // Extreme corner test points
  const complex_t z2 = makeComplex16(MAX_POS, MAX_POS); // (32767 + 32767j) -> 32-bit boundary
  const complex_t z4 = makeComplex16(MAX_NEG, MAX_NEG); // (-32768 - 32768j) -> 33-bit exact max

  // Standard quadrant test points
  complex_t z6 = makeComplex16(  50,   50); // 1st quadrant
  complex_t z7 = makeComplex16(-500,  500); // 2nd quadrant
  complex_t z8 = makeComplex16(-100, -500); // 3rd quadrant


  int i = 0;
  for (i = 0; i < MAX_VECT; i++) {
    matlabMulInputA[i]   = cmplxFrmPacked(MATLAB_MUL_INPUT_A[i]);
    matlabMulInputB[i]   = cmplxFrmPacked(MATLAB_MUL_INPUT_B[i]);
    matlabMulExpected[i] = MATLAB_MUL_EXPECTED_OUT[i]; // Already {int64_t real, int64_t imag}
  }

  // 1. Scalar Complex Multiplication Test (Quadrants + 33-bit Corner Cases)
  if (TEST_CMUL == 1) {
    if (DEBUG_PRINT) {
      printf("########################################################################\n");
      printf("Hardware Implementation : Running Scalar Test for Complex Multiplication\n");
      printf("########################################################################\n");
    }

    BENCH_BEGIN();
    complex_mul_result_t m10 = roccCmplxMul(z6, z6); // (50 + 50j)*(50 + 50j) = 0 + 5000j
    BENCH_END("scalar_complex_mul");

    complex_mul_result_t m11 = roccCmplxMul(z7, z7); // (-500 + 500j)*(-500 + 500j) = 0 - 500000j
    complex_mul_result_t m12 = roccCmplxMul(z8, z8); // (-100 - 500j)*(-100 - 500j) = -240000 + 100000j
    complex_mul_result_t m13 = roccCmplxMul(z2, z2); // (32767 + 32767j)^2 = 0 + 2147352578j (32-bit boundary)
    complex_mul_result_t m00 = roccCmplxMul(z4, z4); // (-32768 - 32768j)^2 = 0 + 2147483648j (33-bit exact max!)

    if (PRINT_MUL_RESULT) {
      printComplexValues(m10.real, m10.imag); printf("\n");
      printComplexValues(m11.real, m11.imag); printf("\n");
      printComplexValues(m12.real, m12.imag); printf("\n");
      printComplexValues(m13.real, m13.imag); printf("\n");
      printComplexValues(m00.real, m00.imag); printf("\n");
    }

    complex_mul_result_t mulTst1Res = makeComplex33(0, 5000);
    complex_mul_result_t mulTst2Res = makeComplex33(0, -500000);
    complex_mul_result_t mulTst3Res = makeComplex33(-240000, 100000);
    complex_mul_result_t mulTst4Res = makeComplex33(0, 2147352578LL);
    complex_mul_result_t mulTst5Res = makeComplex33(0, 2147483648LL);

    int8_t mr1 = checkMulResults("Accelerator Scalar Mul :           ", z6, "*", z6, m10, mulTst1Res);
    int8_t mr2 = checkMulResults("Accelerator Scalar Mul :           ", z7, "*", z7, m11, mulTst2Res);
    int8_t mr3 = checkMulResults("Accelerator Scalar Mul :           ", z8, "*", z8, m12, mulTst3Res);
    int8_t mr4 = checkMulResults("Accelerator Scalar Mul : 32b Max   ", z2, "*", z2, m13, mulTst4Res);
    int8_t mr5 = checkMulResults("Accelerator Scalar Mul : 33b Exact ", z4, "*", z4, m00, mulTst5Res);

    scalarMulFailure  = mr1 + mr2 + mr3 + mr4 + mr5;
    totalScalarMulRun = 5;
  }

  // 2. Array-Based Element-Wise Vector Multiply Test
  if (ARRAY_BASED_SCALAR_ACC_VECTOR_MUL_TEST) {
    complex_mul_result_t mulResArr[MAX_VECT] = {0};

    printf("\n");
    if (DEBUG_PRINT) {
      printf("########################################################################\n");
      printf("Array Based Accelerated Vector Mul Test : Scalar Hardware               \n");
      printf("Running Vector Test for first %d Vectors                                \n", MAX_VECT);
      printf("########################################################################\n");
    }

    BENCH_BEGIN();
    roccCmplxVectMul(matlabMulInputA, matlabMulInputB, mulResArr, MAX_VECT);
    BENCH_END("array_based_scalar_acc_vector_mul");

    for (i = 0; i < MAX_VECT; i++) {
      int8_t passFail = checkMulResults(
					"Accelerator Hardware : Array-Based Vector Mul Result :      ",
					matlabMulInputA[i],
					"*",
					matlabMulInputB[i],
					mulResArr[i],
					matlabMulExpected[i]
					);
      arrayBasedScalarMulFailCnt += passFail;
    }
  }

  // 3. Element-Wise Vector Multiply Using Scalar Accelerator
  if (ELEMENT_WISE_SCALAR_ACC_VECTOR_MUL_TEST) {
    if (DEBUG_PRINT) {
      printf("########################################################################\n");
      printf("Element Wise Accelerated Vector Mul Test : Scalar Hardware              \n");
      printf("Running Vector Test for first %d Vectors                                \n", MAX_VECT);
      printf("########################################################################\n");
    }

    BENCH_BEGIN();
    complex_mul_result_t hwElemMulResults[MAX_VECT];
    for (i = 0; i < MAX_VECT; i++) {
      hwElemMulResults[i] = roccCmplxMul(matlabMulInputA[i], matlabMulInputB[i]);
    }
    BENCH_END("element_wise_scalar_acc_vector_mul");

    for (i = 0; i < MAX_VECT; i++) {
      int8_t passFail = checkMulResults(
					"Accelerator Hardware : Element-Wise Vector Mul Result :     ",
					matlabMulInputA[i],
					"*",
					matlabMulInputB[i],
					hwElemMulResults[i],
					matlabMulExpected[i]
					);
      elementWiseScalarMulFailCnt += passFail;
    }
  }
  printTestSummary("Accelerator Hardware : Scalar Complex Mul", scalarMulFailure, totalScalarMulRun);
  printTestSummary("Accelerator Hardware : Element Wise Vector Mul Result, Whole Array : ", arrayBasedScalarMulFailCnt, MAX_VECT);
  printTestSummary("Accelerator Hardware : Element Wise Vector Mul Result, One Element at a Time : ", elementWiseScalarMulFailCnt, MAX_VECT);
}
