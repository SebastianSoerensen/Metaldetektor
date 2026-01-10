#include <avr/io.h>
#include <stdint.h>

#define F_CPU 16000000UL
#define LOW_THRESHOLD 50

// OC2B = PH6 (Arduino Mega pin 9)
// OC2A as TOP (frequency)

void buzzer_init(void)
{
    // PH6 (pin 9) as output
    DDRH |= (1 << PH6);

    // Timer2: Fast PWM, TOP = OCR2A
    // WGM22:0 = 7 -> Fast PWM, OCR2A as TOP
    TCCR2A = (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << WGM22);

    // Non-inverting PWM OC2B (clear on compare match)
    TCCR2A |= (1 << COM2B1);

    // Stop timer until sound is wanted
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));

    // Initially mute the buzzer
    OCR2B = 0;
}

// Tone frequency (Hz)
// f_out = F_CPU / (prescaler * (1 + OCR2A))
void buzzer_set_frequency(uint16_t freq)
{
    if (freq == 0) return;

    // Prescaler = 64
    uint32_t top = (F_CPU / (64UL * freq)) - 1;
    if (top > 255) top = 255;

    OCR2A = (uint8_t)top;
}

// Set volume by adjusting the duty cycle (0-255) 
void buzzer_set_volume(uint8_t vol)
{
    OCR2B = vol;   // 0 = mute, 255 = max
}

void buzzer_on(void)
{
    // Start timer with prescale = 64
    TCCR2B = (TCCR2B & ~((1 << CS22) | (1 << CS21) | (1 << CS20))) | (1 << CS22);
}

void buzzer_off(void)
{
    // Stop timer
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    OCR2B = 0;
}

// Amplitude -> volumen, Fase -> tone
void update_buzzer(uint16_t amp, int16_t phase)
{
    if (amp < LOW_THRESHOLD) // To stop the buzzer from making sound below the threshold
    {
        buzzer_off();
        return;
    }

    buzzer_on();
    // We represent different phases with different frequencies (tones) for the buzzer. 
    if (phase > 30)               // Non-iron
        buzzer_set_frequency(1200);
    else if (phase < -30)         // Iron
        buzzer_set_frequency(300);
    else
        buzzer_set_frequency(700);

    // Volume of the buzzer to be adjusted by the amplitude of DFT
    const uint16_t AMP_MAX = 1000;   // This is the highest amplitude we want to represent with audio 
    if (amp > AMP_MAX) amp = AMP_MAX;

    uint8_t volume = (uint32_t)amp * 255 / AMP_MAX;
    buzzer_set_volume(volume);
}
