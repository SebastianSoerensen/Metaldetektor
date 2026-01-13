#include <avr/sleep.h>
#include "sleep.h"
void go_to_sleep(void){
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_cpu();
    // The CPU is sleeping at this point, only to be waken up by interrupt. 
    // Once CPU is waken by interrupt, sleep_disable() is called, and system is running again. 
    sleep_disable();
}