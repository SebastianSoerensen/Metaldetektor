#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "ADC.h"

extern volatile uint16_t adc_buf[2][ADC_BLOCK_N];
extern volatile uint8_t  adc_buf_full[2];

typedef enum {
    STATE_IDLE = 0, // not sampling
    STATE_CALIBRATING, // zero ref (set no metal reference when starting)
    STATE_RUNNING // running
} system_state;


int main(void)
{
    system_state state = STATE_IDLE; // enum struct

    // Function calls
    cli();
    timer1_init_8kHz();
    adc_init();
    sei();

    switch (state)
    {
    case STATE_IDLE:
        sampling_enabled = 0;
        // ui logic for oled should be handled here
        break;

    case STATE_CALIBRATING:
        // Logic for zero ref should be handled here
        state = STATE_RUNNING;

    case STATE_RUNNING:
        sampling_enabled = 1;
        // check buffers

        // dsp for samples

        // update display
    
        break;

    default:
        state = STATE_IDLE;
        break;
    }    
}