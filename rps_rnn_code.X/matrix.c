#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "matrix.h"

void m_mult_qf(struct quantized_matrix *m1,
                      struct float_matrix *m2,
                      struct float_matrix *res) {
    // Matrix multiplies quantized_matrix m1 and float_matrix m2,
    // writing the result to the float_matrix res.
    // res cannot be the same as m2
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

void m_mult_fq(struct float_matrix *m1,
                      struct quantized_matrix *m2,
                      struct float_matrix *res) {
    // Matrix multiplies float_matrix m1 and quantized_matrix m2,
    // writing the result to the float_matrix res.
    // res cannot be the same as m1
    
    // Pre-calculate offset/scale so we can pull the multiplication with
    // the scale factor out of the inner loop
    float offset_over_scale = m2->offset / m2->scale;
    float res_entry;
    for (uint8_t row=0; row < m1->rows; row++) {
        for (uint8_t col=0; col < m2->cols; col++) {
            res_entry = 0;
            for (uint8_t i=0; i<m1->cols; i++) {
                // m1[row, i] * m2[i, col]
                res_entry += m1->data[row * m1->cols + i] * (m2->data[i * m2->cols + col] + offset_over_scale);
            }
            // res is shape (m1->rows, m2->cols)
            // res[row, col]
            res->data[row * m2->cols + col] = res_entry * m2->scale;
        };
    };
};

void m_add_ff(struct float_matrix *m1, struct float_matrix *m2, struct float_matrix *res) {
    // Adds float_matrices m1, m2, writing the result to float_matrix res
    // res can be one of m1, m2
    for (uint8_t i=0; i < m1->rows * m1->cols; i++) {
        res->data[i] = m1->data[i] + m2->data[i];
    };
};

void m_add_fq(struct float_matrix *m1, struct quantized_matrix *m2, struct float_matrix *res) {
    // Adds float_matrix m1 and quantized_matrix m2,
    // writing the result to float_matrix res
    // res can be m1
    for (uint8_t i=0; i < m1->rows * m1->cols; i++) {
        res->data[i] = m1->data[i] + m2->data[i] * m2->scale + m2->offset;
    };
};

void m_tanh_f(struct float_matrix *m) {
    // Applies elementwise tanh to float_matrix m. Works inplace.
    for (uint8_t i=0; i < m->rows * m-> cols; i++) {
        m->data[i] = tanh(m->data[i]);
    }
};

void m_mult_fs(struct float_matrix *m, float x) {
    // Multiplies float matrix m with (scalar) float x. Works inplace.
    for (uint8_t i=0; i < m->rows * m-> cols; i++) {
        m->data[i] = m->data[i] * x;
    }
};

float m_max_f(struct float_matrix *m) {
    // Returns maximum value in float_matrix m
    float res = -FLT_MAX;
    for (uint8_t i=0; i < m->rows * m->cols; i++) {
        if (m->data[i] > res) {
            res = m->data[i];
        };
    };
    return res;
}

void m_softmax_f(struct float_matrix *m) {
    // Applies softmax function to float matrix m
    // softmax: m = exp(m)/sum(exp(m))
    // (the max entry of m is subtracted before exponentiating to prevent
    //  overflows)
    float sum = 0;
    float max_value = m_max_f(m);
    for (uint8_t i=0; i < m->rows * m-> cols; i++) {
        m->data[i] = exp(m->data[i] - max_value);
        sum += m->data[i];
    }
    for (uint8_t i=0; i < m->rows * m-> cols; i++) {
        m->data[i] = m->data[i] / sum;
    }
};

uint8_t m_sample_f(struct float_matrix *probs){
    // sample from a discrete probability distribution given by
    // float_matrix probs of dimensions (1, n).
    // Assumes the entries of probs sum to 1.
    
    // random float between 0 and 1
    float r = random() / ((float) RANDOM_MAX);
    for (uint8_t i=0; i < probs->cols; i++){
        r -= probs->data[i];
        if (r <= 0) {
            return i;
        }
    }
    // This shouldn't happen, but maybe possible due to float errors?
    return probs->cols - 1;
};