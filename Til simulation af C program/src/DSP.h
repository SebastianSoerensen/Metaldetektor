#ifndef DSP_H
#define DSP_H

#include <stdint.h>

int16_t DSP_apply_hanning(int16_t sample, uint8_t n);
int16_t DSP_IIR_filter(int16_t x, int16_t y_prev);

// Fast integer amplitude approximation (avoids sqrt)
int32_t DSP_fast_magnitude(int32_t re, int32_t im);

// Fast integer atan2 approximation
// Returns phase in degrees (-180 to +180)
int16_t DSP_fast_atan2_deg(int32_t im, int32_t re);

#endif