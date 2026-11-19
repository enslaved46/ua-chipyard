/* complex_acc.h -- calls the ComplexNumAcc accelerator (ACMUL / AADD)
 *
 * Same packed format as complex.h: real = bits 31..16, imaginary = bits 15..0,
 * each a signed 16-bit number. The instruction is:
 *   .insn r 0x0b, 7, funct7, rd, rs1, rs2
 *     0x0b   = custom-0 opcode
 *     7      = funct3 (xd=1, xs1=1, xs2=1: use rd, rs1, rs2)
 *     funct7 = 0 -> ACMUL (complex multiply), 1 -> AADD (complex add)
 *
 * On RISC-V this issues the custom instruction, so it only works on a
 * simulator built with the accelerator (ComplexNumAccConfig).
 * On any other machine it falls back to the software functions cAdd / cMul.
 */
#ifndef COMPLEX_ACC_H
#define COMPLEX_ACC_H

#include <stdint.h>
#include "complex.h"

static inline complex_t cAddAcc(complex_t z1, complex_t z2) {
#if defined(__riscv)
  uint64_t out;
  asm volatile(".insn r 0x0b, 7, 1, %0, %1, %2"
               : "=r"(out)
               : "r"((uint64_t)(uint32_t)z1.packedComplex),
                 "r"((uint64_t)(uint32_t)z2.packedComplex));
  complex_t r;
  r.packedComplex = (int32_t)out;
  return r;
#else
  return cAdd(z1, z2);
#endif
}

static inline complex_t cMulAcc(complex_t z1, complex_t z2) {
#if defined(__riscv)
  uint64_t out;
  asm volatile(".insn r 0x0b, 7, 0, %0, %1, %2"
               : "=r"(out)
               : "r"((uint64_t)(uint32_t)z1.packedComplex),
                 "r"((uint64_t)(uint32_t)z2.packedComplex));
  complex_t r;
  r.packedComplex = (int32_t)out;
  return r;
#else
  return cMul(z1, z2);
#endif
}

/* vector versions: same loop as cVectAdd / cVectMul in complex.h */
static inline void cVectAddAcc(complex_t z1[], complex_t z2[], complex_t result[], int size) {
  for (int i = 0; i < size; i++) {
    result[i] = cAddAcc(z1[i], z2[i]);
  }
}

static inline void cVectMulAcc(complex_t z1[], complex_t z2[], complex_t result[], int size) {
  for (int i = 0; i < size; i++) {
    result[i] = cMulAcc(z1[i], z2[i]);
  }
}

#endif
