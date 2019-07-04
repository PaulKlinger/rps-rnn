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

void m_mult_qf(const struct quantized_matrix *m1,
               const struct float_matrix *m2,
               struct float_matrix *res);

void m_mult_fq(const struct float_matrix *m1,
               const struct quantized_matrix *m2,
               struct float_matrix *res);

void m_add_ff(const struct float_matrix *m1,
              const struct float_matrix *m2,
              struct float_matrix *res);

void m_add_fq(const struct float_matrix *m1,
              const struct quantized_matrix *m2, 
              struct float_matrix *res);

void m_tanh_f(struct float_matrix *m);
void m_softsign_f(struct float_matrix *m);
void m_mult_fs(struct float_matrix *m, float x);

float m_max_f(struct float_matrix *m);
void m_softmax_f(struct float_matrix *m);
uint8_t m_sample_f(const struct float_matrix *probs);


#ifdef	__cplusplus
}
#endif

#endif	/* MATRIX_H */

