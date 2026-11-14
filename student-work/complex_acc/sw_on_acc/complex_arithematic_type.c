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

#if defined(__riscv)
  #include <riscv-pk/encoding.h>
  #include "../../benchmark_dependencies/metrics.h"
#endif

#include <stdint.h>
#include "complex.h"

#define TEST_MAKE_COMPLEX 0
#define TEST_CADD 1
#define TEST_CMUL 0

#define PRINT_ADD_RESULT 0


int main(void) {
  printf("Welcome to Complex Numer Arithemetic /562\n");

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


  /* if(TEST_MAKE_COMPLEX == 1){ */
  /*   printf("########################################################################\n"); */
  /*   printf("Running Test for packing and packing complex number into 32'b single REG\n"); */
  /*   printf("########################################################################\n"); */
  /*   unPackComplex(z0); */
  /*   unPackComplex(z1); */
  /*   unPackComplex(z2); */
  /*   unPackComplex(z3); */
  /*   unPackComplex(z4); */
  /*   unPackComplex(z5); */
  /* } */

  if (TEST_CADD == 1){
    printf("########################################################################\n");
    printf("Running Test for Complex Addition                                       \n");
    printf("Results are wrapped around for big Number                               \n");
    printf("########################################################################\n");
    complex_t z10 = cAdd(z6, z6);
    complex_t z11 = cAdd(z7, z7);
    complex_t z12 = cAdd(z8, z8);
    complex_t z13 = cAdd(z2, z1);        // wraps around overflow max_val +1
    complex_t z00 = cAdd(z4, z_1);  // wraps around under - neg_max-1
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

    int8_t addFailure = r1+r2+r3+r4+r5;
    int8_t addTestsRun = 5;

    if (addFailure == 0) {
      printf("PASS : all %d Scalar Complex Add Tests Passed\n", addTestsRun);
      int8_t addSucess = 0;
    } else {
      printf("FAIL: %d of %d Scalar Complex Add Tests Failed\n", addFailure, addTestsRun);
      int8_t addSucess = 1;
    }
  }

  /* if (TEST_CMUL == 1){ */
  /*   printf("########################################################################\n"); */
  /*   printf("Running Test for Complex Multiplication                                 \n"); */
  /*   printf("Results are wrapped around for big Number                               \n"); */
  /*   printf("########################################################################\n"); */
  /*   complex_t z14 = cMul(z6, z6); */
  /*   complex_t z15 = cMul(z7, z7); */
  /*   complex_t z16 = cMul(z8, z8); */
  /*   complex_t z17 = cMul(z9, z9); */

  /*   printComplexForm(z14); */
  /*   printComplexForm(z15); */
  /*   printComplexForm(z16); */
  /*   printComplexForm(z17); */
  /* } */
  return 0;
}
