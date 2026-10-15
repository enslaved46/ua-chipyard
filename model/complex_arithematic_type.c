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
#include "complex.h"
#define TEST_MAKE_COMPLEX 0
#define TEST_CADD 1
#define TEST_CMUL 0


int main(void) {
  printf("Welcome to Complex Numer Arithemetic /562\n");

  const int16_t MAX_POS = 32767; //int16_t(65535/2 -1);
  const int16_t MAX_NEG = 32768; //  int16_t(65535/2-1);

  //-32767 is 0x8001
  // extreme test points
  complex_t z0 = makeComplex(0, 0);
  complex_t z1= makeComplex(1,  1); // 4th
  complex_t z2 = makeComplex(MAX_POS, MAX_POS);   // first quard
  complex_t z3 = makeComplex(MAX_NEG, MAX_POS);   // 2nd quard
  complex_t z4 = makeComplex(MAX_NEG, MAX_NEG); // third quard
  complex_t z5 = makeComplex(MAX_POS, MAX_NEG); // 4th quard

  // these data set  doesn't test boundry, due to over flow
  complex_t z6  = makeComplex(  50,   50); // first q
  complex_t z7  = makeComplex(-500,  500); // sec
  complex_t z8  = makeComplex(-100, -500); // thrid
  complex_t z9  = makeComplex(100,  -500); // 4th

 
  if(TEST_MAKE_COMPLEX == 1){
    printf("########################################################################\n");
    printf("Running Test for packing and packing complex number into 32'b single REG\n");
    printf("########################################################################\n");
    unPackComplex(z0);
    unPackComplex(z1);
    unPackComplex(z2);
    unPackComplex(z3);
    unPackComplex(z4);
    unPackComplex(z5);
  }

  if (TEST_CADD == 1){
    printf("########################################################################\n");
    printf("Running Test for Complex Addition                                       \n");
    printf("Results are wrapped around for big Number                               \n");
    printf("########################################################################\n");
    complex_t z10 = cAdd(z6, z6);
    complex_t z11 = cAdd(z7, z7);
    complex_t z12 = cAdd(z8, z8);
    complex_t z13 = cAdd(z9, z2);        // real overflows    
    complex_t zOverFlw = cAdd(z1, z2);  // wraps around
    /* printComplexForm(z10); */
    /* printComplexForm(z11); */
    /* printComplexForm(z12); */
    /* printComplexForm(z13); */
    printComplexForm(zOverFlw);
  }

  if (TEST_CMUL == 1){
    printf("########################################################################\n");
    printf("Running Test for Complex Multiplication                                 \n");
    printf("Results are wrapped around for big Number                               \n");
    printf("########################################################################\n");
    complex_t z14 = cMul(z6, z6);
    complex_t z15 = cMul(z7, z7);
    complex_t z16 = cMul(z8, z8);
    complex_t z17 = cMul(z9, z9);

    printComplexForm(z14);
    printComplexForm(z15);
    printComplexForm(z16);
    printComplexForm(z17);
  }
  return 0;
}
