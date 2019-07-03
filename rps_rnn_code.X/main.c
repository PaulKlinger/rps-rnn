/*
 * File:   main.c
 * Author: Paul
 *
 * Created on June 23, 2019, 8:41 PM
 */

#include "mcc_generated_files/config/clock_config.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/include/pin_manager.h"
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"
#include "simple_model_weights.h"

#include "simple_model.h"

// WARNING!!! I changed "*link_data_start" in
//E:\Program Files (x86)\Microchip\MPLABX\v5.10\packs\Atmel\ATtiny_DFP\1.3.238\gcc\dev\attiny1614\device-specs\specs-attiny1614
// from "-Tdata 0x803800" to "	-Tdata 0x802000"
// this prevents a linker error for static or global variables.
// But I'm very uncertain if this is actually correct.
int main(void)
{
    /* Initializes MCU, drivers and middleware */
    SYSTEM_Initialize();
    
    
    srandom(42);
    random();

    volatile rps first_move = simple_model_predict(START, 1.0);
    
    /* Replace with your application code */
    while (1){
        IO_PA4_set_level(1);
        _delay_ms(500);
        IO_PA4_set_level(0);
        _delay_ms(500);
    }
}
