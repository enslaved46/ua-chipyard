// complex_acc64.h -- 32-bit complex numbers, software reference and accelerator calls
//
// Format: one complex number = one 64-bit value
//   real      = bits 63..32 (signed 32-bit)
//   imaginary = bits 31..0  (signed 32-bit)
// Results keep the low 32 bits of each part, so they wrap only past +/-2,147,483,647.
//
// Two groups of functions:
//   cAdd64, cMul64                         plain C reference (the software baseline)
//   cAddAcc64, cMulAcc64                   same math, done by the accelerator
//   cAccWrite64, cAccRead64, cAccAdd64     the accelerator's storage slots (0 to 3)
// On a normal computer (not RISC-V) the accelerator calls fall back to C and a small
// array that acts like the slots, so this file can be tested without the simulator.
#ifndef COMPLEX_ACC64_H
#define COMPLEX_ACC64_H

#include <stdint.h>

typedef struct {
  int64_t packed64;
} complex64_t;

static inline complex64_t makeComplex64(int32_t re, int32_t im) {
  complex64_t z;
  z.packed64 = (int64_t)(((uint64_t)(uint32_t)re << 32) | (uint64_t)(uint32_t)im);
  return z;
}

static inline int32_t getReal64(complex64_t z) {
  return (int32_t)(uint32_t)((uint64_t)z.packed64 >> 32);
}

static inline int32_t getImag64(complex64_t z) {
  return (int32_t)(uint32_t)((uint64_t)z.packed64 & 0xFFFFFFFFu);
}

// ---- software reference ------------------------------------------------------

static inline complex64_t cAdd64(complex64_t z1, complex64_t z2) {
  uint32_t re = (uint32_t)getReal64(z1) + (uint32_t)getReal64(z2);
  uint32_t im = (uint32_t)getImag64(z1) + (uint32_t)getImag64(z2);
  return makeComplex64((int32_t)re, (int32_t)im);
}

static inline complex64_t cMul64(complex64_t z1, complex64_t z2) {
  int64_t a = getReal64(z1);
  int64_t b = getImag64(z1);
  int64_t c = getReal64(z2);
  int64_t d = getImag64(z2);
  uint64_t re = (uint64_t)(a * c) - (uint64_t)(b * d);
  uint64_t im = (uint64_t)(a * d) + (uint64_t)(b * c);
  return makeComplex64((int32_t)(uint32_t)re, (int32_t)(uint32_t)im);
}

// ---- accelerator calls -------------------------------------------------------
// Each one is .insn r 0x0b, 7, <funct7>, rd, rs1, rs2
//   funct7 = 0 ACMUL, 1 AADD, 2 ACWR, 3 ACRD, 4 ACACC

#if !defined(__riscv)
static complex64_t sw_slots[4] __attribute__((unused));
#endif

static inline complex64_t cMulAcc64(complex64_t z1, complex64_t z2) {
#if defined(__riscv)
  uint64_t out;
  asm volatile(".insn r 0x0b, 7, 0, %0, %1, %2"
               : "=r"(out)
               : "r"((uint64_t)z1.packed64),
                 "r"((uint64_t)z2.packed64));
  complex64_t r;
  r.packed64 = (int64_t)out;
  return r;
#else
  return cMul64(z1, z2);
#endif
}

static inline complex64_t cAddAcc64(complex64_t z1, complex64_t z2) {
#if defined(__riscv)
  uint64_t out;
  asm volatile(".insn r 0x0b, 7, 1, %0, %1, %2"
               : "=r"(out)
               : "r"((uint64_t)z1.packed64),
                 "r"((uint64_t)z2.packed64));
  complex64_t r;
  r.packed64 = (int64_t)out;
  return r;
#else
  return cAdd64(z1, z2);
#endif
}

// Store z in a slot (0 to 3).
static inline void cAccWrite64(int slot, complex64_t z) {
#if defined(__riscv)
  uint64_t out;
  asm volatile(".insn r 0x0b, 7, 2, %0, %1, %2"
               : "=r"(out)
               : "r"((uint64_t)z.packed64),
                 "r"((uint64_t)slot));
  (void)out;
#else
  sw_slots[slot & 3] = z;
#endif
}

// Read the number stored in a slot.
static inline complex64_t cAccRead64(int slot) {
#if defined(__riscv)
  uint64_t out;
  asm volatile(".insn r 0x0b, 7, 3, %0, %1, %2"
               : "=r"(out)
               : "r"((uint64_t)0),
                 "r"((uint64_t)slot));
  complex64_t r;
  r.packed64 = (int64_t)out;
  return r;
#else
  return sw_slots[slot & 3];
#endif
}

// Add z into a slot (slot = slot + z) and return the new total.
static inline complex64_t cAccAdd64(int slot, complex64_t z) {
#if defined(__riscv)
  uint64_t out;
  asm volatile(".insn r 0x0b, 7, 4, %0, %1, %2"
               : "=r"(out)
               : "r"((uint64_t)z.packed64),
                 "r"((uint64_t)slot));
  complex64_t r;
  r.packed64 = (int64_t)out;
  return r;
#else
  sw_slots[slot & 3] = cAdd64(sw_slots[slot & 3], z);
  return sw_slots[slot & 3];
#endif
}

#endif
