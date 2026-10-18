#include <stdio.h>

#if defined(__riscv)
   #include <riscv-pk/encoding.h>
   #include "../../metrics.h"
#endif

#include <stdint.h>
#include "complex.h"

#define MAX_VECT  10
#define COMPLEX_VECTOR_MUL_TEST 1

int main(void) {
  complex_t z1[MAX_VECT] = {0};
  complex_t z2[MAX_VECT] = {0};
  complex_t mulResult[MAX_VECT]= {0};

  // fill pos numbers
  if(COMPLEX_VECTOR_MUL_TEST){
    int i = 1;
    for (i; i<=MAX_VECT; i++){
      printf("%d \t", i);
      z1[i-1] = makeComplex(i,i*2);
      z2[i-1] = makeComplex(i*2,i*3);
    }
	#if defined(__riscv)
		hpm_setup();
	#endif
    
    printf("\n");
    printf("########################################################################\n");
    printf("Running Test for  Vector Multiplication of complex Numbers              \n");
    printf("########################################################################\n");

	#if defined(__riscv)
	    BENCH_BEGIN();
	    cVectMul(z1, z2, mulResult, MAX_VECT );
	    BENCH_END("complex_vector_add");
	#else
		cVectMul(z1, z2, mulResult, MAX_VECT );
	#endif

    //printComplexVector(z1, MAX_VECT);
    //printComplexVector(z2, MAX_VECT);
    printComplexVector(mulResult, MAX_VECT);
  }
  return 0;
}
