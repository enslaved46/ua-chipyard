#ifndef COMPLEX_H
#define COMPLEX_H
#include "/workspace/chipyard/tests/rocc.h"

#define COMPLEX_ADD_FUNCT 0
#define COMPLEX_MUL_FUNCT 1

typedef struct{
  int32_t packedComplex;
}complex_t;

static inline complex_t makeComplex(int16_t realN, int16_t imgN){
  complex_t complexN;
  complexN.packedComplex = (int32_t)(realN << 16) | (int32_t) (imgN  & 0xFFFF);
  // printf("Packed complex is : 0x%8x\n", packedComplex);
  return complexN;
}

static inline void unPackComplex(complex_t z){
  int16_t real = (int16_t)((int32_t) z.packedComplex >> 16);
  int16_t img  = (int16_t) (z.packedComplex & 0xFFFF); // losing sign, aksing compiler to treat as signed number
  printf("Unpacked Real in Decimal : %d \t in HEX : 0x%4x\n", real, real);
  printf("Unpacked Imag in Decimal : %d \t in HEX : 0x%4x\n", img,  img);

  //  printf("Unpacked Real : 0x%4x\n", cmplxN >> 16); // get upper bits
  // printf("Unpacked Imag : 0x%4x\n", (int16_t)(cmplxN & 0xFFFF));  // extract lower bits
  // return packedComplex;
}

static inline int16_t getReal(complex_t z){
  return (int16_t) (z.packedComplex >> 16);
}

static inline int16_t getImag(complex_t z){
  return (int16_t) (z.packedComplex & 0xFFFF); // losing sign, aksing compiler to treat as signed number
}

// we will have probelm with overflow, don't worry about overflow
// z = z1 + z2 = (a+c) + (b+d)i;
static inline complex_t cAdd(complex_t z1, complex_t z2){
  int64_t raw_result;
  complex_t addResult;

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

  addResult.packedComplex = (int32_t)(uint32_t)raw_result;
  return addResult;
}

/* // perform verctor add */
/* static inline  void cVectAdd(complex_t z1[], complex_t z2[], complex_t result[], int size){ // (a+bi) (c+di) */
/*   for (int i = 0; i < size; i++){ */
/*     result[i] = cAdd(z1[i], z2[i]); */
/*     // printf(" realResult[%d] : %d  \t  imgResult[%d] : %d\n", i, realResult, i, imgResult); */
/*   } */
/* } */


/* /\* // z = z1* z2 =(ac + adi + bci + bdi^2) = (ac - bd) + (ad +bc)i *\/ */
/* static inline complex_t cMul(complex_t z1, complex_t z2){ // (a+bi) (c+di) */

/*   int16_t z1Real = getReal(z1); */
/*   int16_t z2Real = getReal(z2); */
/*   int16_t z1Imag = getImag(z1); */
/*   int16_t z2Imag = getImag(z2); */

/*   int16_t realResult = (z1Real*z2Real) - (z1Imag*z2Imag); */
/*   int16_t imgResult  = (z1Real*z2Imag) + (z1Imag*z2Real); */
/*   complex_t cMulResult = makeComplex(realResult, imgResult); */
/*   return cMulResult; */
/* } */

/* static inline  void cVectMul(complex_t z1[], complex_t z2[], complex_t result[], int size){ // (a+bi) (c+di) */
/*   for (int i = 0; i < size; i++){ */
/*     result[i] = cMul(z1[i], z2[i]); */
/*   } */
/* } */

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
      printf("z[%d]", i );
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
      return 1;}
}

#endif
