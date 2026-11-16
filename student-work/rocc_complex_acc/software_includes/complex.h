#ifndef COMPLEX_H
#define COMPLEX_H


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

// used using TB, type casting becomes easy
static inline complex_t cmplxFrmPacked(uint32_t pkdData){
  complex_t z;
  z.packedComplex = (int32_t)pkdData;
  return z;
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

// inline complex_t roccCmplxAdd(complex_t z1, complex_t z2);
// inline  void roccCmplxVectAdd(complex_t z1[], complex_t z2[], complex_t result[], int size); // (a+bi) (c+di)

#endif
