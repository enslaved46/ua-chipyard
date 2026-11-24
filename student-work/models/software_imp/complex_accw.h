// complex_accw.h -- calls for the version 3 accelerator (ComplexNumAccW)
//
// This file ADDS to complex_acc64.h (keep that file in the same folder; it is included below).
// New instruction: ACMAC (funct7 = 5)
//   cMacAccW(z1, z2)   adds the exact product z1*z2 into slot 0 (real) and slot 1 (imaginary).
//                      Each slot is a whole signed 64-bit number, so the totals do not wrap at 32 bits.
//   cAccWriteW(slot,v) writes a full 64-bit number into a slot   (ACWR)
//   cAccReadW(slot)    reads a full 64-bit number from a slot    (ACRD)
// On a normal computer (not RISC-V) these fall back to C and an int64 array, so the file
// can be tested without the simulator.
#ifndef COMPLEX_ACCW_H
#define COMPLEX_ACCW_H

#include <stdint.h>
#include "complex_acc64.h"

#if !defined(__riscv)
static int64_t sw_slotsW[4] __attribute__((unused));
#endif

// slot 0 += real(z1*z2), slot 1 += imag(z1*z2), both at full 64-bit width.
static inline void cMacAccW(complex64_t z1, complex64_t z2) {
#if defined(__riscv)
  uint64_t out;
  asm volatile(".insn r 0x0b, 7, 5, %0, %1, %2"
               : "=r"(out)
               : "r"((uint64_t)z1.packed64),
                 "r"((uint64_t)z2.packed64));
  (void)out;
#else
  int64_t a = getReal64(z1);
  int64_t b = getImag64(z1);
  int64_t c = getReal64(z2);
  int64_t d = getImag64(z2);
  sw_slotsW[0] += a * c - b * d;
  sw_slotsW[1] += a * d + b * c;
#endif
}

// Write a whole 64-bit number into a slot (0 to 3).
static inline void cAccWriteW(int slot, int64_t v) {
#if defined(__riscv)
  uint64_t out;
  asm volatile(".insn r 0x0b, 7, 2, %0, %1, %2"
               : "=r"(out)
               : "r"((uint64_t)v),
                 "r"((uint64_t)slot));
  (void)out;
#else
  sw_slotsW[slot & 3] = v;
#endif
}

// Read a whole 64-bit number from a slot (0 to 3).
static inline int64_t cAccReadW(int slot) {
#if defined(__riscv)
  uint64_t out;
  asm volatile(".insn r 0x0b, 7, 3, %0, %1, %2"
               : "=r"(out)
               : "r"((uint64_t)0),
                 "r"((uint64_t)slot));
  return (int64_t)out;
#else
  return sw_slotsW[slot & 3];
#endif
}

#endif
