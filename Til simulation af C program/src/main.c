#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdio.h>
#include <util/delay.h>
#include "config.h"
#include "ADC.h"
#include "DSP.h"
#include "DFT.h"
#include "button.h"
#include "test_mode.h"
#include "ssd1306.h"
#include "I2C.h"

// Result structure
typedef struct {
    int32_t amp;
    int16_t phase_deg;  // -180 to +180 degrees
    int32_t re;
    int32_t im;
} results_t;

// Process DFT result from buffer b
results_t calc_results(uint8_t b) {
    results_t res;
    z_struct z = dft_latched[b];
    
    // Normalize by block size
    res.re = z.re / ADC_BLOCK_N;
    res.im = z.im / ADC_BLOCK_N;

    // True magnitude with sqrt
    res.amp = DSP_true_magnitude(res.re,res.im);

    // Fast magnitude (no sqrt)
//    res.amp = DSP_fast_magnitude(res.re, res.im);
    
    // Fast phase in degrees
    res.phase_deg = DSP_fast_atan2_deg(res.im, res.re);
    
    return res;
}

typedef enum {
    STATE_IDLE,
    STATE_CALIBRATING,
    STATE_RUNNING
} system_state_t;

int main(void)
{
    system_state_t state = STATE_CALIBRATING;
    
    int32_t baseline_amp = 0;
    int16_t baseline_phase = 0;
    bool system_calibrated = false;
    results_t res = {0, 0, 0, 0};
    
    // Display buffer for numbers
    char str_buf[16];

    // Initialize peripherals
    cli();
    timer1_init_8kHz();
    adc_init();
    buttons_init();
    I2C_Init();
    sei();

    InitializeDisplay();

    while (1)
    {
        // Handle button presses (clear flags after reading)
        if (btn_run_pressed) {
            btn_run_pressed = false;
            // RUN always toggles between IDLE and RUNNING
            if (state == STATE_IDLE && system_calibrated) {
                clear_display();
                state = STATE_RUNNING;
            } else if (state == STATE_RUNNING) {
                clear_display();
                state = STATE_IDLE;
            }
        }
        
        if (btn_pwr_pressed) {
            btn_pwr_pressed = false;
#if TEST_MODE_ENABLED
            // In test mode: cycle through test signals (while running)
            if (state == STATE_RUNNING) {
                test_next_signal();
            } else {
                // Recalibrate if not running
                clear_display();
                system_calibrated = false;
                state = STATE_CALIBRATING;
            }
#else
            // Normal mode: always recalibrate
            clear_display();
            system_calibrated = false;
            state = STATE_CALIBRATING;
#endif
        }

        // Check both DFT buffers for ready data
        for (uint8_t b = 0; b < 2; b++) {
            if (dft_ready[b]) {
                cli();
                dft_ready[b] = 0;
                sei();
                res = calc_results(b);
            }
        }
        
        // Always tick debounce (runs every loop iteration)
        buttons_debounce_tick();

        switch (state)
        {
        case STATE_CALIBRATING:
            sampling_enabled = 1;
            sendStrXY("Calibrating...", 0, 0);
            
            // Wait for first valid result
            if (res.amp > 0 && !system_calibrated) {
                baseline_amp = res.amp;
                baseline_phase = res.phase_deg;
                system_calibrated = true;
                clear_display();
                state = STATE_IDLE;
            }
            break;

        case STATE_IDLE:
            sampling_enabled = 0;
            
            sendStrXY("IDLE", 0, 0);
            sendStrXY("RUN: Start", 2, 0);
            sendStrXY("PWR: Recal", 3, 0);
            break;

        case STATE_RUNNING:
            sampling_enabled = 1;
            
            #if TEST_MODE_ENABLED
                // Vis rå test-resultater direkte
                sendStrXY("AMP:", 0, 0);
                snprintf(str_buf, sizeof(str_buf), "%ld   ", res.amp);
                sendStrXY(str_buf, 40, 6);

                sendStrXY("PHS:", 1, 0);
                snprintf(str_buf, sizeof(str_buf), "%d deg   ", res.phase_deg);
                sendStrXY(str_buf, 40, 6);
            #else
                // Din normale drift (baseline-delta)
                int32_t delta_amp = res.amp - baseline_amp;
                int16_t delta_phase = res.phase_deg - baseline_phase;

                if (delta_phase > 180) delta_phase -= 360;
                if (delta_phase < -180) delta_phase += 360;

                sendStrXY("AMP:", 0, 0);
                snprintf(str_buf, sizeof(str_buf), "%ld   ", delta_amp);
                sendStrXY(str_buf, 40, 6);

                sendStrXY("PHS:", 1, 0);
                snprintf(str_buf, sizeof(str_buf), "%d deg   ", delta_phase);
                sendStrXY(str_buf, 40, 6);
            #endif
                break;

        default:
            state = STATE_CALIBRATING;
            break;
        }
    }
}