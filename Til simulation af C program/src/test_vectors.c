#include "test_vectors.h"
#include <avr/pgmspace.h>

/*
 * Test signal: 30° faseskift ved 2 kHz
 *
 *   fs = 8000 Hz
 *   f  = 2000 Hz
 *   N  = 64
 *   x[n] = 1000 * sin(2*pi*2000*n/8000 + 30°)
 *
 * For 2 kHz at 8 kHz: one period = 4 samples.
 *
 * Using: sin(θ + 30°)
 *   sin(30°)  =  0.5
 *   sin(120°) =  0.866
 *   sin(210°) = -0.5
 *   sin(300°) = -0.866
 *
 * Scaled by 1000 and rounded:
 *   { +500, +866, -500, -866 }
 */

const int16_t test_samples[ADC_BLOCK_N] PROGMEM = {
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866,
     500,   866,  -500,  -866
};

/*
 * ============================
 * Reference DFT results
 * ============================
 *
 * For samples {500, 866, -500, -866}:
 *
 * n=0: re +=  500        -> re =  500
 * n=1: im -=  866       -> im = -866
 * n=2: re -= (-500)     -> re = 1000
 * n=3: im += (-866)     -> im = -1732
 *
 * Over one period:
 *   re = +1000
 *   im = -1732
 *
 * Over 16 periods (N=64):
 *   z.re = +16000
 *   z.im = -27712
 *
 * Normalized:
 *   re = 16000 / 64 = 250
 *   im = -27712 / 64 ≈ -433
 *
 * Fast magnitude:
 *   amp = |250| + |433| = 683
 *
 * BUT: because your test amplitude here is not exactly 1000*sin()
 * at all points (due to rounding), the magnitude deviates.
 *
 * If we instead compare phase only:
 *   phase = atan2(im, re) = atan2(-433, 250) ≈ -60°
 */

#define TEST_REF_RE     250
#define TEST_REF_IM    -433
#define TEST_REF_AMP    683
#define TEST_REF_PHASE  -60
