#ifndef COMPLEX_TEST_HLPR
#define COMPLEX_TEST_HLPR

#include "complex.h"
#include <stdio.h>
#include <inttypes.h>

/* static inline void printComplexForm(complex_t z){ */
/*   int16_t real = getReal(z); */
/*   int16_t img  = getImag(z); */

/*   if (real >= 0 && img >= 0){printf("%d + %dj", real, img);}   // +ve +ve */
/*   else if (real < 0 && img >= 0) {printf("%d + %dj", real, img);} //-ve +ve */
/*   else if (real < 0 && img < 0) {printf("%d%dj", real, img);} // -ve -ve */
/*   else {printf("%d%dj", real, img);} // +ve -ve */
/* } */

// main print
static inline void printComplexValues(int64_t real, int64_t imag){
  if (imag < 0){    printf("%" PRId64 " %" PRId64 "j", real, imag);  }
  else { printf("%" PRId64 " + %" PRId64 "j", real, imag);  }
}

//
// print wrappers
//

// Packed 16-bit input
static inline void printComplexInput(complex_t z){
    printComplexValues((int64_t)getReal16(z),  (int64_t)getImag16(z));
}

// Exact 17-bit addition result
// stored in int32_t fields
static inline void printComplexAddResult(complex_add_result_t z){ 
  printComplexValues((int64_t)getReal17(z), (int64_t)getImag17(z));
}

// Exact 33-bit multiplication result stored in int64_t fields
static inline void printComplexMulResult(complex_mul_result_t z){
    printComplexValues(z.real, z.imag);
}

// end of print wrappers

static inline  void testComplexForm(complex_t z){
  int16_t real = getReal16(z);
  int16_t img  = getImag16(z);

  if (real >= 0 && img >= 0){printf("\t Falls in 1   qrd  -> Complex number : %d + %dj\n", real, img);}   // +ve +ve
  else if (real < 0 && img >= 0) {printf("\t Falls in 2nd qrd -> Complex number : %d + %dj\n", real, img);} //-ve +ve
  else if (real < 0 && img < 0) {printf("\t Falls in 3rd qrd -> Complex number : %d%dj\n", real, img);} // -ve -ve
  else {printf("\t Falls in 4th qrd  -> Complex number : %d%dj\n", real, img);} // +ve -ve
}


static inline  void printComplexAddVector(complex_add_result_t z[], int size){
  for (int i = 0; i < size; i++){
    printf("z[%d] = ", i );
    printComplexAddResult(z[i]);
    printf("\n");
  }
}

//checkResults("Accelerator Scalar :           ", z6, "+", z6,  z10, tst1_res);

//Accelerator Scalar : (50+50i) + (50+50i)                    [PASS]  100 + 100j
//int16_t checkResults(char *testType, complex_t cmplxRes, complex_t expRes){
int16_t checkAddResults(char *testType, complex_t op1, char* ope, complex_t op2, complex_add_result_t calcResult, complex_add_result_t expResult){
  // extract calulated
  int32_t calcReal = getReal17(calcResult);
  int32_t calcImg  = getImag17(calcResult);

  int32_t expReal = getReal17(expResult);
  int32_t expImg  = getImag17(expResult);

  
  /* if( calcResult.real== expResult.real && calcResult.imag == expResult.imag){ */
  if( calcReal== expReal && calcImg == expImg){
    if(DEBUG_PRINT){
      printf("%s [PASS] " , testType);
      printf("(");
      printComplexInput(op1);    //       printf("\t");

      printf(") %s (", ope);

      printComplexInput(op2);     //      printf("\t");
      printf(") = ");

      printComplexAddResult(calcResult);      printf("\n");
    }
    return 0;}
  else{
    if(DEBUG_PRINT){
      printf("%s [Fail] ", testType); printf("\t");
      printf("Calculated : ");
      printf("(");
      printComplexInput(op1);    //       printf("\t");

      printf(") %s (", ope);

      printComplexInput(op2);     //      printf("\t");
      printf(") = ");

      printComplexAddResult(calcResult);  printf("\n");
      printf("Expected  :");
      printComplexAddResult(expResult);
      printf("\n");
    }
    return 1;
  }
}



static inline int16_t checkMulResults(char *testType, complex_t op1, char* ope, complex_t op2, complex_mul_result_t calcResult, complex_mul_result_t expResult){
  // extract calulated
  int64_t calcReal = getReal64(calcResult);
  int64_t calcImg  = getImag64(calcResult);

  int64_t expReal = getReal64(expResult);
  int64_t expImg  = getImag64(expResult);

  
  /* if( calcResult.real== expResult.real && calcResult.imag == expResult.imag){ */
  if( calcReal== expReal && calcImg == expImg){
    if(DEBUG_PRINT){
      printf("%s [PASS] " , testType);
      printf("(");
      printComplexInput(op1);    //       printf("\t");

      printf(") %s (", ope);

      printComplexInput(op2);     //      printf("\t");
      printf(") = ");

      printComplexMulResult(calcResult);      printf("\n");
    }
    return 0;}
  else{
    if(DEBUG_PRINT){
      printf("%s [Fail] ", testType); printf("\t");
      printf("Calculated : ");
      printf("(");
      printComplexInput(op1);    //       printf("\t");

      printf(") %s (", ope);

      printComplexInput(op2);     //      printf("\t");
      printf(") = ");

      printComplexMulResult(calcResult);  printf("\n");
      printf("Expected  :");
      printComplexMulResult(expResult);
      printf("\n");
    }
    return 1;
  }
}

// we need to change the data type of accumulator,
// the bit width is same as mult_ype
static inline int16_t checkMacResults(const char *testType, complex_mul_result_t calc, complex_mul_result_t exp) {
    if (calc.real == exp.real && calc.imag == exp.imag) {
        if (DEBUG_PRINT) {
            printf("%s[PASS] Accumulated = ", testType);
            printComplexValues(calc.real, calc.imag);
            printf("\n");
        }
        return 0;
    } else {
        printf("%s[Fail] \tCalculated : ", testType);
        printComplexValues(calc.real, calc.imag);
        printf("\nExpected  :");
        printComplexValues(exp.real, exp.imag);
        printf("\n");
        return 1;
    }
}


static inline void printTestSummary(const char *testName, int failCount, int totalCount) {
    if (failCount == 0) {
        printf("PASS: all %d %s Tests Passed\n", totalCount, testName);
	//        return 0;
    } else {
        printf("FAIL: %d of %d %s Tests Failed\n", failCount, totalCount, testName);
        // return 1;
    }
}
#endif
