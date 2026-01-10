#include <avr/io.h>
#include <stdint.h>

#define F_CPU 16000000UL
#define LOW_THRESHOLD 50

void buzzer_init(void)
{
    // OC2A = PB4 on ATmega2560 (Arduino Mega pin 10)
    DDRB |= (1 << PB4);

    // Timer2: CTC mode
    TCCR2A = (1 << WGM21);   // CTC
    TCCR2B = 0;             // Stop timer for now

    buzzer_off();
}

void buzzer_set_frequency(uint16_t freq)
{
    if (freq == 0) return;

    // Use prescaler 64
    // f_out = F_CPU / (2 * prescaler * (1 + OCR2A))
    uint32_t top = (F_CPU / (2UL * 64UL * freq)) - 1;

    if (top > 255) top = 255;
    OCR2A = (uint8_t)top;
}

void buzzer_on(void)
{
    // Toggle OC2A on compare match
    TCCR2A |= (1 << COM2A0);
    TCCR2B = (1 << CS22);   // Prescaler = 64
}

void buzzer_off(void)
{
    TCCR2A &= ~(1 << COM2A0); // Disable toggle
    TCCR2B = 0;              // Stop timer
}

void update_buzzer(uint16_t amp, int16_t phase)
{
    if (amp < LOW_THRESHOLD)
    {
        buzzer_off();
        return;
    }

    buzzer_on();

    if (phase > 200)           // Non-iron
        buzzer_set_frequency(1200 + amp/2);
    else if (phase < -200)     // Iron
        buzzer_set_frequency(300 + amp/4);
    else
        buzzer_set_frequency(700);
}