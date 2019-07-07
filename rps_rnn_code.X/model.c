#include <stdint.h>
#include "model.h"
#include "matrix.h"
#include "model_weights.h"

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

float expected_scores_vector_data[3];
struct float_matrix expected_scores_vector = {
    .data = expected_scores_vector_data,
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
    // multiply previous state vector with recurrent kernel
    m_mult_fq(state_vec, W_rec, temp_state_vec);
    // store result in state_vector
    swap_matrices(state_vec, temp_state_vec);
    
    // multiply input with input kernel
    m_mult_fq(input_vec, W_in, temp_state_vec);
    
    // add both results and add bias
    m_add_ff(state_vec, temp_state_vec, state_vec);
    m_add_fq(state_vec, bias, state_vec);
    
    //apply activation function
    m_softsign_f(state_vec);
}


rps model_predict(rps opponent_move, float temperature) {
    set_input(opponent_move);

    recurrent_layer(&input_vector, &l1_state_vector, &temp_state_vector,
                    &l1_W_input, &l1_W_recurrent, &l1_b_state);
    recurrent_layer(&l1_state_vector, &l2_state_vector, &temp_state_vector,
    
                    &l2_W_input, &l2_W_recurrent, &l2_b_state);
    recurrent_layer(&l2_state_vector, &l3_state_vector, &temp_state_vector,
                    &l3_W_input, &l3_W_recurrent, &l3_b_state);
    
    
    //multiply with output kernel
    m_mult_fq(&l3_state_vector, &W_output, &output_vector);

    //add output bias
    m_add_ff(&output_vector, &b_output, &output_vector);
    
    
    //apply softmax to get normalized probabilities
    m_softmax_f(&output_vector);
    
    // calculate the expected score from playing each move
    // (loss=-1, win=1)
    // This is better than just playing the counter move to the predicted move,
    // e.g. when output = (0.1, 0.44, 0.46)
    // expected move would be scissors but playing rock gives expected score
    // of 0.02 while playing scissors gives 0.34
    
    expected_scores_vector.data[0] = -output_vector.data[1] + output_vector.data[2];
    expected_scores_vector.data[1] = -output_vector.data[2] + output_vector.data[0];
    expected_scores_vector.data[2] = -output_vector.data[0] + output_vector.data[1];
    
    // we use these expected scores as logits to compute the probability
    // that we should choose this move
    // divide logits by temperature...
    m_mult_fs(&expected_scores_vector, 1 / temperature);
    
    // normalize expected score using softmax
    m_softmax_f(&expected_scores_vector);
    
    our_last_move = m_sample_f(&expected_scores_vector);
    
    return our_last_move;
};