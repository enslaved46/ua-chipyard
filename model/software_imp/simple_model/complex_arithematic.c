/* Implementation notes */

/* 32 bits Signed INT complex number software imp */
/* 16 bits real and 16 bits img */
/* MSB is real and LSB is IMG */
/* 31     16 15     0 */
/*    Real      IMG */
/* ----------------------------------------------------------- */
// Review : How nuber gets mapped into Hardawre
/* max positive                      */
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
/* ---------------------------- */
/* -32768 = 0x8000  */
/* ----------------------------------------------------------- */


#include <stdio.h>
// #include <riscv-pk/encoding.h>
#include <stdint.h>

#define TEST_PACK_UNPACK 0
#define TEST_CADD 1
#define TEST_CMUL 1

typedef struct{
  int16_t real;
  int16_t complex;
}complex_t;
  

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

void printComplexN(int32_t cmplxN){
  int16_t real = cmplxN >> 16;
  int16_t img  = (int16_t)(cmplxN & 0xFFFF);

  if (real >= 0 && img >= 0){printf("1 qrd  -> Complex number : %d + %dj\n", real, img);}   // +ve +ve
  else if (real < 0 && img >= 0) {printf("2nd qrd -> Complex number : %d + %dj\n", real, img);} //-ve +ve
  else if (real < 0 && img < 0) {printf("3rd qrd -> Complex number : %d%dj\n", real, img);} // -ve -ve
  else {printf("4th qrd -> Complex number : %d%dj\n", real, img);} // +ve -ve
}

// we will have probelm with overflow, don't worry about overflow
// z = z1 + z2 = (a+c) + (b+d)i;
int32_t cAdd(int32_t z1, int32_t z2){
  return ((((z1 >> 16) + (z2 >> 16) & 0xFFFF) << 16)           // shift and add then Mask drop bits , pack into 16 then move back up to upper 32
	  |
	  ((z1 & 0xFFFF) + (z2 & 0xFFFF) & 0xFFFF));
}

// z = z1* z2 =(ac + adi + bci + bdi^2) = (ac - bd) + (ad +bc)i
int32_t cMul(int32_t z1, int32_t z2){ // (a+bi) (c+di)

  int32_t  a = z1 >> 16;
  int32_t  b = z1 & 0XFFFF;
  int32_t  c = z2 >> 16;
  int32_t  d = z2 & 0XFFFF;
  
  return ( ((( (a*c) - (b*d) )  & 0xFFFF) << 16 ) |
	   (( (a*d) + (b*c) ) & 0xFFFF) );
  /* return (( */
  /* 	     (z1 >> 16 * z2 >> 16)   // a x c  */
  /*          -       */
  /* 	     (z1 & 0xFFFF *  z2 & 0xFFFF) // b * d */
  /*          & 0xFFFFF */
  /* 	     ) << 16)           // shift and add then Mask drop bits , pack into 16 then move back up to upper 32 */
  /*        | */
  /*        ((z1 >> 16  * z2 & 0xFFFF)  // a *d */
  /*         + */
  /*         (z1 & 0xFFFF * z1 >> 16) */
  /*         & 0xFFFF */
  /* 	    ); */
}

int main(void) {
  printf("Welcome to Complex Numer Arithemetic /562\n");

  int16_t real = 32767; //int16_t(65535/2 -1);
  int16_t imag = 32767; //  int16_t(65535/2-1);

  //-32767 is 0x8001
  // extreme test points
  int32_t z1 = packComplex(0, 0);
  int32_t z2 = packComplex(real,     imag);   // first quard
  int32_t z3 = packComplex(-real-1,  imag);   // 2nd quard
  int32_t z4 = packComplex(-real-1, -imag-1); // third quard
  int32_t z5 = packComplex(real,    -imag-1); // 4th quard

  // these data set  doesn't test boundry, due to over flow
  int32_t z6 = packComplex(  50,   50); // first q
  int32_t z7 = packComplex(-500,  500); // sec
  int32_t z8 = packComplex(-100, -500); // thrid
  int32_t z9 = packComplex(100,  -500); // 4th
 
  if(TEST_PACK_UNPACK == 1){
    printf("########################################################################\n");
    printf("Running Test for packing and packing complex number into 32'b single REG\n");
    printf("########################################################################\n");
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

  if (TEST_CADD == 1){
    printf("########################################################################\n");
    printf("Running Test for Complex Addition                                       \n");
    printf("Results are wrapped around for big Number                               \n");
    printf("########################################################################\n");
    int32_t z10 = cAdd(z6, z6);
    int32_t z11 = cAdd(z7, z7);
    int32_t z12 = cAdd(z8, z8);
    int32_t z13 = cAdd(z9, z9);
 
    printComplexN(z10);
    printComplexN(z11);
    printComplexN(z12);
    printComplexN(z13);

  }

  if (TEST_CMUL == 1){
    printf("########################################################################\n");
    printf("Running Test for Complex Multiplication                                 \n");
    printf("Results are wrapped around for big Number                               \n");
    printf("########################################################################\n");
    int32_t z14 = cMul(z6, z6);
    int32_t z15 = cMul(z7, z7);
    int32_t z16 = cMul(z8, z8);
    int32_t z17 = cMul(z9, z9);

    printComplexN(z14);
    printComplexN(z15);
    printComplexN(z16);
    printComplexN(z17);
  }
  return 0;
}
