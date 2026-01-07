#ifndef DSP_H
#define DSP_H

#include "config.h"
#include <avr/pgmspace.h>
#include <avr/io.h>

// LUT for Hanning window saved in FLASH
const int16_t hanning[ADC_BLOCK_N + 1] PROGMEM = {
       0,    655,   3277,   7211,  11305,  15784,  20471,  25380,
   30530,  35939,  41621,  47583,  53837,  60391,  67250,  74419,
   81905,  89711,  97843, 106305, 115103, 124242, 133728, 143566,
  153761, 164318, 175243, 186540, 198213, 210269, 222713, 235551,
  248789, 262432, 276486, 290957, 305852, 321177, 336940, 353147,
  369805, 386921, 404503, 422557, 441089, 460107, 479617, 499627,
  520143, 541174, 562726, 584806, 607420, 630576, 654280, 678539,
  703359, 728748, 754712, 781258, 808392, 836122, 864454, 893394,
  922950
};

static inline int16_t apply_hanning(int16_t sample, uint8_t n);

static inline int16_t IIR_filter(int16_t x, int16_t y_prev);

#endif