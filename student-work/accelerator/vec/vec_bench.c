/* vec_bench.c - Ashish's vector add / multiply (complex.h) vs the ACMUL/AADD accelerator.
 *
 *   -DVEC_N=<n>   vector length (50, 500, 5000)
 *   -DOP_MUL      multiply (default is add)
 *   -DUSE_ROCC    use the accelerator instructions (run on ComplexAccelConfig only)
 *
 * Software path = Ashish's cVectAdd / cVectMul, unchanged (from complex.h).
 * Same inputs as his tests: z1[i] = (i, 2i), z2[i] = (2i, 3i), i = 1..N.
 * One timed pass (BENCH_BEGIN/BENCH_END from metrics.h), same as Ashish's tests: no warm-up.
 * Nothing is printed inside the timed region and the vectors are never printed.
 */
#include <stdio.h>
#include <stdint.h>
#if defined(__riscv)
#include "../../metrics.h"
#endif
#include "../../models/software_imp/complex.h"

#ifndef VEC_N
#define VEC_N 50
#endif

#ifdef OP_MUL
#define OPNAME "vmul"
#define SW_VEC(a, b, r, n) cVectMul(a, b, r, n)
#define HW_OP(x, y) hw_cmul(x, y)
#else
#define OPNAME "vadd"
#define SW_VEC(a, b, r, n) cVectAdd(a, b, r, n)
#define HW_OP(x, y) hw_cadd(x, y)
#endif

#ifdef USE_ROCC
#define MODE "hw"
#else
#define MODE "sw"
#endif

/* accelerator wrappers (custom-0, funct3=7; funct7 0 = ACMUL, 1 = AADD) */
static inline int32_t hw_cmul(int32_t x, int32_t y)
{
#if defined(USE_ROCC) && defined(__riscv)
    uint64_t rd;
    asm volatile(".insn r 0x0b, 7, 0, %0, %1, %2"
                 : "=r"(rd) : "r"((uint64_t)(uint32_t)x), "r"((uint64_t)(uint32_t)y));
    return (int32_t)rd;
#else
    complex_t a, b; a.packedComplex = x; b.packedComplex = y;   /* x86 check only */
    return cMul(a, b).packedComplex;
#endif
}

static inline int32_t hw_cadd(int32_t x, int32_t y)
{
#if defined(USE_ROCC) && defined(__riscv)
    uint64_t rd;
    asm volatile(".insn r 0x0b, 7, 1, %0, %1, %2"
                 : "=r"(rd) : "r"((uint64_t)(uint32_t)x), "r"((uint64_t)(uint32_t)y));
    return (int32_t)rd;
#else
    complex_t a, b; a.packedComplex = x; b.packedComplex = y;   /* x86 check only */
    return cAdd(a, b).packedComplex;
#endif
}

static complex_t z1[VEC_N], z2[VEC_N], out[VEC_N], ref[VEC_N];

static __attribute__((noinline)) void run_kernel(complex_t *a, complex_t *b, complex_t *r, int n)
{
#ifdef USE_ROCC
    for (int i = 0; i < n; i++)
        r[i].packedComplex = HW_OP(a[i].packedComplex, b[i].packedComplex);
#else
    SW_VEC(a, b, r, n);
#endif
}

int main(void)
{
    for (int i = 1; i <= VEC_N; i++) {
        z1[i - 1] = makeComplex(i, i * 2);
        z2[i - 1] = makeComplex(i * 2, i * 3);
    }
    for (int i = 0; i < VEC_N; i++)
        out[i].packedComplex = 0;                /* like his '= {0}' result array */

#if defined(__riscv)
    {
        BENCH_BEGIN();
        asm volatile("" ::: "memory");
        run_kernel(z1, z2, out, VEC_N);          /* timed */
        asm volatile("" ::: "memory");
        BENCH_END(OPNAME "_" MODE);
    }
#else
    run_kernel(z1, z2, out, VEC_N);
#endif

    SW_VEC(z1, z2, ref, VEC_N);                  /* Ashish's software result = reference */
    int mismatches = 0;
    uint32_t checksum = 0;
    for (int i = 0; i < VEC_N; i++) {
        if (out[i].packedComplex != ref[i].packedComplex) mismatches++;
        checksum += (uint32_t)out[i].packedComplex;
    }
    printf("RESULT op=%s mode=%s N=%d mismatches=%d checksum=0x%x\n",
           OPNAME, MODE, VEC_N, mismatches, (unsigned)checksum);
    return mismatches ? 1 : 0;
}
