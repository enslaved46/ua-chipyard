#ifndef COMPLEX_H
#define COMPLEX_H


#define COMPLEX_ADD_FUNCT 0
#define COMPLEX_MUL_FUNCT 1
#define MAX_VECT   10
#define DEBUG_PRINT 1

typedef struct{
  int32_t packedComplex;
}complex_t;

// add results are 17 bits each, so we couldn't pack in 32 bit signal
typedef struct {
  int64_t addResultPkd;
} complex_add_result_t;

// multiplicaiton result is 33 bits. 32 from 16 bit mult and 1 from subtraction and addition
// Qm.n × Qj.q =Q(m+j).(n+q)
typedef struct {
    int64_t real;
    int64_t imag;
} complex_mul_result_t;

// inputs
static inline complex_t makeComplex16(int16_t realN, int16_t imgN){
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

/* static inline complex_add_result_t cmplxAddRsltFrmPacked(uint64_t pkdData){ */
/*   complex_add_result_t z; */
/*   z.real = (int32_t) (pkdData >>17) & 0x1FFFF; */
/*   z.imag = (int32_t)pkdData & 0x1FFFF; */
/*   return z; */
/* } */

// input
static inline void unPackComplex(complex_t z){
  int16_t real = (int16_t)((int32_t) z.packedComplex >> 16);
  int16_t img  = (int16_t) (z.packedComplex & 0xFFFF); // losing sign, aksing compiler to treat as signed number
  printf("Unpacked Real in Decimal : %d \t in HEX : 0x%4x\n", real, real);
  printf("Unpacked Imag in Decimal : %d \t in HEX : 0x%4x\n", img,  img);

  //  printf("Unpacked Real : 0x%4x\n", cmplxN >> 16); // get upper bits
  // printf("Unpacked Imag : 0x%4x\n", (int16_t)(cmplxN & 0xFFFF));  // extract lower bits
  // return packedComplex;
}

static inline int16_t getReal16(complex_t z){
  return (int16_t) (z.packedComplex >> 16);
}

static inline int16_t getImag16(complex_t z){
  return (int16_t) (z.packedComplex & 0xFFFF); // losing sign, aksing compiler to treat as signed number
}

static inline int32_t signExtend17(uint32_t num) {
  num &= 0x1FFFFu;
  if (num & 0x10000u) { // extrast MSB of addder result and check Sign
    num |= 0xFFFE0000u;
  }
  return (int32_t)num;
}

static inline int32_t getReal17(complex_add_result_t z){
  return (int32_t) signExtend17((uint64_t)(z.addResultPkd >> 17));
}

static inline int32_t getImag17(complex_add_result_t z){
  return (int32_t) signExtend17((uint64_t)(z.addResultPkd & 0x1FFFF)); // losing sign, aksing compiler to treat as signed number
}


// inline complex_t roccCmplxAdd(complex_t z1, complex_t z2);
// inline  void roccCmplxVectAdd(complex_t z1[], complex_t z2[], complex_t result[], int size); // (a+bi) (c+di)

#endif
