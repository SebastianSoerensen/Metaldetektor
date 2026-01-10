#include <avr/io.h>
#include <stdint.h>

#define F_CPU 16000000UL
#define LOW_THRESHOLD 50

// OC2B = PH6 (Arduino Mega pin 9)
// OC2A bruges som TOP (frekvens)

void buzzer_init(void)
{
    // Sæt OC2B (PH6) som output
    DDRH |= (1 << PH6);

    // Timer2: Fast PWM, TOP = OCR2A
    // WGM22:0 = 7 -> Fast PWM, OCR2A as TOP
    TCCR2A = (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << WGM22);

    // Ikke-inverterende PWM på OC2B (clear on compare match)
    TCCR2A |= (1 << COM2B1);

    // Stop timer indtil vi vil have lyd
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));

    // Start med lyd slukket
    OCR2B = 0;
}

// Sæt tonefrekvens (Hz)
// f_out = F_CPU / (prescaler * (1 + OCR2A))
void buzzer_set_frequency(uint16_t freq)
{
    if (freq == 0) return;

    // Prescaler = 64
    uint32_t top = (F_CPU / (64UL * freq)) - 1;
    if (top > 255) top = 255;

    OCR2A = (uint8_t)top;
}

// Sæt volumen via duty-cycle (0..255)
void buzzer_set_volume(uint8_t vol)
{
    OCR2B = vol;   // 0 = lydløs, 255 = max
}

void buzzer_on(void)
{
    // Start timer med prescaler = 64
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
    if (amp < LOW_THRESHOLD)
    {
        buzzer_off();
        return;
    }

    buzzer_on();

    // -------- Tone fra fase --------
    if (phase > 30)               // Non-iron
        buzzer_set_frequency(1200);
    else if (phase < -30)         // Iron
        buzzer_set_frequency(300);
    else
        buzzer_set_frequency(700);

    // -------- Volumen fra amplitude --------
    // Skaler amplitude til 0..255 (tilpas AMP_MAX til dit system)
    const uint16_t AMP_MAX = 1000;   // justér efter dine målinger
    if (amp > AMP_MAX) amp = AMP_MAX;

    uint8_t volume = (uint32_t)amp * 255 / AMP_MAX;
    buzzer_set_volume(volume);
}
