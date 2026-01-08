#include <stdio.h>
#include <avr/io.h>
#include "config.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "ssd1306.h"
#include "I2C.h"
#include "ADC.h"
#include "DSP.h"
#include "DFT.h"
#include "math.h"

extern volatile uint16_t adc_buf[2][ADC_BLOCK_N];
extern volatile uint8_t  adc_buf_full[2];
extern volatile z_struct dft_latched[2];
extern volatile uint8_t  dft_ready[2];

typedef struct {
    int32_t amp;
    int32_t phase;
}results;

void calc_data(int16_t re,int16_t im){
    for(int b = 0 ; b < 2; b++){
        if(dft_ready[b]){
            z_struct z;
            results res;
            cli();
            z = dft_latched[b];
            dft_ready[b] = 0;
            sei();
            int32_t re = dft_latched[b].re / ADC_BLOCK_N;
            int32_t im = dft_latched[b].im / ADC_BLOCK_N;
            res.amp = sqrt(re*re+im*im);
            res.phase = atan2(im,re);
        }  
    }
}

typedef enum {
    STATE_IDLE = 0, // not sampling
    STATE_CALIBRATING, // zero ref (set no metal reference when starting)
    STATE_RUNNING // running
} system_state;

int main(void)
{
    system_state state = STATE_CALIBRATING;

    cli();
    timer1_init_8kHz();
    adc_init();
    sei();

    int32_t baseline_amp = 0; // Calibrating point
    bool SYSTEM_CALIBRATED = false;
    bool RUN = false; // For powering the coils
    results res;
    z_struct z;

    InitializeDisplay(); // display on

    while (1)
    {
        switch (state)
        {
        case STATE_CALIBRATING:
            sampling_enabled = 1;

            // ---- PLACE: read finished block DFT here ----
            if(SYSTEM_CALIBRATED == false){

            
            calc_data(z.re,z.im);
            baseline_amp = res.amp;
            SYSTEM_CALIBRATED = true;
            }
            
            state = STATE_IDLE;

        case STATE_IDLE:
            sampling_enabled = 0;
            // OLED UI idle text
            sendStrXY("V. Run",1,1);
            sendStrXY("H. Power",1,2);

            DFT_reset();
            // TODO: update POWER from button (poll or ISR)
            if (SYSTEM_CALIBRATED && RUN) {
                state = STATE_RUNNING;
            } else {
                state = STATE_IDLE;
            }
            break;

        case STATE_RUNNING:
            sampling_enabled = 1;

            // ---- PLACE: read finished block DFT here ----
            calc_data(z.re,z.im);
            int32_t delta_amp = res.amp - baseline_amp;
            
                    // TODO: threshold delta -> “metal detected”
                    // TODO: update OLED with delta / status
                

            if (!RUN) {
                state = STATE_IDLE;
            }
            else {
            state = STATE_RUNNING;
            }
            break;

        default:
            state = STATE_CALIBRATING;
            break;
        }
    }
}



/*
for (uint8_t b = 0; b < 2; b++) {
                if (dft_ready[b]) {
                    z_struct z;

                    cli();
                    z = dft_latched[b];
                    dft_ready[b] = 0;
                    sei();

                    // compute magnitude metric (example: mag^2)
                    int32_t re = z.re / ADC_BLOCK_N;
                    int32_t im = z.im / ADC_BLOCK_N;
                    int32_t mag = re*re + im*im;
                    */