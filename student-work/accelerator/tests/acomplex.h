/*
 * acomplex.h  -- shared helpers for the ACMUL / AADD accelerator tests
 *
 * Data format (same as Ashish's baseline and the Matlab model):
 *   one complex number = one 32-bit word
 *   real      = bits 31..16 (signed 16-bit)
 *   imaginary = bits 15..0  (signed 16-bit)
 *   overflow WRAPS (does not saturate)
 *
 * Build modes:
 *   no flag      -> hw_cadd / hw_cmul fall back to the software model
 *                   (lets you sanity-check the tests on x86 with plain gcc)
 *   -DUSE_ROCC   -> hw_cadd / hw_cmul emit the real custom instructions
 *                   (only run this on the accelerator simulator!)
 *
 * Instruction encoding (RoCC, custom-0 opcode = 0x0b):
 *   funct3 = 7 (xd=1, xs1=1, xs2=1: uses rd, rs1, rs2)
 *   funct7 = 0 -> ACMUL   rd = rs1 * rs2   (complex multiply)
 *   funct7 = 1 -> AADD    rd = rs1 + rs2   (complex add)
 */
#ifndef ACOMPLEX_H
#define ACOMPLEX_H

#include <stdint.h>

typedef uint32_t cpx_t;

static inline cpx_t cpx_pack(int16_t re, int16_t im) {
    return ((uint32_t)(uint16_t)re << 16) | (uint16_t)im;
}
static inline int16_t cpx_re(cpx_t z) { return (int16_t)(z >> 16); }
static inline int16_t cpx_im(cpx_t z) { return (int16_t)(z & 0xFFFFu); }

/* ---------------- software model (golden reference in C) ---------------- */
static inline cpx_t sw_cadd(cpx_t x, cpx_t y) {
    return cpx_pack((int16_t)(cpx_re(x) + cpx_re(y)),
                    (int16_t)(cpx_im(x) + cpx_im(y)));
}

static inline cpx_t sw_cmul(cpx_t x, cpx_t y) {
    int32_t a = cpx_re(x), b = cpx_im(x);
    int32_t c = cpx_re(y), d = cpx_im(y);
    return cpx_pack((int16_t)(a * c - b * d),
                    (int16_t)(a * d + b * c));
}

/* ---------------- hardware (accelerator) versions ----------------------- */
#ifdef USE_ROCC
static inline cpx_t hw_cmul(cpx_t x, cpx_t y) {
    uint64_t rd;
    asm volatile(".insn r 0x0b, 7, 0, %0, %1, %2"
                 : "=r"(rd) : "r"((uint64_t)x), "r"((uint64_t)y));
    return (cpx_t)rd;
}
static inline cpx_t hw_cadd(cpx_t x, cpx_t y) {
    uint64_t rd;
    asm volatile(".insn r 0x0b, 7, 1, %0, %1, %2"
                 : "=r"(rd) : "r"((uint64_t)x), "r"((uint64_t)y));
    return (cpx_t)rd;
}
#else
static inline cpx_t hw_cmul(cpx_t x, cpx_t y) { return sw_cmul(x, y); }
static inline cpx_t hw_cadd(cpx_t x, cpx_t y) { return sw_cadd(x, y); }
#endif

#endif /* ACOMPLEX_H */

