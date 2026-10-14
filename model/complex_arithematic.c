/* 32 bits Signed INT complex number software imp */
/* 16 bits real and 16 bits img */
/* MSB is real and LSB is IMG */
/* 31     16 15     0 */
/*    Real      IMG */

#include <stdio.h>
// #include <riscv-pk/encoding.h>
#include <stdint.h>

#define DEBUG_PRINT 1

int32_t packComplex(int16_t realN, int16_t imgN){
  int32_t packedComplex = (int32_t)(realN << 16) | (int32_t) (imgN  & 0xFFFF);
  // printf("Packed complex is : 0x%8x\n", packedComplex);
  return packedComplex;
}

void unPackComplex(int32_t cmplxN){
  int16_t real = (int16_t)((int32_t)cmplxN >> 16);
  int16_t img  = (int16_t) (cmplxN & 0xFFFF); // losing sign, aksing compiler to treat as signed number
  printf("Unpacked Real in Decimal : %d \t in HEX : 0x%4x\n", real, real);
  printf("Unpacked Imag in Decimal : %d \t in HEX : 0x%4x\n", img,  img);

  //  printf("Unpacked Real : 0x%4x\n", cmplxN >> 16); // get upper bits
  // printf("Unpacked Imag : 0x%4x\n", (int16_t)(cmplxN & 0xFFFF));  // extract lower bits
  // return packedComplex;
}

void printComplexN( int32_t cmplxN){
  int16_t real = cmplxN >> 16;
  int16_t img  = (int16_t)(cmplxN & 0xFFFF);

  if (real >= 0 && img >= 0){printf("1 qrd  -> Complex number : %d + %dj\n", real, img);}   // +ve +ve
  else if (real < 0 && img >= 0) {printf(" 2nd qrd -> Complex number : %d + %dj\n", real, img);} //-ve +ve
  else if (real < 0 && img < 0) {printf(" 3rd qrd -> Complex number : %d%dj\n", real, img);} // -ve -ve
  else {printf(" 4th qrd -> Complex number : %d%dj\n", real, img);} // +ve -ve
}



int main(void) {
  printf("Welcome to Complex Numer Arithemetic /562\n");

  // int16_t real = 0xfffe;
  // uint16_t imag = 0xEEED;

  /* max positive */
  /* range is -2^N/2 to 2^N/2 -1 */
  /* −32,768 to +32,767 */
  /* Number mapping is */
  /*  0x8000  0x0000 0x7FFF */
  
  /* 32768 is 0x8000 */ //TWO complement
  /*    ~(8000) */
  /*    =0111 1111 1111 1111 */
  /*                      +1 */
  /*    -------------------- */
  /*    1000  0000 0000 0000 */
  ----------------------------
     /* -32768 = 0x8000  */

		   
  int16_t real = 32767; //int16_t(65535/2 -1);
  int16_t imag = 32767; //  int16_t(65535/2-1);

  //-32767 is 0x8001

  int32_t z1 = packComplex(0, 0);
  int32_t z2 = packComplex(real,     imag);   // first quard
  int32_t z3 = packComplex(-real-1,  imag);   // 2nd quard
  int32_t z4 = packComplex(-real-1, -imag-1); // third quard
  int32_t z5 = packComplex(real,    -imag-1); // 4th quard

  if(DEBUG_PRINT == 1){
    unPackComplex(z1);
    printComplexN(z1);

    unPackComplex(z2);
    printComplexN(z2);

    unPackComplex(z3);
    printComplexN(z3);

    unPackComplex(z4);
    printComplexN(z4);

    unPackComplex(z5);
    printComplexN(z5);
  }

  return 0;
}
