#ifndef DFT_H
#define DFT_H
#include <stdint.h>
#include "config.h"

typedef struct {
    int32_t re;
    int32_t im;

} z_struct;

volatile z_struct dft_latched[2];
volatile uint8_t  dft_ready[2] = {0, 0};

z_struct DFT_accum(const int16_t sample, uint16_t n);

void DFT_reset(void);

z_struct DFT_get(void);


#endif