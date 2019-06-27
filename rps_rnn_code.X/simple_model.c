#include <stdint.h>
#include "simple_model.h"
#include "matrix.h"
#include "simple_model_weights.h"

rps our_last_move = START;

float input_vector_data[6];
struct float_matrix input_vector = {
    .data = input_vector_data,
    .rows = 6,
    .cols = 1,
};

float state_vector_data[10];
struct float_matrix state_vector = {
    .data = state_vector_data,
    .rows = 10,
    .cols = 1,
};

float intermediate_state_vector_data[10];
struct float_matrix intermediate_state_vector = {
    .data = intermediate_state_vector_data,
    .rows = 10,
    .cols = 1,
};

float output_vector_data[3];
struct float_matrix output_vector = {
    .data = output_vector_data,
    .rows = 3,
    .cols = 1,
};

void set_new_input_vector(rps opponent_move) {
    for (uint8_t i=0; i<6; i++){
            input_vector_data[i] = 0;
    };
    if (opponent_move != START) {
        input_vector_data[our_last_move] = 1;
        input_vector_data[3 + opponent_move] = 1;
    };
}

rps simple_model_predict(rps opponent_move) {
    set_new_input_vector(opponent_move);
    mult_float_quant(&input_vector, &W_input, &intermediate_state_vector);
    add_float_float(&intermediate_state_vector, &b_input, &intermediate_state_vector);
   
    
    return opponent_move;
    
};