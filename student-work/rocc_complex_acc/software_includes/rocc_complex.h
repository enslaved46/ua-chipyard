#ifndef ROCC_COMPLEX_H

#include "complex.h"
#include "/workspace/chipyard/tests/rocc.h"

#define COMPLEX_ADD_FUNCT   0
#define COMPLEX_MUL_FUNCT   1
#define COMPLEX_MUL_IMG_RTN_FUNCT 2

#define COMPLEX_MAC_FUNCT   3  // returns real with every command
#define COMPLEX_MAC_IMG_RTN_FUNCT 4
#define COMPLEX_MAC_CLR_REG_FUNCT 5


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
static inline complex_mul_result_t roccCmplxMul(complex_t z1, complex_t z2){ // (a+bi) (c+di)
  int64_t realResult;
  int64_t imgResult;
  complex_mul_result_t mulResult;

  // only rtns real
  ROCC_INSTRUCTION_DSS(
		       0,                              // custom0
		       realResult,                     // destination
		       (uint32_t)z1.packedComplex,     // rs1
		       (uint32_t)z2.packedComplex,     // rs2
		       COMPLEX_MUL_FUNCT               // funct = 1
		       );

  ROCC_INSTRUCTION_DSS(
		       0,                              // custom0
		       imgResult,                     // destination
		       (uint32_t)z1.packedComplex,     // rs1
		       (uint32_t)z2.packedComplex,     // rs2
		       COMPLEX_MUL_IMG_RTN_FUNCT             // funct = 1
		       );
  

  mulResult.real = realResult;
  mulResult.imag = imgResult;
  return mulResult;
}
 
static inline  void roccCmplxVectMul(complex_t z1[], complex_t z2[], complex_mul_result_t result[], int size){ // (a+bi) (c+di)
  for (int i = 0; i < size; i++){
    result[i] = roccCmplxMul(z1[i], z2[i]);
  }
}


static inline  complex_mul_result_t roccCmplxMac(complex_t z1[], complex_t z2[], int size){ // (a+bi) (c+di)
  // complex_mul_result_t macResult = {0, 0};
  complex_mul_result_t result = {0,0};
  // int64_t realResult;
  // int64_t imgResult;
  
  if (size <=0) return result; // need at leaset 1 objs meaning 2 complex num
 
  for (int i = 0; i < size; i++){
    // keep sending operands and continue MAC
    //   xd=1, xs1=1, xs2=
    ROCC_INSTRUCTION_DSS(
			 0,                              // custom0
			 result.real,                     // destination, alway return real reeult
			 (uint32_t)z1[i].packedComplex,     // rs1
			 (uint32_t)z2[i].packedComplex,     // rs2
			 COMPLEX_MAC_FUNCT               // funct 
			 );
  }
  // read final imag xd=1, xs1=0, xs2=0
  ROCC_INSTRUCTION_D(
		     0,                                  // custom0
		     result.imag,                           // rd  final accImagReg , real is alreay read ever time we send inst
		     COMPLEX_MAC_IMG_RTN_FUNCT             // funct for doRdMACImag
		     );

  /* macResult.real = realResult;; */
  /* macResult.imag = imgResult; */
  return result; // macResult;
}

// xd=0, xs1=0, xs2=0
static inline void roccCmplxMacClr(){
  ROCC_INSTRUCTION(0, COMPLEX_MAC_CLR_FUNCT);
}
#endif
