#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

void buzzer_init(void);
void buzzer_set_frequency(uint16_t freq);
void buzzer_on(void);
void buzzer_off(void);
void update_buzzer(uint16_t amp, int16_t phase);

#endif 