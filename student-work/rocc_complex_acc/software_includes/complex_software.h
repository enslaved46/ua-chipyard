#ifndef SOFTWARE_COMPLEX_H
#include "complex.h"

// we will have probelm with overflow, don't worry about overflow
// z = z1 + z2 = (a+c) + (b+d)i;

static inline complex_t swCmplxAdd(complex_t z1, complex_t z2){
  int16_t z1Real = getReal(z1);
  int16_t z2Real = getReal(z2);
  int16_t z1Imag = getImag(z1);
  int16_t z2Imag = getImag(z2);
  int16_t realResult = (z1Real  + z2Real) & 0xFFFF;
  int16_t imgResult   = (z1Imag + z2Imag) & 0xFFFF;
  complex_t addResult = makeComplex(realResult, imgResult);
  return addResult;
}

// perform verctor add
static inline void cSwVectAdd(complex_t z1[], complex_t z2[], complex_t result[], int size){ // (a+bi) (c+di)
  for (int i = 0; i < size; i++){
    result[i] = swCmplxAdd(z1[i], z2[i]);
    // printf(" realResult[%d] : %d  \t  imgResult[%d] : %d\n", i, realResult, i, imgResult);
  }
}


/* // z = z1* z2 =(ac + adi + bci + bdi^2) = (ac - bd) + (ad +bc)i */
static inline complex_t cSwMul(complex_t z1, complex_t z2){ // (a+bi) (c+di)
  int16_t z1Real = getReal(z1);
  int16_t z2Real = getReal(z2);
  int16_t z1Imag = getImag(z1);
  int16_t z2Imag = getImag(z2);

  int16_t realResult = (z1Real*z2Real) - (z1Imag*z2Imag);
  int16_t imgResult  = (z1Real*z2Imag) + (z1Imag*z2Real);
  complex_t cSwMulResult = makeComplex(realResult, imgResult);
  return cSwMulResult;
}

static inline void cVectMul(complex_t z1[], complex_t z2[], complex_t result[], int size){ // (a+bi) (c+di)
  for (int i = 0; i < size; i++){
    result[i] = cSwMul(z1[i], z2[i]);
  }
}

#endif

/* void printComplexForm(complex_t z){ */
/*   int16_t real = getReal(z); */
/*   int16_t img  = getImag(z); */

/*   if (real >= 0 && img >= 0){printf("\t Falls in 1   qrd  -> Complex number : %d + %dj\n", real, img);}   // +ve +ve */
/*   else if (real < 0 && img >= 0) {printf("\t Falls in 2nd qrd -> Complex number : %d + %dj\n", real, img);} //-ve +ve */
/*   else if (real < 0 && img < 0) {printf("\t Falls in 3rd qrd -> Complex number : %d%dj\n", real, img);} // -ve -ve */
/*   else {printf("\t Falls in 4th qrd  -> Complex number : %d%dj\n", real, img);} // +ve -ve */
/* } */

/* void printComplexVector(complex_t z[], int size){ */
/*     for (int i = 0; i < size; i++){ */
/*       printf("z[%d]", i ); */
/*       printComplexForm(z[i]); */
/*     } */
/* } */

