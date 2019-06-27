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



struct quantized_matrix {
    uint8_t *data;
    uint8_t rows, cols;
    float scale, offset;
};

struct float_matrix {
    float *data;
    uint8_t rows, cols;
};

void mult_quant_float(struct quantized_matrix *m1,
                      struct float_matrix *m2,
                      struct float_matrix *res) {
    float res_entry;
    for (uint8_t row=0; row < m1->rows; row++) {
        for (uint8_t col=0; col < m2->cols; col++) {
            res_entry = 0;
            for (uint8_t i=0; i<m1->cols; i++) {
                // m1[row, i] * m2[i, col]
                res_entry += m1->data[row * m1->cols + i] * m2->data[i * m2->cols + col];
            }
            // res is shape (m1->rows, m2->cols)
            // res[row, col]
            res->data[row * m2->cols + col] = res_entry * m1->scale + m1->offset;
        };
    };
};

// WARNING!!! I changed "*link_data_start" in
//E:\Program Files (x86)\Microchip\MPLABX\v5.10\packs\Atmel\ATtiny_DFP\1.3.238\gcc\dev\attiny1614\device-specs\specs-attiny1614
// from "-Tdata 0x803800" to "	-Tdata 0x802000"
// this prevents a linker error for static or global variables.
// But I'm very uncertain if this is actually correct.
int main(void)
{
    /* Initializes MCU, drivers and middleware */
    SYSTEM_Initialize();
    
    struct quantized_matrix *m1 = malloc(sizeof(struct quantized_matrix));
    m1->rows = 2;
    m1->cols = 2;
    m1->scale = 2.5;
    m1->offset = -1;
    m1->data = malloc(4);
    m1->data[0] = 1;
    m1->data[1] = 2;
    m1->data[2] = 3;
    m1->data[3] = 4;
    
    struct float_matrix *m2 = malloc(sizeof(struct float_matrix));
    m2->rows = 2;
    m2->cols = 2;
    m2->data = malloc(4 * sizeof(float));
    m2->data[0] = 1;
    m2->data[1] = 2;
    m2->data[2] = 2;
    m2->data[3] = 3;
    
    struct float_matrix *m3 = malloc(sizeof(struct float_matrix));
    m3->data = malloc(4 * sizeof(float));
    
    mult_quant_float(m1,m2,m3);
    
    volatile float a = m3->data[0];
    volatile float b = m3->data[1];
    volatile float c = m3->data[2];
    volatile float d = m3->data[3];
    
    
    
    /* Replace with your application code */
    while (1){
        IO_PA4_set_level(1);
        _delay_ms(500);
        IO_PA4_set_level(0);
        _delay_ms(500);
    }
}
