#include <stdio.h>
// #include <riscv-pk/encoding.h>
#include <stdint.h>
#include "complex.h"
#define TEST_MAKE_COMPLEX 0
#define TEST_CADD 1
#define TEST_CMUL 0
#define MAX_VECT  50

#define POS_NUMBER_VECTOR_ADD_TEST 1
#define NEG_NUMBER_VECTOR_ADD_TEST 1

#define COMPLEX_VECTOR_ADD_TEST 1

int main(void) {
  printf("Welcome to Complex Numer Arithemetic /562\n");

  complex_t z1[MAX_VECT] = {0};
  complex_t z2[MAX_VECT] = {0};
  complex_t addResult= {0};

  if(POS_NUMBER_VECTOR_ADD_TEST){
    // fill pos numbers
    for (int i = 0; i<MAX_VECT; i++){
      z1[i] = makeComplex(i,i);
      z2[i] = makeComplex(i,i);
    }

    /* for (int i = 0; i<MAX_VECT; i++){ */
    /*   printComplexForm(z1[i]); */
    /* } */

    printf("########################################################################\n");
    printf("Running Test for first N Vector of Positive numbers                   \n");
    printf("######################################################################\n");

    addResult =  cVectAdd(z1, z1, MAX_VECT );
    printComplexForm(addResult);
  }
  if(NEG_NUMBER_VECTOR_ADD_TEST){
    // fill NEG numbers
    for (int i = 0; i<MAX_VECT; i++){
      z1[i] = makeComplex(-i*10,-i*10);
      z2[i] = makeComplex(-i*10,-i*10);
    }

    printf("########################################################################\n");
    printf("Running Test for first N Vector of Negative numbers                     \n");
    printf("########################################################################\n");
    for (int i = 0; i<MAX_VECT; i++){
      printComplexForm(z1[i]);
    }

    addResult =  cVectAdd(z1, z1, MAX_VECT );
    printComplexForm(addResult);
  }
  return 0;

}
