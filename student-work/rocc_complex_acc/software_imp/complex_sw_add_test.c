/* Implementation notes */

/* 32 bits Signed INT complex number software imp */
/* 16 bits real and 16 bits img */
/* MSB is real and LSB is IMG */
/* 31     16 15     0 */
/*    Real      IMG */
/* ----------------------------------------------------------- */
// Review : How nuber gets mapped into Hardawre
/* max positive                      */
/* range is -2^N/2 to 2^N/2 -1 */
/* −32,768 to +32,767 */
/* Number mapping is */
/*  0x8000  0x0000 0x7FFF */

/* 32768 is 0x8000 */ //TWO complement
/*    ~(8000) */
/*    =0111 1111 1111 1111 */
/*                      +1 */
/*    -------------------- */
/*    1000  0000 0000 0000 */
/* ---------------------------- */
/* -32768 = 0x8000  */
/* ----------------------------------------------------------- */


#include <stdio.h>
// #include <riscv-pk/encoding.h>
#include <stdint.h>
#include "../software_includes/complex.h"
#include "../software_includes/complex_software.h"
#include "../software_includes/complex_add_test_vectors.h"
#include "../software_includes/complex_test_hlpr.h"

#if defined(__riscv)
#include <riscv-pk/encoding.h>
	  
#include "../software_includes/benchmark_dependencies/metrics.h"
#endif



#define TEST_MAKE_COMPLEX 1

#define TEST_CADD 1
#define TEST_CMUL 0

#define PRINT_ADD_RESULT 0
#define ARRAY_BASED_SCALAR_ACC_VECTOR_ADD_TEST 1
#define ELEMET_WISE_SCALAR_ACC_VECTOR_ADD_TEST 1



