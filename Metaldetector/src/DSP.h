#ifndef DSP_H
#define DSP_H

#include <stdint.h>

int16_t DSP_apply_hanning(int16_t sample, uint8_t n);
int16_t DSP_IIR_filter(int16_t x, int16_t y_prev);

#endif