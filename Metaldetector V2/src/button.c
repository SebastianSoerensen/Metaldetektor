#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "button.h"
#include "config.h"

// Button pins on PORTE (INT4 and INT5 for Mega 2560)
// INT4 = PE4 = Arduino Pin 2
// INT5 = PE5 = Arduino Pin 3
#define BTN_RUN_PIN  PE4  // INT4 - Arduino Pin 2
#define BTN_PWR_PIN  PE5  // INT5 - Arduino Pin 3

volatile bool btn_run_pressed = false;
volatile bool btn_pwr_pressed = false;

void buttons_init(void)
{
    // Set as inputs
    DDRE &= ~((1 << BTN_RUN_PIN) | (1 << BTN_PWR_PIN));
    
    // Enable internal pull-ups (buttons connect to GND)
    PORTE |= (1 << BTN_RUN_PIN) | (1 << BTN_PWR_PIN);
    
    // Configure INT4 and INT5 for falling edge (button press)
    EICRB |= (1 << ISC41) | (1 << ISC51);  // Falling edge for both
    EICRB &= ~((1 << ISC40) | (1 << ISC50));
    
    // Enable INT4 and INT5
    EIMSK |= (1 << INT4) | (1 << INT5);
}

// INT4 - Run button (Pin 2)
ISR(INT4_vect)
{
    _delay_ms(50);  // Simple debounce delay
    // Check if button still pressed (pin is LOW)
    if (!(PINE & (1 << BTN_RUN_PIN))) {
        btn_run_pressed = true;
    }
}

// INT5 - Power button (Pin 3)
ISR(INT5_vect)
{
    _delay_ms(50);  // Simple debounce delay
    // Check if button still pressed (pin is LOW)
    if (!(PINE & (1 << BTN_PWR_PIN))) {
        btn_pwr_pressed = true;
    }
}

void buttons_debounce_tick(void)
{
    // Not needed with delay-based debounce
}