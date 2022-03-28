#include "stdlib.h"
#include "stdio.h"

typedef struct out_vector{
    float *elements;
    size_t size;
}out_vector;

typedef struct out_matrix{
    float **elements;
    size_t rows;
    size_t cols;
}out_matrix;

out_vector make_out_vector(size_t size){
    out_vector vec;
    vec.elements = calloc(size, sizeof(float));
    vec.size = size;
    return vec;
}

out_matrix make_out_matrix(size_t rows, size_t cols){
    out_matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.elements = malloc(rows * sizeof(float*));
    for(int i = 0; i < rows; i++){
        mat.elements[i] = calloc(cols, sizeof(float));
    }
    return mat;
}