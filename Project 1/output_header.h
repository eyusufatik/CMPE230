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

void print_sep() {
    printf("----------\n");
}

void my_print_s(float s) {
    printf("%f\n", s);
}

void my_print_v(out_vector v) {
    for(int i=0; i<v.size; i++){
        printf("%f\n", ((flaot *)v.elements)[i]);
    }
}

void my_print_m(out_matrix m) {
    for(int i=0; i<m.rows; i++){
        for(int j=0; j<m.cols; j++){
            printf("%f\n", ((float **)m.elements)[i][j]);
        }
    }
}

out_matrix tr(out_matrix m) {
    out_matrix tr_m = make_out_matrix(m.cols, m.rows);
    for(int i=0; i<m.rows; i++){
        for(int j=0; j<m.cols; j++){
            ((float **)tr_m.elements)[j][i] = ((float **)m.elements)[i][j];
        }
    }
    return tr_m;
}

out_matrix tr(out_vector v) {
    out_matrix tr_v = make_out_matrix(1, v.size);
    for(int i=0; i<v.size; i++){
        ((float **)tr_v.elements)[0][i] = ((float *)v.elements)[i];
    }
    return tr_v;
}

out_matrix m_m_mul(out_matrix m1,out_matrix m2) {
    out_matrix m_m_mul_m = make_out_matrix(m1.rows, m2.cols);
    for(int i=0; i<m1.rows; i++){
        for(int j=0; j<m2.cols; j++){
            for(int k=0; k<m1.cols; k++){
                ((float **)m_m_mul_m.elements)[i][j] += ((float **)m1.elements)[i][k] * ((float **)m2.elements)[k][j];
            }
        }
    }
    return m_m_mul_m;
}
out_matrix m_m_sum(out_matrix m1, out_matrix m2) {
    out_matrix m_m_sum_m = make_out_matrix(m1.rows, m1.cols);
    for(int i=0; i<m1.rows; i++){
        for(int j=0; j<m1.cols; j++){
            ((float **)m_m_sum_m.elements)[i][j] = ((float **)m1.elements)[i][j] + ((float **)m2.elements)[i][j];
        }
    }
    return m_m_sum_m;
}

out_matrix m_m_min(out_matrix m1, out_matrix m2) {
    out_matrix m_m_min_m = make_out_matrix(m1.rows, m1.cols);
    for(int i=0; i<m1.rows; i++){
        for(int j=0; j<m1.cols; j++){
            ((float **)m_m_min_m.elements)[i][j] = ((float **)m1.elements)[i][j] - ((float **)m2.elements)[i][j];
        }
    }
    return m_m_min_m;
}

out_matrix m_v_mul(out_matrix m,out_vector v) {
    out_matrix m_v_mul_m = make_out_matrix(m.rows, m.cols);
    for(int i=0; i<m.rows; i++){
        for(int j=0; j<m.cols; j++){
            ((float **)m_v_mul_m.elements)[i][j] = ((float **)m.elements)[i][j] * ((float *)v.elements)[j];
        }
    }
    return m_v_mul_m;
}

out_matrix m_v_sum(out_matrix m,out_vector v) {
    out_matrix m_v_sum_m = make_out_matrix(m.rows, m.cols);
    for(int i=0; i<m.rows; i++){
        for(int j=0; j<m.cols; j++){
            ((float **)m_v_sum_m.elements)[i][j] = ((float **)m.elements)[i][j] + ((float *)v.elements)[j];
        }
    }
    return m_v_sum_m;
}

out_vector v_v_sum(out_vector v, out_vector v) {
    out_vector v_v_sum_v = make_out_vector(v.size);
    for(int i=0; i<v.size; i++){
        ((float *)v_v_sum_v.elements)[i] = ((float *)v.elements)[i] + ((float *)v.elements)[i];
    }
    return v_v_sum_v;
}

out_vector v_v_min(out_vector v, out_vector v) {
    out_vector v_v_min_v = make_out_vector(v.size);
    for(int i=0; i<v.size; i++){
        ((float *)v_v_min_v.elements)[i] = ((float *)v.elements)[i] - ((float *)v.elements)[i];
    }
    return v_v_min_v;
}

float s_s_mul(float s1, float s2) {
    return s1 * s2;
}

float s_s_sum(float s1, float s2) {
    return s1 + s2;
}

float s_s_min(float s1, float s2) {
    return s1 - s2;
}

out_matrix m_s_mul(out_matrix m, float s) {
    out_matrix m_s_mul_m = make_out_matrix(m.rows, m.cols);
    for(int i=0; i<m.rows; i++){
        for(int j=0; j<m.cols; j++){
            ((float **)m_s_mul_m.elements)[i][j] = ((float **)m.elements)[i][j] * s;
        }
    }
    return m_s_mul_m;
}

out_vector v_s_mul(out_vector v, float s) {
    out_vector v_s_mul_v = make_out_vector(v.size);
    for(int i=0; i<v.size; i++){
        ((float *)v_s_mul_v.elements)[i] = ((float *)v.elements)[i] * s;
    }
    return v_s_mul_v;
}

float sqrt(float f) {
    return sqrtf(f);
}
