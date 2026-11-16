#ifndef COMPLEX_TEST_HLPR
#define COMPLEX_TEST_HLPR

#include "complex.h"
#include <stdio.h>

static inline void printComplexForm(complex_t z){
  int16_t real = getReal(z);
  int16_t img  = getImag(z);

  if (real >= 0 && img >= 0){printf("%d + %dj", real, img);}   // +ve +ve
  else if (real < 0 && img >= 0) {printf("%d + %dj", real, img);} //-ve +ve
  else if (real < 0 && img < 0) {printf("%d%dj", real, img);} // -ve -ve
  else {printf("%d%dj", real, img);} // +ve -ve
}

static inline  void testComplexForm(complex_t z){
  int16_t real = getReal(z);
  int16_t img  = getImag(z);

  if (real >= 0 && img >= 0){printf("\t Falls in 1   qrd  -> Complex number : %d + %dj\n", real, img);}   // +ve +ve
  else if (real < 0 && img >= 0) {printf("\t Falls in 2nd qrd -> Complex number : %d + %dj\n", real, img);} //-ve +ve
  else if (real < 0 && img < 0) {printf("\t Falls in 3rd qrd -> Complex number : %d%dj\n", real, img);} // -ve -ve
  else {printf("\t Falls in 4th qrd  -> Complex number : %d%dj\n", real, img);} // +ve -ve
}


static inline  void printComplexVector(complex_t z[], int size){
  for (int i = 0; i < size; i++){
    printf("z[%d] = ", i );
    printComplexForm(z[i]);
    printf("\n");
  }
}

//checkResults("Accelerator Scalar :           ", z6, "+", z6,  z10, tst1_res);

//Accelerator Scalar : (50+50i) + (50+50i)                    [PASS]  100 + 100j
//int16_t checkResults(char *testType, complex_t cmplxRes, complex_t expRes){
int16_t checkResults(char *testType, complex_t op1, char* ope, complex_t op2, complex_t cmplxRes, complex_t expRes){
  // extract calulated
  int16_t calcReal = getReal(cmplxRes);
  int16_t calcImg  = getImag(cmplxRes);

  int16_t expReal = getReal(expRes);
  int16_t expImg  = getImag(expRes);

  if(calcReal == expReal && calcImg == expImg)
    { printf("%s [PASS] " , testType);
      printf("(");
      printComplexForm(op1);    //       printf("\t");

      printf(") %s (", ope);

      printComplexForm(op2);     //      printf("\t");
      printf(") = ");

      printComplexForm(cmplxRes);      printf("\n");
      return 0;}
  else
    { printf("%s [Fail] ", testType); printf("\t");
      printf("Calculated : ");
      printComplexForm(cmplxRes);  printf("\t");
      printf("Expected  :");
      printComplexForm(expRes);
      printf("\n");
      return 1;
    }
}

#endif
