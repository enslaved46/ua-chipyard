#ifndef SOFTWARE_COMPLEX_H
#define SOFTWARE_COMPLEX_H
#include "complex.h"

// we will have probelm with overflow, don't worry about overflow
// z = z1 + z2 = (a+c) + (b+d)i;


static inline complex_add_result_t makeComplex17(int32_t realN, int32_t imgN){
  complex_add_result_t z;
  uint64_t realBits = (uint64_t)(realN & 0x1FFFF);
  uint64_t imagBits = (uint64_t)(imgN  & 0x1FFFF);
  z.addResultPkd = (realBits << 17)  | imagBits;
  
  // printf("Packed complex is : 0x%8x\n", packedComplex);
  return z;
}


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
  int16_t z1Real = getReal16(z1);
  int16_t z2Real = getReal16(z2);
  int16_t z1Imag = getImag16(z1);
  int16_t z2Imag = getImag16(z2);
  cSwMulResult.real = ((z1Real*z2Real) - (z1Imag*z2Imag)) & 0x1FFFFFFFF;
  cSwMulResult.imag = ((z1Real*z2Imag) + (z1Imag*z2Real))  & 0x1FFFFFFFF;
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
