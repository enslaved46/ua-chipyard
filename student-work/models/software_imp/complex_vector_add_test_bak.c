#include <stdio.h>
#include <riscv-pk/encoding.h>
#include <stdint.h>
#include "complex.h"
#include "metrics.h"


#define TEST_MAKE_COMPLEX 0
#define TEST_CADD 1

#define MAX_VECT  500

#define POS_NUMBER_VECTOR_ADD_TEST 1
#define NEG_NUMBER_VECTOR_ADD_TEST 0

#define COMPLEX_VECTOR_ADD_TEST 1

int main(void) {
  complex_t z1[MAX_VECT] = {0};
  complex_t z2[MAX_VECT] = {0};
  complex_t addResult[MAX_VECT]= {0};

    hpm_setup();
  // fill pos numbers
  if(POS_NUMBER_VECTOR_ADD_TEST){
    int16_t i = 1;
    for (i; i<=MAX_VECT; i++){
     // printf("%d \t", i);
      z1[i-1] = makeComplex(i,i*2);
      z2[i-1] = makeComplex(i*2,i*3);
    }
    // printf("\n");
    printf("########################################################################\n");
    printf("Running Test for first N Vector of Positive numbers                     \n");
    printf("########################################################################\n");
    
    BENCH_BEGIN();
    cVectAdd(z1, z2, addResult, MAX_VECT );
    BENCH_END("complex_vector)add");
    
    // printComplexVector(z1, MAX_VECT);
    // printComplexVector(z2, MAX_VECT);
    printComplexVector(addResult, MAX_VECT);
  }

  // fill NEG numbers
  if(NEG_NUMBER_VECTOR_ADD_TEST){
    for (int16_t i = 1; i <= MAX_VECT; i++){
      z1[i-1] = makeComplex(-i*10,-i*20);
      z2[i-1] = makeComplex(-i*20,-i*30);
    }
    printf("########################################################################\n");
    printf("Running Vector Addition Test for first N Vector of Negative numbers     \n");
    printf("########################################################################\n");
    cVectAdd(z1, z2, addResult, MAX_VECT );
    // printComplexVector(z1, MAX_VECT);
    printComplexVector(addResult, MAX_VECT);
  }

  return 0;
}
