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
#include <stdint.h>

#include "../software_includes/complex.h"
#include "../software_includes/rocc_complex.h"
#include "../software_includes/complex_add_test_vectors.h"
#include "../software_includes/complex_test_hlpr.h"

#if defined(__riscv)
  #include <riscv-pk/encoding.h>
	  
  #include "../software_includes/benchmark_dependencies/metrics.h"
#endif


#define TEST_MAKE_COMPLEX 0
#define TEST_CADD 1
#define TEST_CMUL 0

#define PRINT_ADD_RESULT 0


#define ARRAY_BASED_SCALAR_ACC_VECTOR_ADD_TEST 1
#define ELEMET_WISE_SCALAR_ACC_VECTOR_ADD_TEST 1
        
int main(void) {
  printf("########################################################################\n");
  printf("Welcome to Complex Number Scalar Arithemetic Test Using Accelerator     \n");
  printf("########################################################################\n");
  const int16_t MAX_POS = 32767; //int16_t(65535/2 -1);
  const int16_t MAX_NEG = -32768; //  int16_t(65535/2-1);

  //-32767 is 0x8001
  // extreme test points
  const complex_t z0  = makeComplex(0, 0);
  const complex_t z_1 = makeComplex(-1, -1);
  const complex_t z1  = makeComplex(1,  1); // 4th
  const complex_t z2  = makeComplex(MAX_POS, MAX_POS);   // first quard
  const complex_t z3  = makeComplex(MAX_NEG, MAX_POS);   // 2nd quard
  const complex_t z4  = makeComplex(MAX_NEG, MAX_NEG); // third quard
  const complex_t z5  = makeComplex(MAX_POS, MAX_NEG); // 4th quard

  // these data set  doesn't test boundry, due to over flow
  complex_t z6  = makeComplex(  50,   50); // first q
  complex_t z7  = makeComplex(-500,  500); // sec
  complex_t z8  = makeComplex(-100, -500); // thrid
  complex_t z9  = makeComplex(100,  -500); // 4th

  if (TEST_CADD == 1){
    printf("########################################################################\n");
    printf("Running Scalar Test for Complex Addition                                \n");
    printf("Results are wrapped around for big Number                               \n");
    printf("########################################################################\n");
    BENCH_BEGIN();
    complex_t z10 = roccCmplxAdd(z6, z6);
    BENCH_END("scalar_complex_add");
    complex_t z11 = roccCmplxAdd(z7, z7);
    complex_t z12 = roccCmplxAdd(z8, z8);
    complex_t z13 = roccCmplxAdd(z2, z1);        // wraps around overflow max_val +1
    complex_t z00 = roccCmplxAdd(z4, z_1);  // wraps around under - neg_max-1

    // only  print when needed
    if(PRINT_ADD_RESULT){
      printComplexForm(z10);
      printComplexForm(z11);
      printComplexForm(z12);
      printComplexForm(z13);
      printComplexForm(z00);
    }

    complex_t tst1_res = makeComplex(100,      100);
    complex_t tst2_res = makeComplex(-1000,   1000);
    complex_t tst3_res = makeComplex(-200,   -1000);
    complex_t tst4_res = makeComplex(-32768, -32768);
    complex_t tst5_res = makeComplex(32767,  32767);

    // int16_t checkResults(char *testType, complex_t cmplxRes, int16_t expReal, int16_t expImg)
    //  checkResults(char *testType, complex_t cmplxRes, complex_t expRes){
    int8_t r1 = checkResults("Accelerator Scalar :           ", z6, "+", z6,  z10, tst1_res);
    int8_t r2 = checkResults("Accelerator Scalar :           ", z7, "+", z7,  z11, tst2_res);
    int8_t r3 = checkResults("Accelerator Scalar :           ", z8, "+", z8,  z12, tst3_res);
    int8_t r4 = checkResults("Accelerator Scalar : Overflow  ", z2, "+", z1,  z13, tst4_res);
    int8_t r5 = checkResults("Accelerator Scalar : Underflow ", z4, "+", z_1, z00, tst5_res);

    int8_t scalarAddFailure = r1+r2+r3+r4+r5;
    int8_t totalScalarAddRun = 5;

    if (scalarAddFailure == 0) {
      printf("PASS : all %d Scalar Complex Add Tests Passed\n", totalScalarAddRun);
      int8_t scalarAddSucess = 0;
    } else {
      printf("FAIL: %d of %d Scalar Complex Add Tests Failed\n", scalarAddFailure, totalScalarAddRun);
      int8_t scalarAddSucess = 1;
    }
  }

  // array based  element wise vector add test
  if(ARRAY_BASED_SCALAR_ACC_VECTOR_ADD_TEST){
    int i = 1;
    complex_t z1Arr[MAX_VECT] = {0};
    complex_t z2Arr[MAX_VECT] = {0};
    complex_t addResArr[MAX_VECT] = {0};
    for (i; i<=MAX_VECT; i++){
      printf("%d \t", i);
      z1Arr[i-1] = makeComplex(i,i*2);
      z2Arr[i-1] = makeComplex(i*2,i*3);
    }
    printf("\n");
    printf("########################################################################\n");
    printf("Array Based Accelerated  Vector Add Test  : Scalar Hardware             \n");
    printf("Running Vector Test for first %d Vector of Positive numbers             \n",MAX_VECT );
    printf("########################################################################\n");
    
    // CPU performs the array loads and stores
    // software-managed vector loop,
    BENCH_BEGIN();
    roccCmplxVectAdd(z1Arr, z2Arr, addResArr, MAX_VECT );
    BENCH_END("array_based_scalar_vector_add_test");

    // printComplexVector(z1Arr, MAX_VECT);
    // printComplexVector(z2Arr, MAX_VECT);
    printComplexVector(addResArr, MAX_VECT);
  }

  // element wise vector adder using scalar accelerator
  if(ELEMET_WISE_SCALAR_ACC_VECTOR_ADD_TEST)  {
    complex_t a[MAX_VECT] = {0};
    complex_t b[MAX_VECT] = {0};
    complex_t accAddResCalc[MAX_VECT] = {0};
    complex_t accAddResExp[MAX_VECT] = {0};
    int i=0;

    printf("########################################################################\n");
    printf("Element Wise Accelerateed Vector Add Test : Scalar Hardware             \n");
    printf("Running Vector Test for first %d Vector of Positive numbers             \n",MAX_VECT );
    printf("########################################################################\n");

    BENCH_BEGIN();
    
    for (i; i< MAX_VECT; i++){
      a[i] = cmplxFrmPacked(MATLAB_INPUT_A_POS[i]);
      b[i] = cmplxFrmPacked(MATLAB_INPUT_B_POS[i]);
      accAddResCalc[i] = roccCmplxAdd(a[i], b[i]); 
    }
    BENCH_END("elemet_wise_scalar_acc_vector_add");

    int tstFailCntr = 0;
    int pasFailRes;
    for (int i=0; i< MAX_VECT; i++){
      accAddResExp[i] = cmplxFrmPacked(MATLAB_EXPECTED_POS[i]);
      pasFailRes = checkResults("Element Wise Vector Add Using Scalar Accelerator Result :    ", a[i], "+", b[i],  accAddResCalc[i], accAddResExp[i] );
      tstFailCntr = tstFailCntr + pasFailRes;
    }

    int8_t totalScalarAddRun = MAX_VECT;

      
    if (tstFailCntr == 0) {
      printf("PASS : all %d Scalar Complex Add Tests Passed\n", totalScalarAddRun);
      int8_t scalarAddSucess = 0;
    } else {
      printf("FAIL: %d of %d Scalar Complex Add Tests Failed\n", tstFailCntr, totalScalarAddRun);
      int8_t scalarAddSucess = 1;
    }
    
  }
  
  return 0;
}
