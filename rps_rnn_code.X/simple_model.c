#include <stdint.h>
#include "simple_model.h"
#include "matrix.h"
#include "simple_model_weights.h"

rps our_last_move = START;

float input_vector_data[6];
struct float_matrix input_vector = {
    .data = input_vector_data,
    .rows = 1,
    .cols = 6,
};

float state_vector_data[10];
struct float_matrix state_vector = {
    .data = state_vector_data,
    .rows = 1,
    .cols = 10,
};

float intermediate_state_vector_data[10];
struct float_matrix intermediate_state_vector = {
    .data = intermediate_state_vector_data,
    .rows = 1,
    .cols = 10,
};

float output_vector_data[3];
struct float_matrix output_vector = {
    .data = output_vector_data,
    .rows = 1,
    .cols = 3,
};

void set_input(rps opponent_move) {
    // construct new input vector from our and opponents move
    // if this is the first move of the game zero out state vector
    for (uint8_t i=0; i<6; i++){
            input_vector_data[i] = 0;
    };
    if (opponent_move == START) {
        for (uint8_t i=0; i<10; i++) {
            state_vector_data[i] = 0;
            intermediate_state_vector_data[i] = 0;
        }
    } else {
        input_vector_data[our_last_move] = 1;
        input_vector_data[3 + opponent_move] = 1;
    };
};

void swap_state_vectors() {
    // swap intermediate and final state vectors
    struct float_matrix temp = state_vector;
    state_vector = intermediate_state_vector;
    intermediate_state_vector = temp;
}


rps simple_model_predict(rps opponent_move, float temperature) {
    set_input(opponent_move);
    // multiply previous state vector with recurrent kernel
    m_mult_fq(&state_vector, &W_recurrent, &intermediate_state_vector);
    // store result in state_vector
    swap_state_vectors();
    
    // multiply input with input kernel
    m_mult_fq(&input_vector, &W_input, &intermediate_state_vector);
    
    // add both results and add bias
    m_add_ff(&state_vector, &intermediate_state_vector, &state_vector);
    m_add_fq(&state_vector, &b_state, &state_vector);
    
    //apply activation function
    m_tanh_f(&state_vector);
    
    //multiply with output kernel
    m_mult_fq(&state_vector, &W_output, &output_vector);

    //add output bias
    m_add_ff(&output_vector, &b_output, &output_vector);
    
    //multiply logits with temperature...
    m_mult_fs(&output_vector, 1 / temperature);
    //... and apply softmax to get normalized probabilities
    m_softmax_f(&output_vector);
    
    rps predicted_opponent_move = m_sample_f(&output_vector);
    
    switch (predicted_opponent_move) {
        case ROCK: // predicted rock, we play paper
            our_last_move = PAPER;
            break;
        case PAPER: // predicted paper, we play scissors
            our_last_move = SCISSORS;
            break;
        case SCISSORS: // predicted scissors, we play rock
            our_last_move = ROCK;
            break;
        case START: // should never happen
            break;
    };
    return our_last_move;
};