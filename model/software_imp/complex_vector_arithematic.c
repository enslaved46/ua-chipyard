#include <stdio.h>

#if defined(__riscv)
#include <riscv-pk/encoding.h>
#endif
   
#include <stdint.h>
#include "complex.h"
#define TEST_MAKE_COMPLEX 0
#define TEST_CADD 1
#define TEST_CMUL 1
#define MAX_VECT  5

#define POS_NUMBER_VECTOR_ADD_TEST 0
#define NEG_NUMBER_VECTOR_ADD_TEST 0

#define COMPLEX_VECTOR_ADD_TEST 1

int main(void) {
  complex_t z1[MAX_VECT] = {0};
  complex_t z2[MAX_VECT] = {0};
  complex_t addResult[MAX_VECT]= {0};

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

    cVectAdd(z1, z1, addResult, MAX_VECT );    
    printComplexVector(addResult, MAX_VECT);
  }
  if(NEG_NUMBER_VECTOR_ADD_TEST){
    // fill NEG numbers
    for (int i = 0; i<MAX_VECT; i++){
      z1[i] = makeComplex(-i*10,-i*10);
      z2[i] = makeComplex(-i*10,-i*10);
    }

    printf("########################################################################\n");
    printf("Running Vector Addition Test for first N Vector of Negative numbers     \n");
    printf("########################################################################\n");
    cVectAdd(z1, z1, addResult, MAX_VECT );
    printComplexVector(z1, MAX_VECT);
    printComplexVector(addResult, MAX_VECT);
  }

  printf("########################################################################\n");
  printf("Running CMUL Vector TEST for first N Vector of Negative numbers         \n");
  printf("########################################################################\n");

    for (int i = 0; i<MAX_VECT; i++){
      z1[i] = makeComplex(-i*10,-i*10);
      z2[i] = makeComplex(-i*10,-i*10);
    }
  cVectMul(z1, z1, addResult, MAX_VECT );
  printComplexVector(z1, MAX_VECT);
  printComplexVector(addResult, MAX_VECT);
  return 0;
}
