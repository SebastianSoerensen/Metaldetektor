#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "DFT.h"
#include "test_mode.h"

// Two N-sample buffers, double buffer
volatile uint16_t adc_buf[2][ADC_BLOCK_N];
volatile uint8_t adc_buf_full[2] = {0, 0};

// DFT results - DEFINED here (extern in DFT.h)
volatile z_struct dft_latched[2];
volatile uint8_t  dft_ready[2] = {0, 0};

static volatile uint8_t adc_cur_buf = 0;    // 0 or 1
static volatile uint16_t adc_cur_idx = 0;   // 0 - N-1

volatile uint8_t sampling_enabled = 1;

// Coil pin setup
#define COIL_DDR  DDRB
#define COIL_PORT PORTB
#define COIL_PIN  PB5

void timer1_init_8kHz(void)
{
    COIL_DDR |= (1 << COIL_PIN);

    TCCR1A = 0;
    TCCR1B = (1 << WGM12); // CTC mode
    OCR1A = (F_CPU / (8UL * FS_HZ)) - 1; // 249 for 8 kHz

    TIMSK1 = (1 << OCIE1A); // enable compare match ISR
    TCCR1B |= (1 << CS11);  // prescaler = 8, start timer
}

static uint8_t coil_div = 0; // divider to get 2 kHz from 8 kHz

ISR(TIMER1_COMPA_vect)
{
    if (!sampling_enabled)
        return;

    // Toggle coil every 2nd tick: 8000/2 = 4000 toggles/s = 2000 Hz
    if (++coil_div >= 2) {
        coil_div = 0;
        COIL_PORT ^= (1 << COIL_PIN);
    }

    // Start ADC conversion
    ADCSRA |= (1 << ADSC);
}

void adc_init(void)
{
    ADMUX = (1 << REFS0); // AVcc reference, ADC0
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1); // prescaler 64 -> 250 kHz ADC clock
    DIDR0 |= (1 << ADC0D); // disable digital input on ADC0 pin
}

ISR(ADC_vect)
{
    int16_t sample;
    uint16_t s;
    
#if TEST_MODE_ENABLED
    // Use simulated test signal instead of real ADC
    sample = test_get_sample(adc_cur_idx);
    s = (uint16_t)(sample + 512);  // convert back to 0-1023 for buffer
#else
    // Real ADC reading
    s = ADC;
    sample = (int16_t)s - 512; // center around 0
#endif

    uint8_t  buf = adc_cur_buf;
    uint16_t i   = adc_cur_idx;

    // Reset DFT at start of each block
    if (i == 0) {
        DFT_reset();
    }

    DFT_accum(sample, i);

    // Store raw sample
    adc_buf[buf][i] = s;

    i++;

    if (i >= ADC_BLOCK_N) {
        // Latch completed DFT result
        dft_latched[buf] = DFT_get();
        dft_ready[buf] = 1;
        adc_buf_full[buf] = 1;

        // Swap buffer
        buf ^= 1;
        i = 0;
    }

    adc_cur_buf = buf;
    adc_cur_idx = i;
}