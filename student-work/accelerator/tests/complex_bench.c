/*
 * complex_bench.c -- cycle/instruction benchmark, ONE source for both configs
 *
 *   software build :  no flag      -> uses sw_cadd / sw_cmul
 *   accelerator    :  -DUSE_ROCC   -> uses AADD / ACMUL custom instructions
 *
 * Same loop, same volatile style, same counters => apples-to-apples.
 * Also times an EMPTY loop so loop overhead can be subtracted and you get
 * the true cost per operation.
 *
 * NOTE: the software path here is a stand-in. For the official baseline,
 * paste the bodies of Ashish's complex add / multiply into sw_cadd / sw_cmul
 * in acomplex.h (same 16/16 packed format) and re-run the software build.
 */
#include <stdint.h>
#include <stdio.h>
#include "acomplex.h"

#define N 1000

#ifdef USE_ROCC
  #define OP_ADD(x, y) hw_cadd(x, y)
  #define OP_MUL(x, y) hw_cmul(x, y)
  #define MODE "ACCELERATOR (AADD/ACMUL)"
#else
  #define OP_ADD(x, y) sw_cadd(x, y)
  #define OP_MUL(x, y) sw_cmul(x, y)
  #define MODE "SOFTWARE BASELINE"
#endif

#define READ_CSR(name, var) asm volatile("csrr %0, " name : "=r"(var))

int main(void) {
    uint64_t c0, c1, i0, i1;
    volatile cpx_t za = cpx_pack(50, 50);
    volatile cpx_t zb = cpx_pack(50, 50);
    volatile cpx_t result;
    uint64_t cyc_empty, ins_empty, cyc_add, ins_add, cyc_mul, ins_mul;

    printf("=== complex_bench: %s, N=%d ===\n", MODE, N);

    /* empty loop: measures loop + volatile load/store overhead only */
    READ_CSR("mcycle", c0); READ_CSR("minstret", i0);
    for (volatile int i = 0; i < N; i++) { result = za; }
    READ_CSR("mcycle", c1); READ_CSR("minstret", i1);
    cyc_empty = c1 - c0; ins_empty = i1 - i0;

    /* complex add */
    READ_CSR("mcycle", c0); READ_CSR("minstret", i0);
    for (volatile int i = 0; i < N; i++) { result = OP_ADD(za, zb); }
    READ_CSR("mcycle", c1); READ_CSR("minstret", i1);
    cyc_add = c1 - c0; ins_add = i1 - i0;

    /* complex multiply */
    READ_CSR("mcycle", c0); READ_CSR("minstret", i0);
    for (volatile int i = 0; i < N; i++) { result = OP_MUL(za, zb); }
    READ_CSR("mcycle", c1); READ_CSR("minstret", i1);
    cyc_mul = c1 - c0; ins_mul = i1 - i0;

    printf("empty loop : cycles %lu  instret %lu\n", (unsigned long)cyc_empty, (unsigned long)ins_empty);
    printf("complex add: cycles %lu  instret %lu\n", (unsigned long)cyc_add,   (unsigned long)ins_add);
    printf("complex mul: cycles %lu  instret %lu\n", (unsigned long)cyc_mul,   (unsigned long)ins_mul);
    printf("net add/op : cycles %lu  instret %lu  (x1000 = per-op * 1000)\n",
           (unsigned long)(cyc_add - cyc_empty), (unsigned long)(ins_add - ins_empty));
    printf("net mul/op : cycles %lu  instret %lu  (x1000 = per-op * 1000)\n",
           (unsigned long)(cyc_mul - cyc_empty), (unsigned long)(ins_mul - ins_empty));
    return 0;
}

