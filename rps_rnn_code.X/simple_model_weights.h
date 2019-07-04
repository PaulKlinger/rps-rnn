/* 
 * File:   simple_model_weights.h
 * Author: Paul
 *
 * Created on June 27, 2019, 10:47 PM
 */


#ifndef SIMPLE_MODEL_WEIGHTS_H
#define	SIMPLE_MODEL_WEIGHTS_H

#ifdef	__cplusplus
extern "C" {
#endif

    const struct quantized_matrix W_input;
    const struct quantized_matrix W_recurrent;
    const struct quantized_matrix W_output;
    const struct quantized_matrix b_state;
    const struct float_matrix b_output;


#ifdef	__cplusplus
}
#endif

#endif	/* SIMPLE_MODEL_WEIGHTS_H */

