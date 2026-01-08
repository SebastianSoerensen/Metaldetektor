#include "config.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "DSP.h"

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

int16_t DSP_apply_hanning(int16_t sample, uint8_t n){
    int16_t w = pgm_read_word(&hanning[n]);
    int32_t temp = (int32_t)sample * w;
    return (int16_t)(temp >> 15);
}

int16_t DSP_IIR_filter(int16_t x, int16_t y_prev){
    const int16_t a = 9830; // 0.3 in Q15
    int32_t temp = (int32_t)(32768 - a) * y_prev + (int32_t)a * x;
    return (int16_t)(temp >> 15);
}

/* Example DSP-loop

int16_t signal[N];    // ADC samples
int16_t filtered[N];  // IIR-filter
int16_t windowed[N];  // Vindues-applikeret

int main(void) {
    int16_t y_prev = 0;

    for (uint8_t i = 0; i < N; i++){
        // Simuler ADC input
        signal[i] = i*100; // eksempelværdi

        // 1) IIR filter
        filtered[i] = IIR_filter_q15(signal[i], y_prev);
        y_prev = filtered[i];

        // 2) Hanning window
        windowed[i] = apply_hanning(filtered[i], i);
    }

    // windowed[] klar til FFT
    while(1){
        // real-time DSP loop
    }
}
*/