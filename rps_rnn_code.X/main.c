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


//LED pins:
// rock: PB0
// paper: PB1
// scissors: PB2

// button pins:
// rock: P4
// paper: PA3
// scissors: PA1
// start: PA2
static inline bool start_pressed() {return !IO_PA2_get_level();}
static inline bool rock_pressed() {return !IO_PA4_get_level();}
static inline bool paper_pressed() {return !IO_PA3_get_level();}
static inline bool scissors_pressed() {return !IO_PA1_get_level();}

static inline void set_led_rock(bool new_state) {IO_PB0_set_level(new_state);}
static inline void set_led_paper(bool new_state) {IO_PB1_set_level(new_state);}
static inline void set_led_scissors(bool new_state) {IO_PB2_set_level(new_state);}

static void set_all_leds(bool new_state) {
    set_led_rock(new_state);
    set_led_paper(new_state);
    set_led_scissors(new_state);
}

static inline void blink_all(uint8_t times, uint16_t dwell_ms) {
    for (; times > 0;times--){
        set_all_leds(1);
        _delay_ms(dwell_ms);
        set_all_leds(0);
        _delay_ms(dwell_ms);
    }
}

static void display_move(rps move) {
    set_all_leds(false);
    switch (move) {
        case ROCK:
            set_led_rock(true);
            break;
        case PAPER:
            set_led_paper(true);
            break;
        case SCISSORS:
            set_led_scissors(true);
            break;
        case START:
            break;
    }
}

int main(void)
{
    /* Initializes MCU, drivers and middleware */
    SYSTEM_Initialize();
    
    blink_all(2, 100);
    
    rps our_move = START;
    rps opponent_move = START;
    bool opponent_move_entered = true;
    
    while (1){
        if (start_pressed() && opponent_move_entered) {
            if (opponent_move == START) {
                // if this is the first move seed the RNG
                // with the current time (from RTC counter)
                srandom(RTC.CNT);
                random();
            }
            our_move = simple_model_predict(opponent_move, 0.1);
            blink_all(3, 200);
            display_move(our_move);
            opponent_move_entered = false;
        } else if (rock_pressed()) {
            opponent_move = ROCK;
            opponent_move_entered = true;
        } else if (paper_pressed()) {
            opponent_move = PAPER;
            opponent_move_entered = true;
        } else if (scissors_pressed()) {
            opponent_move = SCISSORS;
            opponent_move_entered = true;
        }
        _delay_ms(10);
    }
}
