#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "DFT.h"

static z_struct z;  // persistent akkumulator

void DFT_reset(void){z.re = 0; z.im = 0;}

z_struct DFT_get(void){ 
    return z;
}

z_struct DFT_accum(const int16_t sample, uint16_t n){
    switch (n & 3){
        case 0: z.re += sample; break;
        case 1: z.im -= sample; break;
        case 2: z.re -= sample; break;
        case 3: z.im += sample; break;
    }
    return z;
}
