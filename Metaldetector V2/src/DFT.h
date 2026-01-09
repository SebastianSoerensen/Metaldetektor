#ifndef DFT_H
#define DFT_H

#include <stdint.h>
#include "config.h"

typedef struct {
    int32_t re;
    int32_t im;
} z_struct;

// Declared in ADC.c - extern only here
extern volatile z_struct dft_latched[2];
extern volatile uint8_t  dft_ready[2];

// Accumulate sample into DFT (Goertzel at bin k=16 for 2kHz)
void DFT_accum(int16_t sample, uint16_t n);

// Reset accumulator
void DFT_reset(void);

// Get current accumulator value
z_struct DFT_get(void);

#endif