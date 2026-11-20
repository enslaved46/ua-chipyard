#include <stdio.h>

#if defined(__riscv)
   #include <riscv-pk/encoding.h>
   #include "../../metrics.h"
#endif

#include <stdint.h>
#include "complex.h"
#define TEST_MAKE_COMPLEX 0
#define TEST_CADD 1

#define MAX_VECT  10

#define POS_NUMBER_VECTOR_ADD_TEST 1
#define NEG_NUMBER_VECTOR_ADD_TEST 0


int main(void) {
  complex_t z1[MAX_VECT] = {0};
  complex_t z2[MAX_VECT] = {0};
  complex_t addResult[MAX_VECT]= {0};

  // fill pos numbers
  if(POS_NUMBER_VECTOR_ADD_TEST){
    int i = 1;
    for (i; i<=MAX_VECT; i++){
      printf("%d \t", i);
      z1Arr[i-1] = makeComplex16(i,i*2);
      z2Arr[i-1] = makeComplex16(i*2,i*3);
    }
     printf("\n");
    printf("########################################################################\n");
    printf("Running Test for first N Vector of Positive numbers                     \n");
    printf("########################################################################\n");

	#if defined(__riscv)
	    BENCH_BEGIN();
    	cVectAdd(z1Arr, z2Arr, addResult, MAX_VECT );
	    BENCH_END("complex_vector_add");
	#else
		cVectAdd(z1Arr, z2Arr, addResult, MAX_VECT );
	#endif
    // CPU performs the array loads and stores
    // software-managed vector loop,
    cVectAdd(z1Arr, z2Arr, addResult, MAX_VECT );
    // printComplexVector(z1Arr, MAX_VECT);
    // printComplexVector(z2Arr, MAX_VECT);
    printComplexVector(addResult, MAX_VECT);
  }

  // fill NEG numbers
  if(NEG_NUMBER_VECTOR_ADD_TEST){
    for (int i = 1; i <= MAX_VECT; i++){
      z1Arr[i-1] = makeComplex16(-i*10,-i*20);
      z2Arr[i-1] = makeComplex16(-i*20,-i*30);
    }
    printf("########################################################################\n");
    printf("Running Vector Addition Test for first N Vector of Negative numbers     \n");
    printf("########################################################################\n");
    cVectAdd(z1Arr, z2Arr, addResult, MAX_VECT );
    // printComplexVector(z1Arr, MAX_VECT);
    printComplexVector(addResult, MAX_VECT);
  }

  return 0;
}
