#include <stdio.h>
#include <stdint.h>

#include "../software_includes/complex.h"
#include "../software_includes/rocc_complex.h"
#include "../software_includes/complex_mul_test_vector.h"
#include "../software_includes/complex_mac_test_vector.h"
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



  int8_t scalarMacFailure = 0, totalScalarMacRun = 0;
    int32_t arrayBasedScalarMacFailCnt  = 0;
    int32_t elementWiseScalarMacFailCnt = 0;
    int8_t cornerFailures  = 0;
    
    // Static arrays in .bss to prevent bare-metal stack overflow at MAX_VECT = 5000
    static complex_t matlabMacInputA[MAX_VECT];
    static complex_t matlabMacInputB[MAX_VECT];
    static complex_mul_result_t matlabMacCumExpected[MAX_VECT];

    int i = 0;
    for (i = 0; i < MAX_VECT; i++) {
        matlabMacInputA[i]      = cmplxFrmPacked(MATLAB_MUL_INPUT_A[i]);
        matlabMacInputB[i]      = cmplxFrmPacked(MATLAB_MUL_INPUT_B[i]);
        matlabMacCumExpected[i] = MATLAB_MAC_CUMSUM_OUT[i];
    }





    
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

  // Test 1
  // MAC TEST, group of 4
  if (TEST_CMAC == 1) {
    if (DEBUG_PRINT) {
      printf("########################################################################\n");
      printf("Hardware Implementation : Running Complex MAC TEST                      \n");
      printf("########################################################################\n");
    }
    printf("Debug Print : %d/n", DEBUG_PRINT);

    BENCH_BEGIN();
    volatile  complex_mul_result_t seq0Res = roccCmplxMac(&cornerA[0],  &cornerB[0],  1);   
    BENCH_END("rocc_1_elem_complex_mac");

    // printComplexMulResult(seq0Res);  // shoul dprint -5+10j
    

    volatile  complex_mul_result_t seq1Res = roccCmplxMac(&cornerA[0],  &cornerB[0],  4);   


    complex_mul_result_t seq2Res = roccCmplxMac(&cornerA[4],  &cornerB[4],  4);
    complex_mul_result_t seq3Res = roccCmplxMac(&cornerA[8],  &cornerB[8],  4);
    complex_mul_result_t seq4Res = roccCmplxMac(&cornerA[12], &cornerB[12], 4);

    cornerFailures += checkMacResults("\nCorner Seq 1 (Small Pos)   :", seq1Res, cornerExpected[0]);
    cornerFailures += checkMacResults("Corner Seq 2 (Zero Cancel) :", seq2Res, cornerExpected[1]);
    cornerFailures += checkMacResults("Corner Seq 3 (>33b Pos)    :", seq3Res, cornerExpected[2]);
    cornerFailures += checkMacResults("Corner Seq 4 (>33b Neg)    :", seq4Res, cornerExpected[3]);

  }

  // Test2 Array based Acceleratoed Vector MACH TeST
  //  run whole test via wrapper

  if (ARRAY_BASED_SCALAR_ACC_VECTOR_MAC_TEST) {
    if (DEBUG_PRINT) {
      printf("########################################################################\n");
      printf("Array Based Accelerated Vector MAC Test : Scalar Hardware               \n");
      printf("Running Vector MAC Test for first %d Vectors                            \n", MAX_VECT);
      printf("########################################################################\n");
    }

    BENCH_BEGIN();
    complex_mul_result_t macArrayRes = roccCmplxMac(matlabMacInputA, matlabMacInputB, MAX_VECT);
    BENCH_END("array_based_scalar_acc_vector_mac");

    int8_t passFail = checkMacResults(
				      "Accelerator Hardware : Array-Based Vector MAC Result :      ",
				      macArrayRes,
				      matlabMacCumExpected[MAX_VECT - 1]
				      );
    arrayBasedScalarMacFailCnt += passFail;
  }

    printf("======================================================================\n");
    printf("  TEST SUMMARY REPORT\n");
    printf("======================================================================\n");
    printTestSummary("Complex MAC : 4 Corner Sequences", cornerFailures, 4);
    printTestSummary("Complex MAC : MATLAB Vector MAC (MAX_VECT)", arrayBasedScalarMacFailCnt, 1);  
}
