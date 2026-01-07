#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"

// Two N-sample buffers, double buffer
volatile uint16_t adc_buf[2][ADC_BLOCK_N];
volatile uint8_t  adc_buf_full[2] = {0, 0};

static volatile uint8_t  adc_cur_buf  = 0;    // 0 or 1
static volatile uint16_t adc_cur_idx  = 0;    // 0 - N-1

volatile uint8_t sampling_enabled = 1;

// set coil pins
#define COIL_DDR DDRB
#define COIL_PORT PORTB
#define COIL_PIN PB5

void timer1_init_8kHz(void)
{
    COIL_DDR |= (1 << COIL_PIN);

    TCCR1A = 0;
    TCCR1B = (1 << WGM12); // CTC 
    OCR1A  = (F_CPU / (8UL * FS_HZ)) - 1; // 249 for 8 kHz

    TIMSK1 = (1 << OCIE1A); // enable ISR
    TCCR1B |= (1 << CS11); // prescaler = 8
}

static uint8_t coil_div = 0; // coil divide flag to get 2 kHz

ISR(TIMER1_COMPA_vect)
{
    if (!sampling_enabled) 
    return;

    // Tracks every 4th tick
    coil_div++;
    if (coil_div == 4) {
        coil_div = 0;
    }

    // 2 kHz coil (toggle every 4th 8 kHz tick) 
    if (coil_div == 0) {
        COIL_PORT ^= (1 << COIL_PIN);
    }

    // start ADC conversion
    ADCSRA |= (1 << ADSC);
    
}
void adc_init(void)
{
    ADMUX  = (1 << REFS0); // AVcc, ADC0
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS0); // presc 32, 500 kHz         
    // ADATE = 0, we trigger manually from timer ISR
}

ISR(ADC_vect)
{
    uint16_t s = ADC; // read sample

    uint8_t  buf = adc_cur_buf; // current buffer
    uint16_t i   = adc_cur_idx;

    adc_buf[buf][i] = s;
    i++;

    if (i >= ADC_BLOCK_N) {
        // current buffer full
        adc_buf_full[buf] = 1; // signal to main

        // switch to other buffer (xor)
        buf ^= 1; // 0-1, 1-0
        i = 0;
    }

    adc_cur_buf = buf;
    adc_cur_idx = i;
}


