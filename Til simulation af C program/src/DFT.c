#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "DFT.h"

static z_struct z;  // persistent accumulator

void DFT_reset(void) {
    z.re = 0;
    z.im = 0;
}

z_struct DFT_get(void) {
    return z;
}

// Goertzel-style accumulation for bin k=16 (2kHz at 8kHz sample rate)
// cos/sin values at k=16 cycle through: 1,0,-1,0 and 0,-1,0,1
void DFT_accum(int16_t sample, uint16_t n) {
    switch (n & 3) {
        case 0: z.re += sample; break;
        case 1: z.im -= sample; break;
        case 2: z.re -= sample; break;
        case 3: z.im += sample; break;
    }
}