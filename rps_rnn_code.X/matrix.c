#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"

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

void mult_float_quant(struct float_matrix *m1,
                      struct quantized_matrix *m2,
                      struct float_matrix *res) {
    float res_entry;
    
    for (uint8_t row=0; row < m1->rows; row++) {
        for (uint8_t col=0; col < m2->cols; col++) {
            res_entry = 0;
            for (uint8_t i=0; i<m1->cols; i++) {
                // m1[row, i] * m2[i, col]
                res_entry += m1->data[row * m1->cols + i] * (m2->data[i * m2->cols + col] * m2->scale + m2->offset);
            }
            // res is shape (m1->rows, m2->cols)
            // res[row, col]
            res->data[row * m2->cols + col] = res_entry;
        };
    };
};

void add_float_float(struct float_matrix *m1, struct float_matrix *m2, struct float_matrix *res) {
    for (uint8_t i=0; i < m1->rows * m1->cols; i++) {
        res->data[i] = m1->data[i] + m2->data[i];
    };
};

void add_float_quant(struct float_matrix *m1, struct quantized_matrix *m2, struct float_matrix *res) {
    for (uint8_t i=0; i < m1->rows * m1->cols; i++) {
        res->data[i] = m1->data[i] + m2->data[i] * m2->scale + m2->offset;
    };
};

void tanh_elementwise(struct float_matrix *m) {
    for (uint8_t i=0; i < m->rows * m-> cols; i++) {
        m->data[i] = tanh(m->data[i]);
    }
};

void mult_float_scalar(struct float_matrix *m, float x) {
    for (uint8_t i=0; i < m->rows * m-> cols; i++) {
        m->data[i] = m->data[i] * x;
    }
};

void softmax(struct float_matrix *m) {
    // softmax: m = exp(m)/sum(exp(m))
    float sum = 0;
    for (uint8_t i=0; i < m->rows * m-> cols; i++) {
        m->data[i] = exp(m->data[i]);
        sum += m->data[i];
    }
    for (uint8_t i=0; i < m->rows * m-> cols; i++) {
        m->data[i] = m->data[i] / sum;
    }
};

uint8_t sample(struct float_matrix *probs){
    // sample from a discrete probability distribution given by a matrix
    // of dimensions (1, n). Assumes the entries of probs sum to 1.
    
    // random float between 0 and 1
    float r = random() / ((float) RANDOM_MAX);
    for (uint8_t i=0; i < probs->cols; i++){
        r -= probs->data[i];
        if (r <= 0) {
            return i;
        }
    }
    return probs->cols - 1;
};