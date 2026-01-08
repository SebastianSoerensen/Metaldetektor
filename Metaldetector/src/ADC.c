#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "DFT.h"

// Two N-sample buffers, double buffer
volatile uint16_t adc_buf[2][ADC_BLOCK_N];
volatile uint8_t adc_buf_full[2] = {0, 0};

static volatile uint8_t adc_cur_buf = 0;    // 0 or 1
static volatile uint16_t adc_cur_idx = 0;    // 0 - N-1

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
    OCR1A = (F_CPU / (8UL * FS_HZ)) - 1; // 249 for 8 kHz

    TIMSK1 = (1 << OCIE1A); // enable ISR
    TCCR1B |= (1 << CS11); // prescaler = 8, start
}

static uint8_t coil_div = 0; // coil divide flag to get 2 kHz

ISR(TIMER1_COMPA_vect)
{
    if (!sampling_enabled) 
    return;

    // Tracks every 2th tick, gives 4000 toggles / 2 = 2000kHz (2 toggles per period)
    coil_div++;
    if (coil_div == 2) {
        coil_div = 0;
    }
    if (coil_div == 0) {
        COIL_PORT ^= (1 << COIL_PIN);
    }

    // start ADC conversion
    ADCSRA |= (1 << ADSC);
    
}
void adc_init(void)
{
    ADMUX = (1 << REFS0); // AVcc, ADC0
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1); // presc 64, 250 kHz (Skal muligvis justeres til 125 hvis sampling er upræcis)
    // ADATE = 0, we trigger manually from timer ISR

    DIDR0 |= (1<<ADC0D); // digital ADC pin disable (lessens noise)
}

ISR(ADC_vect)
{
    uint16_t s = ADC; // read sample
    int16_t sample = (int16_t)s - 512;

    uint8_t buf = adc_cur_buf; // current buffer
    uint16_t i = adc_cur_idx;

    // Start of a new ADC_BLOCK_N-sample block
    if (i == 0) {
        DFT_reset();
    }

    DFT_accum(sample, i);

    adc_buf[buf][i] = s;
    i++;

    if (i >= ADC_BLOCK_N) {
        // latch the finished block DFT result here
        dft_latched[buf] = DFT_get();
        dft_ready[buf] = 1;

        adc_buf_full[buf] = 1; // signal to main

        buf ^= 1; // swap buffer
        i = 0;
    }

    adc_cur_buf = buf;
    adc_cur_idx = i;
}


