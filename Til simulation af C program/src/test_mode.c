#include "test_mode.h"
#include "config.h"
#include "DFT.h"
#include "DSP.h"
#include "test_vectors.h"
#include <avr/pgmspace.h>
#include "I2C.h"
#include "ssd1306.h"

volatile test_signal_t current_test_signal = TEST_NONE;

/*
 * Dynamisk testsignal (kan beholdes til demo),
 * men DFT-verifikation bruger test_vectors[].
 */

#define TEST_AMPLITUDE 200

int16_t test_get_sample(uint16_t sample_index) {
    uint8_t phase_in_period;
    int16_t sample = 0;
    
    switch (current_test_signal) {
        case TEST_NONE:
            phase_in_period = sample_index & 3;
            switch (phase_in_period) {
                case 0: sample = 0; break;
                case 1: sample = TEST_AMPLITUDE; break;
                case 2: sample = 0; break;
                case 3: sample = -TEST_AMPLITUDE; break;
            }
            break;
            
        case TEST_FERROUS:
            phase_in_period = (sample_index + 1) & 3;
            switch (phase_in_period) {
                case 0: sample = 0; break;
                case 1: sample = TEST_AMPLITUDE + 50; break;
                case 2: sample = 0; break;
                case 3: sample = -(TEST_AMPLITUDE + 50); break;
            }
            break;
            
        case TEST_NON_FERROUS:
            phase_in_period = (sample_index + 3) & 3;
            switch (phase_in_period) {
                case 0: sample = 0; break;
                case 1: sample = TEST_AMPLITUDE + 50; break;
                case 2: sample = 0; break;
                case 3: sample = -(TEST_AMPLITUDE + 50); break;
            }
            break;
            
        default:
            sample = 0;
            break;
    }
    
    return sample;
}

void test_next_signal(void) {
    current_test_signal = (current_test_signal + 1) % TEST_COUNT;
}

const char* test_get_signal_name(void) {
    switch (current_test_signal) {
        case TEST_NONE:        return "BASELINE";
        case TEST_FERROUS:     return "FERROUS ";
        case TEST_NON_FERROUS: return "NON-FER ";
        default:               return "UNKNOWN ";
    }
}

/*
 * Run DFT on Python-generated test vector
 * and verify against reference values.
 */
void run_dft_test(void)
{
    char buf[16];

    DFT_reset();

    for (uint16_t n = 0; n < ADC_BLOCK_N; n++) {
        int16_t s = pgm_read_word(&test_samples[n]);
        DFT_accum(s, n);
    }

    z_struct z = DFT_get();

    int32_t re = z.re / ADC_BLOCK_N;
    int32_t im = z.im / ADC_BLOCK_N;
    int32_t amp = DSP_fast_magnitude(re, im);
    int16_t phase = DSP_fast_atan2_deg(im, re);

    clear_display();

    // Vis beregnede værdier
    sendStrXY("RE:", 0, 0);
    snprintf(buf, sizeof(buf), "%ld   ", re);
    sendStrXY(buf, 40, 6);

    sendStrXY("IM:", 1, 0);
    snprintf(buf, sizeof(buf), "%ld   ", im);
    sendStrXY(buf, 40, 6);

    sendStrXY("AMP:", 2, 0);
    snprintf(buf, sizeof(buf), "%ld   ", amp);
    sendStrXY(buf, 40, 6);

    sendStrXY("PHS:", 3, 0);
    snprintf(buf, sizeof(buf), "%d   ", phase);
    sendStrXY(buf, 40, 6);

    // Sammenlign med reference
    if (re == TEST_REF_RE && im == TEST_REF_IM) {
        sendStrXY("DFT OK", 5, 0);
    } else {
        sendStrXY("DFT FAIL", 5, 0);
    }
}
