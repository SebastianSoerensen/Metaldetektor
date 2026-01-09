#include "test_mode.h"
#include "config.h"

volatile test_signal_t current_test_signal = TEST_NONE;

// Simulated 2 kHz signals with different phase shifts
// At 8 kHz sample rate, 2 kHz = 4 samples per period
// Phase shift is achieved by offsetting the waveform

// Amplitude of test signal (arbitrary units, centered around 0)
#define TEST_AMPLITUDE 200

int16_t test_get_sample(uint16_t sample_index) {
    // Base 2 kHz sine wave: period = 4 samples at 8 kHz
    // Values for one period: {0, +A, 0, -A} approximates sine
    
    uint8_t phase_in_period;
    int16_t sample = 0;
    
    switch (current_test_signal) {
        case TEST_NONE:
            // Baseline signal - clean 2 kHz, no shift
            // sin(2π * 2000 * n / 8000) = sin(πn/2)
            phase_in_period = sample_index & 3;  // mod 4
            switch (phase_in_period) {
                case 0: sample = 0; break;
                case 1: sample = TEST_AMPLITUDE; break;
                case 2: sample = 0; break;
                case 3: sample = -TEST_AMPLITUDE; break;
            }
            break;
            
        case TEST_FERROUS:
            // Ferrous metal: larger amplitude + positive phase shift
            // Shift by +1 sample ≈ +90 degrees (exaggerated for visibility)
            phase_in_period = (sample_index + 1) & 3;  // shift +1
            switch (phase_in_period) {
                case 0: sample = 0; break;
                case 1: sample = TEST_AMPLITUDE + 50; break;  // stronger
                case 2: sample = 0; break;
                case 3: sample = -(TEST_AMPLITUDE + 50); break;
            }
            break;
            
        case TEST_NON_FERROUS:
            // Non-ferrous metal: larger amplitude + negative phase shift
            // Shift by -1 sample ≈ -90 degrees (exaggerated for visibility)
            phase_in_period = (sample_index + 3) & 3;  // shift -1 (same as +3 mod 4)
            switch (phase_in_period) {
                case 0: sample = 0; break;
                case 1: sample = TEST_AMPLITUDE + 50; break;  // stronger
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