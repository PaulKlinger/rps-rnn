/* 
 * File:   matrix.h
 * Author: Paul
 *
 * Created on June 27, 2019, 11:00 PM
 */

#ifndef MATRIX_H
#define	MATRIX_H

#ifdef	__cplusplus
extern "C" {
#endif

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
                      struct float_matrix *res);

void mult_float_quant(struct float_matrix *m1,
                      struct quantized_matrix *m2,
                      struct float_matrix *res);

void add_float_float(struct float_matrix *m1,
                     struct float_matrix *m2,
                     struct float_matrix *res);

void add_float_quant(struct float_matrix *m1,
                    struct quantized_matrix *m2, 
                    struct float_matrix *res);

void tanh_elementwise(struct float_matrix *m);
void mult_float_scalar(struct float_matrix *m, float x);

void softmax(struct float_matrix *m);
uint8_t sample(struct float_matrix *probs);


#ifdef	__cplusplus
}
#endif

#endif	/* MATRIX_H */

