#ifndef ROCC_COMPLEX_H

#include "complex.h"
#include "/workspace/chipyard/tests/rocc.h"

// we will have probelm with overflow, don't worry about overflow
// z = z1 + z2 = (a+c) + (b+d)i;
static inline complex_add_result_t roccCmplxAdd(complex_t z1, complex_t z2){
  int64_t raw_result;

  /* D -> desitnation used : xd  = 1 */
  /* S -> source 1 used    : xs1 = 1 */
  /* S ->  source 2 used   : xs2 = 1 */


  ROCC_INSTRUCTION_DSS(
		       0,                              // custom0
		       raw_result,                     // destination
		       (uint32_t)z1.packedComplex,     // rs1
		       (uint32_t)z2.packedComplex,     // rs2
		       COMPLEX_ADD_FUNCT               // funct = 0
		       );

   return cmplxAddRsltFrmPacked(raw_result);
    // addResult.packedComplex = (int32_t)(uint32_t)raw_result;
   // return addResult;
}

// perform verctor add using scaar adder
static inline  void roccCmplxVectAdd(complex_t z1[], complex_t z2[], complex_add_result_t result[], int size){ // (a+bi) (c+di)
  for (int i = 0; i < size; i++){
    result[i] = roccCmplxAdd(z1[i], z2[i]);
    // printf(" realResult[%d] : %d  \t  imgResult[%d] : %d\n", i, realResult, i, imgResult);
  }
}


/* /\* // z = z1* z2 =(ac + adi + bci + bdi^2) = (ac - bd) + (ad +bc)i *\/ */
/* static inline complex_t cMul(complex_t z1, complex_t z2){ // (a+bi) (c+di) */

/*   int16_t z1Real = getReal(z1); */
/*   int16_t z2Real = getReal(z2); */
/*   int16_t z1Imag = getImag(z1); */
/*   int16_t z2Imag = getImag(z2); */

/*   int16_t realResult = (z1Real*z2Real) - (z1Imag*z2Imag); */
/*   int16_t imgResult  = (z1Real*z2Imag) + (z1Imag*z2Real); */
/*   complex_t cMulResult = makeComplex16(realResult, imgResult); */
/*   return cMulResult; */
/* } */

/* static inline  void cVectMul(complex_t z1[], complex_t z2[], complex_t result[], int size){ // (a+bi) (c+di) */
/*   for (int i = 0; i < size; i++){ */
/*     result[i] = cMul(z1[i], z2[i]); */
/*   } */
/* } */

#endif
