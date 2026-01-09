#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "config.h"
#include "DFT.h"

// Double buffer - each with ADC_BLOCK_N samples
extern volatile uint16_t adc_buf[2][ADC_BLOCK_N];
extern volatile uint8_t  adc_buf_full[2];

// Sampling on/off flag (used in timer ISR)
extern volatile uint8_t sampling_enabled;

// Configure Timer1 for 8 kHz interrupts and 2 kHz coil drive
void timer1_init_8kHz(void);

// Configure ADC (single-conversion mode, interrupt enabled)
void adc_init(void);

#endif