int main(void) {
  printf("########################################################################\n");
  printf("Welcome to Complex Number Scalar Arithemetic Test Using Pure Software   \n");
  printf("########################################################################\n");
  const int16_t MAX_POS = 32767; //int16_t(65535/2 -1);
  const int16_t MAX_NEG = -32768; //  int16_t(65535/2-1);
  int8_t scalarAddFailure, totalScalarAddRun;
  // one array elemtn at a itmte
  int elementWiseScalarTstRsltCntr = 0;
  int elementWisePassFailRes;
  // passing whole array
  int32_t swElemVecAddFailCnt = 0;
  int8_t  swElemVecAddPassFail = 0;
    


  // load form matlab , golden model
  static complex_t matlabImputA[MAX_VECT] = {0};
  static complex_t matlabImputB[MAX_VECT] = {0};
  static complex_add_result_t matlabResultExpected[MAX_VECT] = {0};
  int i =0;
    
  for (; i< MAX_VECT; i++){
    matlabImputA[i] = cmplxFrmPacked(MATLAB_INPUT_A[i]);
    matlabImputB[i] = cmplxFrmPacked(MATLAB_INPUT_B[i]);
    matlabResultExpected[i] = cmplxAddRsltFrmPacked(MATLAB_EXPECTED_OUT[i]);
  }

    
  //-32767 is 0x8001
  // extreme test points
  // const complex_t z0  = makeComplex16(0, 0);
  const complex_t z_1 = makeComplex16(-1, -1);
  const complex_t z1  = makeComplex16(1,  1); // 4th
  const complex_t z2  = makeComplex16(MAX_POS, MAX_POS);   // first quard
  //  const complex_t z3  = makeComplex16(MAX_NEG, MAX_POS);   // 2nd quard
  const complex_t z4  = makeComplex16(MAX_NEG, MAX_NEG); // third quard
  // const complex_t z5  = makeComplex16(MAX_POS, MAX_NEG); // 4th quard

  // these data set  doesn't test boundry, due to over flow
  complex_t z6  = makeComplex16(  50,   50); // first q
  complex_t z7  = makeComplex16(-500,  500); // sec
  complex_t z8  = makeComplex16(-100, -500); // thrid
  // complex_t z9  = makeComplex16(100,  -500); // 4th

  if (TEST_CADD == 1){
    if(DEBUG_PRINT){
      printf("########################################################################\n");
      printf("Software Implementation : Running Scalar Test for Complex Addition      \n");
      printf("########################################################################\n");
    }
    BENCH_BEGIN();
    volatile complex_add_result_t z10 = swCmplxAdd(z6, z6);
    BENCH_END("scalar_complex_add_software_based");
    complex_add_result_t z11 = swCmplxAdd(z7, z7);
    complex_add_result_t z12 = swCmplxAdd(z8, z8);
    complex_add_result_t z13 = swCmplxAdd(z2, z1);    // wraps around overflow max_val +1
    complex_add_result_t z00 = swCmplxAdd(z4, z_1);  // wraps around under - neg_max-1

    // only  print when needed
    if(PRINT_ADD_RESULT){
      printComplexAddResult(z10);
      printComplexAddResult(z11);
      printComplexAddResult(z12);
      printComplexAddResult(z13);
      printComplexAddResult(z00);
    }

    complex_add_result_t tst1_res = makeComplex17(100, 100);
    complex_add_result_t tst2_res = makeComplex17(-1000, 1000);
    complex_add_result_t tst3_res = makeComplex17(-200, -1000);
    complex_add_result_t tst4_res = makeComplex17(32768, 32768);
    complex_add_result_t tst5_res = makeComplex17(-32769, -32769);

    /* complex_add_result_t tst1_res = {.real = 100,   .imag = 100}; */
    /* complex_add_result_t tst2_res = {.real = -1000, .imag = 1000}; */
    /* complex_add_result_t tst3_res = {.real = -200, .imag = -1000}; */
    /* complex_add_result_t tst4_res = {.real = 32768, .imag = 32768}; */
    /* complex_add_result_t tst5_res = {.real = -32769, .imag = -32769}; */
    // int16_t checkResults(char *testType, complex_t cmplxRes, int16_t expReal, int16_t expImg)
    //  checkResults(char *testType, complex_t cmplxRes, complex_t expRes){
    int8_t r1 = checkAddResults("Software Implementation Scalar :           ", z6, "+", z6,  z10, tst1_res);
    int8_t r2 = checkAddResults("Software Implementation Scalar :           ", z7, "+", z7,  z11, tst2_res);
    int8_t r3 = checkAddResults("Software Implementation Scalar :           ", z8, "+", z8,  z12, tst3_res);
    int8_t r4 = checkAddResults("Software Implementation Scalar : Overflow  ", z2, "+", z1,  z13, tst4_res);
    int8_t r5 = checkAddResults("Software Implementation Scalar : Underflow ", z4, "+", z_1, z00, tst5_res);

    scalarAddFailure = r1+r2+r3+r4+r5;
    totalScalarAddRun = 5;

  }

  // array based  element wise vector add test
  if(ARRAY_BASED_SCALAR_ACC_VECTOR_ADD_TEST){
    complex_add_result_t addResArr[MAX_VECT] = {0};

    printf("\n");
    if(DEBUG_PRINT){
      printf("########################################################################\n");
      printf("Array Based Vector Add Test : Software Implementation                   \n");
      printf("Running Vector Test for first %d Vector of Positive numbers             \n",MAX_VECT );
      printf("########################################################################\n");
    }
    // CPU performs the array loads and stores
    // software-managed vector loop,
    BENCH_BEGIN();
    cSwVectAdd(matlabImputA, matlabImputB,addResArr, MAX_VECT );
    BENCH_END("array_based_scalar_vector_add_software_based");

    for (i=0; i< MAX_VECT; i++){
      swElemVecAddPassFail  = checkAddResults("Software Implementation : Element Wise Vector Add Result :    ",
					      matlabImputA[i],
					      "+",
					      matlabImputA[i],
					      addResArr[i],
					      matlabResultExpected[i] );
      swElemVecAddFailCnt += swElemVecAddPassFail;
    }
  }

  // element wise vector adder using scalar accelerator
  if(ELEMET_WISE_SCALAR_ACC_VECTOR_ADD_TEST)  {

    if(DEBUG_PRINT){
      printf("########################################################################\n");
      printf("Software Implementation : Element Wise Accelerateed Vector Add Test     \n");
      printf("Running Vector Test for first %d Vector of Positive numbers             \n",MAX_VECT );
      printf("########################################################################\n");
    }

    BENCH_BEGIN();
    complex_add_result_t swElemAddResults[MAX_VECT];
    for (i=0 ; i< MAX_VECT; i++){     
      swElemAddResults[i] = swCmplxAdd(matlabImputA[i], matlabImputB[i]);
    }
    BENCH_END("elemet_wise_scalar_acc_vector_add_software_based");

    for ( i=0; i< MAX_VECT; i++){
      // matlabResultExpected[i] = cmplxAddRsltFrmPacked(MATLAB_EXPECTED_OUT[i]);
      elementWisePassFailRes = checkAddResults("Software Implementation : Element Wise Vector Add Result :    ",
					       matlabImputA[i],
					       "+",
					       matlabImputB[i],
					       swElemAddResults[i],
					       matlabResultExpected[i] );

      elementWiseScalarTstRsltCntr = elementWiseScalarTstRsltCntr + elementWisePassFailRes;
    }

      
    /* if (elementWiseScalarTstRsltCntr == 0) { */
    /*   printf("PASS : all %d Software Implementation : Scalar Complex Add Tests Passed\n", totalScalarAddRun); */
    /*   // int8_t scalarAddSucess = 0; */
    /* } else { */
    /*   printf("FAIL: %d of %d Software Implementation : Scalar Complex Add Tests Failed\n", elementWiseScalarTstRsltCntr, totalScalarAddRun); */
    /*   //  int8_t scalarAddSucess = 1; */
    /* } */
    
  }

  printf("======================================================================\n");
  printf("  TEST SUMMARY REPORT\n");
  printf("======================================================================\n");

  printTestSummary("Software Implementation : Scalar Complex Add", scalarAddFailure, totalScalarAddRun);
  printTestSummary("Software Implementation : Element Wise Vector Add Result, Whole Array : ", swElemVecAddFailCnt, MAX_VECT);
  printTestSummary("Software Implementation : Element Wise Vector Add Result, One Array at a Time : ", elementWiseScalarTstRsltCntr, MAX_VECT);
  
  return 0;
}
