#ifndef SOFTWARE_COMPLEX_H
#define SOFTWARE_COMPLEX_H
#include "complex.h"

// we will have probelm with overflow, don't worry about overflow
// z = z1 + z2 = (a+c) + (b+d)i;

static inline complex_add_result_t swCmplxAdd(complex_t z1, complex_t z2){
  int16_t z1Real = getReal16(z1);
  int16_t z2Real = getReal16(z2);
  int16_t z1Imag = getImag16(z1);
  int16_t z2Imag = getImag16(z2);

  // lesson learned, if we mask then we need to presere sign when we make it to 32bit
  // ((((int_32t)(z1Real  + z2Real)) & 0x1FFFF) << 15)>>15;
  // addResult.real = (int32_t)(z1Real) + (int32_t)(z2Real);// & 0x1FFFF;
  // addResult.imag = (int32_t)(z1Imag) + (int32_t)(z2Imag); // & 0x1FFFF;
  int32_t realResult = (int32_t)(z1Real) + (int32_t)(z2Real);
  int32_t imgResult  = (int32_t)(z1Imag) + (int32_t)(z2Imag);
  complex_add_result_t addResult = makeComplex17(realResult, imgResult);
  return addResult;
}

// perform verctor add
static inline void cSwVectAdd(complex_t z1[], complex_t z2[], complex_add_result_t result[], int size){ // (a+bi) (c+di)
  for (int i = 0; i < size; i++){
    result[i] = swCmplxAdd(z1[i], z2[i]);
    // printf(" realResult[%d] : %d  \t  imgResult[%d] : %d\n", i, realResult, i, imgResult);
  }
}


/* // z = z1* z2 =(ac + adi + bci + bdi^2) = (ac - bd) + (ad +bc)i */
static inline complex_mul_result_t cSwMul(complex_t z1, complex_t z2){ // (a+bi) (c+di)
  complex_mul_result_t cSwMulResult;
  int64_t z1Real = getReal16(z1);  // overflows for 16 bit max number , that resutls -32768
  int64_t z2Real = getReal16(z2);
  int64_t z1Imag = getImag16(z1);
  int64_t z2Imag = getImag16(z2);
  cSwMulResult.real = signExtend33((uint64_t)((z1Real * z2Real) - (z1Imag * z2Imag)));  //& 0x1FFFFFFFF;
  cSwMulResult.imag = signExtend33((uint64_t)((z1Real * z2Imag) + (z1Imag * z2Real)));
  
  // int16_t realResult = (z1Real*z2Real) - (z1Imag*z2Imag);
  // int16_t imgResult  = (z1Real*z2Imag) + (z1Imag*z2Real);
  // complex_t cSwMulResult = makeComplex(realResult, imgResult);
  return cSwMulResult;
}

static inline void cVectMul(complex_t z1[], complex_t z2[], complex_mul_result_t result[], int size){ // (a+bi) (c+di)
  for (int i = 0; i < size; i++){
    result[i] = cSwMul(z1[i], z2[i]);
  }
}

static inline complex_mul_result_t cSwMac(const complex_t z1[], const complex_t z2[], int len) {
    complex_mul_result_t acc = {0, 0};
    for (int i = 0; i < len; i++) {
        complex_mul_result_t prod = cSwMul(z1[i], z2[i]);
        acc.real += prod.real;
        acc.imag += prod.imag;
    }
    return acc;
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
