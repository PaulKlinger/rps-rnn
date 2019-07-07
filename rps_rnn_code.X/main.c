/*
 * File:   main.c
 * Author: Paul
 *
 * Created on June 23, 2019, 8:41 PM
 */

#include <avr/io.h>
#include "mcc_generated_files/config/clock_config.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/include/pin_manager.h"
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"
#include "model_weights.h"

#include "model.h"

// WARNING!!! I changed "*link_data_start" in
//E:\Program Files (x86)\Microchip\MPLABX\v5.10\packs\Atmel\ATtiny_DFP\1.3.238\gcc\dev\attiny1614\device-specs\specs-attiny1614
// from "-Tdata 0x803800" to "	-Tdata 0x802000"
// this prevents a linker error for static or global variables.
// But I'm somewhat uncertain if this is actually correct.


#define TEMPERATURE 0.001

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

rps get_selected_move() {
    if (rock_pressed()) {
        return ROCK;
    } else if (paper_pressed()) {
        return PAPER;
    } else if (scissors_pressed()) {
        return SCISSORS;
    } else {
        return START;
    }
}

void play_against_rps_rnn() {
    // human player plays against device
    // pressing start starts a countdown until device move is shown
    // then player move needs to be entered before start can be pressed again
    
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
            our_move = model_predict(opponent_move, TEMPERATURE);
            blink_all(3, 200);
            display_move(our_move);
            opponent_move_entered = false;
        } else {
            rps selected_move = get_selected_move();
            if (selected_move != START) {
                opponent_move = selected_move;
                opponent_move_entered = true;
            }
        }
        _delay_ms(10);
    }
}

void play_with_rps_rnn() {
    // use device to suggest moves against human opponent
    // entering opponents move immediately suggests next move
    
    // Wait until start button is released
    // This gives some randomness to seed the RNG
    while (start_pressed()){};
    // seed the RNG with the current time (from RTC counter)
    srandom(RTC.CNT);
    
    display_move(model_predict(START, TEMPERATURE));
    rps selected_move;
    while (1) {
        // need to save selected move, otherwise it could change between
        // condition evaluation and prediction
        selected_move = get_selected_move();
        if (selected_move != START) {
            // blink the LED so it's obviously a new move even if the
            // new suggestion is the same as the old one
            set_all_leds(0);
            _delay_ms(100);
            display_move(model_predict(selected_move, TEMPERATURE));
            // wait until button is released + a bit for bouncing
            while (get_selected_move() != START){}
            _delay_ms(100);
        }
        _delay_ms(10);
    }
}

int main(void)
{
    /* Initializes MCU, drivers and middleware */
    SYSTEM_Initialize();
    // wait a bit to avoid spurious input on startup
    _delay_ms(100);
    // choose device mode depending on whether start is pressed
    if (start_pressed()) {
        play_with_rps_rnn();
    } else {
        play_against_rps_rnn();
    }
}
