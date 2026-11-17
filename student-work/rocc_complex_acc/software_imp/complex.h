#ifndef COMPLEX_H
#define COMPLEX_H

typedef struct{
  int32_t packedComplex;
}complex_t;
  
static inline complex_t makeComplex(int16_t realN, int16_t imgN){
  complex_t complexN;
  complexN.packedComplex = (int32_t)(realN << 16) | (int32_t) (imgN  & 0xFFFF);
  // printf("Packed complex is : 0x%8x\n", packedComplex);
  return complexN;
}

void unPackComplex(complex_t z){
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
complex_t cAdd(complex_t z1, complex_t z2){
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
void cVectAdd(complex_t z1[], complex_t z2[], complex_t result[], int size){ // (a+bi) (c+di)

  for (int i = 0; i < size; i++){
    result[i] = cAdd(z1[i], z2[i]);
    // printf(" realResult[%d] : %d  \t  imgResult[%d] : %d\n", i, realResult, i, imgResult);
  }
}


/* // z = z1* z2 =(ac + adi + bci + bdi^2) = (ac - bd) + (ad +bc)i */
complex_t cMul(complex_t z1, complex_t z2){ // (a+bi) (c+di)

  int16_t z1Real = getReal(z1);
  int16_t z2Real = getReal(z2);
  int16_t z1Imag = getImag(z1);
  int16_t z2Imag = getImag(z2);

  int16_t realResult = (z1Real*z2Real) - (z1Imag*z2Imag);
  int16_t imgResult  = (z1Real*z2Imag) + (z1Imag*z2Real);
  complex_t cMulResult = makeComplex(realResult, imgResult);
  return cMulResult;
}

void cVectMul(complex_t z1[], complex_t z2[], complex_t result[], int size){ // (a+bi) (c+di)
  for (int i = 0; i < size; i++){
    result[i] = cMul(z1[i], z2[i]);
  }
}

void printComplexForm(complex_t z){
  int16_t real = getReal(z);
  int16_t img  = getImag(z);

  if (real >= 0 && img >= 0){printf("\t Falls in 1   qrd  -> Complex number : %d + %dj\n", real, img);}   // +ve +ve
  else if (real < 0 && img >= 0) {printf("\t Falls in 2nd qrd -> Complex number : %d + %dj\n", real, img);} //-ve +ve
  else if (real < 0 && img < 0) {printf("\t Falls in 3rd qrd -> Complex number : %d%dj\n", real, img);} // -ve -ve
  else {printf("\t Falls in 4th qrd  -> Complex number : %d%dj\n", real, img);} // +ve -ve
}

void printComplexVector(complex_t z[], int size){
    for (int i = 0; i < size; i++){
      printf("z[%d]", i );
      printComplexForm(z[i]);
    }
}

#endif
