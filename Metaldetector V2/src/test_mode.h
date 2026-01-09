#ifndef TEST_MODE_H
#define TEST_MODE_H

#include <stdint.h>
#include <stdbool.h>

// Enable this to bypass real ADC and inject test signals
#define TEST_MODE_ENABLED 0

// Test signal types
typedef enum {
    TEST_NONE = 0,      // No metal (baseline)
    TEST_FERROUS,       // Simulated iron/steel (positive phase shift)
    TEST_NON_FERROUS,   // Simulated copper/aluminum (negative phase shift)
    TEST_COUNT
} test_signal_t;

// Current test signal (change with button in test mode)
extern volatile test_signal_t current_test_signal;

// Call this instead of real ADC reading in test mode
int16_t test_get_sample(uint16_t sample_index);

// Cycle to next test signal
void test_next_signal(void);

// Get name of current test signal for display
const char* test_get_signal_name(void);

#endif