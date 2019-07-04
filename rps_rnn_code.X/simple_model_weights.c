#include <stdint.h>
#include "matrix.h"

const uint8_t W_input_data[] = {237, 157,  67, 155, 226,  52, 185,  92, 196,  88, 130, 219, 200,
         170, 110,   6, 118, 221, 255, 193,  85,  79, 137, 170, 192, 201,
         158, 156,  76, 189, 140, 130, 156, 155, 184, 142, 175, 197, 196,
         149, 168, 137, 159, 137, 134, 178, 211, 169, 129, 136, 187, 136,
         131, 154, 143,   0, 162,  86, 147, 160};
const struct quantized_matrix W_input = {
    .data = (uint8_t*)W_input_data,
    .rows = 6,
    .cols = 10,
    .scale = 0.012794787276024911,
    .offset = -1.840727
};

const uint8_t W_recurrent_data[] = {
         215, 115, 123,  78, 141, 159, 184, 149, 154, 113, 107, 255, 224,
         166,  54, 133, 181, 102,  50,  29, 144,  51, 120, 117, 168,  80,
          28,  61, 140, 182, 103, 132, 206, 225,  50,  82, 186, 109,  82,
          33,  53,  74,  36, 103, 160, 153, 126, 148, 155,  99, 109,  65,
          70, 161,  99, 138, 105, 123, 106,  70,  89,  56,  95,  98, 108,
          98, 112, 134, 176, 141, 138, 112, 161, 159, 117,  65, 125, 121,
          84,  96, 205, 202, 216, 102, 118, 148, 200, 119, 182, 124, 162,
          85, 156, 102,  64,   0,  18,  57,  34, 146};
const struct quantized_matrix W_recurrent = {
    .data = (uint8_t*)W_recurrent_data,
    .rows = 10,
    .cols = 10,
    .scale = 0.00705262913423426,
    .offset = -0.79503363
};

const uint8_t b_state_data[] = {
         227,  97,  92, 105, 149,   0, 105, 217, 255, 185};
const struct quantized_matrix b_state = {
    .data = (uint8_t*)b_state_data,
    .rows = 1,
    .cols = 10,
    .scale = 0.002377917719822304,
    .offset = -0.2683249
};

const uint8_t W_output_data[] = {
         255, 184,  89,  91,  68, 157, 171, 108, 135, 139, 165, 140, 163,
          79, 221, 148, 189, 103,   0, 102,  60, 196,  98, 135, 117, 209,
         158,   2, 100,  88};
const struct quantized_matrix W_output = {
    .data = (uint8_t*) W_output_data,
    .rows = 10,
    .cols = 3,
    .scale = 0.005261778363994524,
    .offset = -0.6662272
};

const float b_output_data[] = {
         -0.02720571, -0.03758323, -0.16626628};
const struct float_matrix b_output = {
    .data = (float*) b_output_data,
    .rows = 1,
    .cols = 3,
};