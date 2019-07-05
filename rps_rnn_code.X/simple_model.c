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

float temp_state_vector_data[10];
struct float_matrix temp_state_vector = {
    .data = temp_state_vector_data,
    .rows = 1,
    .cols = 10,
};

float l1_state_vector_data[10];
struct float_matrix l1_state_vector = {
    .data = l1_state_vector_data,
    .rows = 1,
    .cols = 10,
};

float l2_state_vector_data[10];
struct float_matrix l2_state_vector = {
    .data = l2_state_vector_data,
    .rows = 1,
    .cols = 10,
};

float l3_state_vector_data[10];
struct float_matrix l3_state_vector = {
    .data = l3_state_vector_data,
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
            l1_state_vector_data[i] = 0;
            l2_state_vector_data[i] = 0;
            l3_state_vector_data[i] = 0;
            temp_state_vector_data[i] = 0;
        }
    } else {
        input_vector_data[opponent_move] = 1;
        input_vector_data[3 + our_last_move] = 1;
    };
};

void swap_matrices(struct float_matrix *a, struct float_matrix *b) {
    // swap float matrices a and b
    struct float_matrix temp = *a;
    *a = *b;
    *b = temp;
}

void recurrent_layer(struct float_matrix *input_vec,
                     struct float_matrix *state_vec,
                     struct float_matrix *temp_state_vec,
                     const struct quantized_matrix *W_in,
                     const struct quantized_matrix *W_rec,
                     const struct quantized_matrix *bias) {
    volatile float state_0 = state_vec->data[0];
    volatile float state_1 = state_vec->data[1];
    volatile float state_2 = state_vec->data[2];
    volatile float state_3 = state_vec->data[3];
    volatile float state_4 = state_vec->data[4];
    // multiply previous state vector with recurrent kernel
    m_mult_fq(state_vec, W_rec, temp_state_vec);
    volatile float t_state_0 = temp_state_vec->data[0];
    volatile float t_state_1 = temp_state_vec->data[1];
    volatile float t_state_2 = temp_state_vec->data[2];
    volatile float t_state_3 = temp_state_vec->data[3];
    volatile float t_state_4 = temp_state_vec->data[4];
    // store result in state_vector
    swap_matrices(state_vec, temp_state_vec);
    state_0 = state_vec->data[0];
    state_1 = state_vec->data[1];
    state_2 = state_vec->data[2];
    state_3 = state_vec->data[3];
    state_4 = state_vec->data[4];
    
    volatile float input_vec_0 = input_vec->data[0];
    volatile float input_vec_1 = input_vec->data[1];
    volatile float input_vec_2 = input_vec->data[2];
    volatile float input_vec_3 = input_vec->data[3];
    volatile float input_vec_4 = input_vec->data[4];
    // multiply input with input kernel
    m_mult_fq(input_vec, W_in, temp_state_vec);
    
    // add both results and add bias
    m_add_ff(state_vec, temp_state_vec, state_vec);
    m_add_fq(state_vec, bias, state_vec);
    
    //apply activation function
    m_softsign_f(state_vec);
}


rps simple_model_predict(rps opponent_move, float temperature) {
    set_input(opponent_move);

    recurrent_layer(&input_vector, &l1_state_vector, &temp_state_vector,
                    &l1_W_input, &l1_W_recurrent, &l1_b_state);
    volatile float l1_state_0 = l1_state_vector.data[0];
    volatile float l1_state_1 = l1_state_vector.data[1];
    volatile float l1_state_2 = l1_state_vector.data[2];
    volatile float l1_state_3 = l1_state_vector.data[3];
    volatile float l1_state_4 = l1_state_vector.data[4];
    recurrent_layer(&l1_state_vector, &l2_state_vector, &temp_state_vector,
    
                    &l2_W_input, &l2_W_recurrent, &l2_b_state);
    recurrent_layer(&l2_state_vector, &l3_state_vector, &temp_state_vector,
                    &l3_W_input, &l3_W_recurrent, &l3_b_state);
    volatile float l3_state_0 = l3_state_vector.data[0];
    volatile float l3_state_1 = l3_state_vector.data[1];
    volatile float l3_state_2 = l3_state_vector.data[2];
    volatile float l3_state_3 = l3_state_vector.data[3];
    volatile float l3_state_4 = l3_state_vector.data[4];
    
    
    //multiply with output kernel
    m_mult_fq(&l3_state_vector, &W_output, &output_vector);

    //add output bias
    m_add_ff(&output_vector, &b_output, &output_vector);
    
    //divide logits by temperature...
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