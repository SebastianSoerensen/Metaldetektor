#include "config.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <math.h>
#include "DSP.h"

#define RAD2DEG 57.29577951308232
// LUT for Hanning window saved in FLASH
static const int32_t hanning[ADC_BLOCK_N + 1] PROGMEM = {
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


int16_t DSP_apply_hanning(int16_t sample, uint8_t n) {
    int16_t w = pgm_read_word(&hanning[n]);
    int32_t temp = (int32_t)sample * w;
    return (int16_t)(temp >> 15);
}

int16_t DSP_IIR_filter(int16_t x, int16_t y_prev) {
    const int16_t a = 9830; // 0.3 in Q15
    int32_t temp = (int32_t)(32768 - a) * y_prev + (int32_t)a * x;
    return (int16_t)(temp >> 15);
}
// TRUE magnitude
int32_t DSP_true_magnitude(int32_t re, int32_t im){
    int64_t acc = (int64_t)re * re +(int64_t)im * im;
    return (int32_t)sqrt((double)acc);
}


// Fast magnitude approximation: max(|re|,|im|) + 0.4*min(|re|,|im|)
// Error < 4%, much faster than sqrt on AVR

int32_t DSP_fast_magnitude(int32_t re, int32_t im) {
    if (re < 0) re = -re;
    if (im < 0) im = -im;
    
    int32_t max_val, min_val;
    if (re > im) {
        max_val = re;
        min_val = im;
    } else {
        max_val = im;
        min_val = re;
    }
    
    // Approximation: max + (3/8)*min ≈ max + 0.375*min
    // Using shift: min*3/8 = (min >> 2) + (min >> 3)
    return max_val + (min_val >> 2) + (min_val >> 3);
}

/*
// Fast atan2 approximation returning degrees (-180 to +180)
// Uses CORDIC-style polynomial approximation
// Max error ~0.3 degrees
int16_t DSP_fast_atan2_deg(int32_t im, int32_t re) {
    int16_t angle;
    int32_t abs_re = (re < 0) ? -re : re;
    int32_t abs_im = (im < 0) ? -im : im;
    
    // Handle zero case
    if (re == 0 && im == 0) return 0;
    
    // Compute atan(y/x) for |y| <= |x| using approximation:
    // atan(z) ≈ z * 45 degrees (for small z, scaled)
    // More accurate: atan(z) ≈ z * 45 * (1 - 0.28 * z^2) degrees
    
    int32_t ratio;
    int16_t base_angle;
    
    if (abs_re >= abs_im) {
        // |angle| <= 45 degrees from x-axis
        // ratio = im/re scaled by 1024
        ratio = (im * 1024) / re;
        // angle ≈ ratio * 45 / 1024 = ratio * 45 >> 10
        angle = (int16_t)((ratio * 45) >> 10);
        
        // Adjust for quadrant
        if (re < 0) {
            angle = (im >= 0) ? (180 + angle) : (-180 + angle);
        }
    } else {
        // |angle| > 45 degrees from x-axis
        // Use atan(y/x) = 90 - atan(x/y) for y > 0
        ratio = (re * 1024) / im;
        base_angle = (int16_t)((ratio * 45) >> 10);
        
        if (im > 0) {
            angle = 90 - base_angle;
        } else {
            angle = -90 - base_angle;
        }
    }
    
    return angle;
}
*/

// True atan2
int16_t DSP_true_atan2_deg(int32_t im, int32_t re){
    double phase = atan2((double(im),(double)re));
    return (int16_t)(phase*RAD2DEG); // RAD2DEG is 180/pi approximated to 57.29577951308232, defined in top of this document to save CPU power by pre-calculating the division. 
}


// Fast atan2
int16_t DSP_fast_atan2_deg(int32_t im, int32_t re) {
    if (re == 0 && im == 0) return 0;
    
    // atan2 returns radians, convert to degrees
    double radians = atan2((double)im, (double)re);
    double degrees = radians * (180.0 / M_PI);
    
    return (int16_t)degrees;
}