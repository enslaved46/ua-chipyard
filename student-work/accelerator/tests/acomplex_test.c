/*
 * acomplex_test.c -- correctness check for ACMUL / AADD
 *
 * Runs the SAME inputs through the software model and through the
 * accelerator instructions and compares every result bit-for-bit.
 * Also checks the known-good table from the baseline (Ashish's output /
 * Matlab model) so we know the software model itself is right.
 *
 * x86 sanity check (accelerator calls fall back to software):
 *     gcc -I. acomplex_test.c -o t && ./t
 * RISC-V accelerator run:  build with -DUSE_ROCC (see README).
 */
#include <stdio.h>
#include <stdint.h>
#include "acomplex.h"

static int fails = 0, checks = 0;

static void show(const char *tag, cpx_t z) {
    if (cpx_im(z) >= 0) printf("%s %d + %dj", tag, cpx_re(z), cpx_im(z));
    else                printf("%s %d - %dj", tag, cpx_re(z), -(int)cpx_im(z));
}

static void check(const char *what, cpx_t got, cpx_t want) {
    checks++;
    if (got != want) {
        fails++;
        printf("FAIL %s: ", what);
        show("got", got);
        show("  want", want);
        printf("\n");
    }
}

int main(void) {
    /* ---- 1. known-good table (z op z) from baseline / Matlab model ---- */
    const int16_t in[4][2]      = {{50,50},{-500,500},{-100,-500},{100,-500}};
    const int16_t add_exp[4][2] = {{100,100},{-1000,1000},{-200,-1000},{200,-1000}};
    const int16_t mul_exp[4][2] = {{0,5000},{0,24288},{22144,-31072},{22144,31072}};

    for (int i = 0; i < 4; i++) {
        cpx_t z = cpx_pack(in[i][0], in[i][1]);
        check("table add (sw)", sw_cadd(z, z), cpx_pack(add_exp[i][0], add_exp[i][1]));
        check("table mul (sw)", sw_cmul(z, z), cpx_pack(mul_exp[i][0], mul_exp[i][1]));
        check("table add (hw)", hw_cadd(z, z), cpx_pack(add_exp[i][0], add_exp[i][1]));
        check("table mul (hw)", hw_cmul(z, z), cpx_pack(mul_exp[i][0], mul_exp[i][1]));
    }

    /* ---- 2. extreme corners (from Ashish's .m test points) ---- */
    const int16_t ext[] = {0, 1, -1, 32767, -32768, 32766, -32767, 255, -256};
    const int n = sizeof(ext) / sizeof(ext[0]);
    for (int a = 0; a < n; a++)
      for (int b = 0; b < n; b++)
        for (int c = 0; c < n; c++)
          for (int d = 0; d < n; d++) {
              cpx_t x = cpx_pack(ext[a], ext[b]);
              cpx_t y = cpx_pack(ext[c], ext[d]);
              check("corner add", hw_cadd(x, y), sw_cadd(x, y));
              check("corner mul", hw_cmul(x, y), sw_cmul(x, y));
          }

    /* ---- 3. pseudo-random sweep (deterministic LCG, no libc rand) ---- */
    uint32_t s = 12345u;
    for (int i = 0; i < 2000; i++) {
        s = s * 1664525u + 1013904223u; cpx_t x = s;
        s = s * 1664525u + 1013904223u; cpx_t y = s;
        check("random add", hw_cadd(x, y), sw_cadd(x, y));
        check("random mul", hw_cmul(x, y), sw_cmul(x, y));
    }

    printf("checks: %d  failures: %d\n", checks, fails);
    printf(fails == 0 ? "RESULT: PASS\n" : "RESULT: FAIL\n");
    return fails != 0;
}

