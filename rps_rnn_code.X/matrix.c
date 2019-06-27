#include <stdint.h>

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
                res_entry += m1->data[row * m1->cols + i] * m2->data[i * m2->cols + col];
            }
            // res is shape (m1->rows, m2->cols)
            // res[row, col]
            res->data[row * m2->cols + col] = res_entry * m2->scale + m2->offset;
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