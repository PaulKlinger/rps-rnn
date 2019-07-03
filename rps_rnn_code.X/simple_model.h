/* 
 * File:   simple_model.h
 * Author: Paul
 *
 * Created on June 27, 2019, 11:24 PM
 */

#ifndef SIMPLE_MODEL_H
#define	SIMPLE_MODEL_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum rps {
        ROCK = 0,
        PAPER = 1,
        SCISSORS = 2,
        START = 3
    } rps;
    
    rps simple_model_predict(rps opponent_move, float temperature);


#ifdef	__cplusplus
}
#endif

#endif	/* SIMPLE_MODEL_H */

