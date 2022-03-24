#include "stdlib.h"
#include "stdio.h"

typedef struct out_vector{
    void *elements;
    size_t size;
}out_vector;

typedef struct out_matrix{
    void **elements;
    size_t rows;
    size_t cols;
}out_matrix;

out_vector make_out_vector(size_t size){
    out_vector vec;
    vec.elements = calloc(size, sizeof(int));
    vec.size = size;
    return vec;
}

out_matrix make_out_matrix(size_t rows, size_t cols){
    out_matrix mat;
    mat.rows = rows;
    mat.cols = cols;
    mat.elements = malloc(rows * sizeof(void*));
    for(int i = 0; i < rows; i++){
        mat.elements[i] = calloc(cols, sizeof(int));
    }
    return mat;
}