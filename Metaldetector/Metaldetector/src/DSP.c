#include <avr/io.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "DSP.h"

// Hanning window LUT
static inline int16_t apply_hanning(int16_t sample, uint8_t n){
    int16_t w = pgm_read_word(&hanning[n]); // læs fra PROGMEM
    int32_t temp = (int32_t)sample * w;
    return (int16_t)(temp >> 15);
}

static inline int16_t IIR_filter(int16_t x, int16_t y_prev){
    const int16_t a = 9830; //0.3*32768
    int32_t temp = (32768-a) * y_prev + (int32_t)a * x;
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