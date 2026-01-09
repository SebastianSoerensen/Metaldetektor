#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <stdbool.h>

// Button states (directly usable in main)
extern volatile bool btn_run_pressed;
extern volatile bool btn_pwr_pressed;

// Initialize button pins and interrupts
void buttons_init(void);

// Call periodically to handle debounce (from main loop)
void buttons_debounce_tick(void);

#endif