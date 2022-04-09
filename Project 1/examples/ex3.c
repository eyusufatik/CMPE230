#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#define EPSILON 0.00000001
// typedef struct out_vector{
//     float *elements;
//     size_t size;
// }out_vector;

typedef struct out_matrix{
    float **elements;
    size_t rows;
    size_t cols;
}out_matrix;

int my_round(float f){
    float rounded = round(f);
    return (int) rounded;
}

void print_float(float x){
    if(fabs(x - round(x)) < EPSILON)
        printf("%d\n", my_round(x));
    else
        printf("%.7f\n", x);
}

// out_vector make_out_vector(size_t size){
//     out_vector vec;
//     vec.elements = calloc(size, sizeof(float));
//     vec.size = size;
//     return vec;
// }

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
    print_float(s);
}

void my_print_v(out_matrix v) {
    for(int i=0; i<v.rows; i++){
        print_float(v.elements[i][0]);
    }
}

void my_print_m(out_matrix m) {
    for(int i=0; i<m.rows; i++){
        for(int j=0; j<m.cols; j++){
            print_float(m.elements[i][j]);
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

out_matrix tr_v(out_matrix v) {
    out_matrix tr_v = make_out_matrix(1, v.rows);
    for(int i=0; i<v.rows; i++){
        tr_v.elements[0][i] = v.elements[i][0];
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

out_matrix m_v_mul(out_matrix m, out_matrix v) {
    out_matrix m_v_mul_m = make_out_matrix(m.rows, 1);
    for(int i=0; i<m.rows; i++){
        for(int j=0; j<m.cols; j++)
            m_v_mul_m.elements[i][0] += m.elements[i][j] * v.elements[j][0];
    }
    return m_v_mul_m;
}

out_matrix m_v_sum(out_matrix m, out_matrix v) {
    out_matrix m_v_sum_m = make_out_matrix(m.rows, m.cols);
    for(int i=0; i<m.rows; i++){
        for(int j=0; j<m.cols; j++){
            m_v_sum_m.elements[i][j] = m.elements[i][j] + v.elements[j][0];
        }
    }
    return m_v_sum_m;
}

out_matrix v_m_mul(out_matrix v, out_matrix m) {
    out_matrix v_m_mul_m = make_out_matrix(v.rows, m.cols);
    for(int i=0; i<v.rows; i++){
        for(int j=0; j<m.cols; j++){
            v_m_mul_m.elements[i][j] += v.elements[i][0] * m.elements[0][j];
        }
    }
    return v_m_mul_m;
}

out_matrix v_v_sum(out_matrix v1, out_matrix v2) {
    out_matrix v_v_sum_v = make_out_matrix(v1.rows, 1);
    for(int i=0; i<v1.rows; i++){
        v_v_sum_v.elements[i][0] = v1.elements[i][0] + v2.elements[i][0];
    }
    return v_v_sum_v;
}

out_matrix v_v_min(out_matrix v1, out_matrix v2) {
    out_matrix v_v_min_v = make_out_matrix(v1.rows, 1);
    for(int i=0; i<v1.rows; i++){
        v_v_min_v.elements[i][0] = v1.elements[i][0] - v2.elements[i][0];
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

out_matrix v_s_mul(out_matrix v, float s) {
    out_matrix v_s_mul_v = make_out_matrix(v.rows, 1);
    for(int i=0; i<v.rows; i++){
        v_s_mul_v.elements[i][0] = v.elements[i][0] * s;
    }
    return v_s_mul_v;
}

// float sqrt(float f) {
//     return sqrtf(f);
// }



float choose(float x, float y, float z, float t){
    if(fabs(x) < EPSILON)
        return y;
    else if(x > EPSILON)
        return z;
    else
        return t;
}

int main(){

out_matrix A = make_out_matrix(3, 3);
out_matrix T = make_out_matrix(1, 1);
out_matrix x = make_out_matrix(3, 1);
out_matrix y = make_out_matrix(3, 1);
float r = 0;
float i = 0;
A.elements[0][0] = 0.5;A.elements[0][1] = 0;A.elements[0][2] = 0.5;A.elements[1][0] = 0;A.elements[1][1] = 0;A.elements[1][2] = 0.5;A.elements[2][0] = 0.5;A.elements[2][1] = 1;A.elements[2][2] = 0;
x.elements[0][0] = 1;x.elements[1][0] = 1;x.elements[2][0] = 1;
for(i = 1; i <= 10; i += 1) {
y = m_v_mul(A, x);
T = m_v_mul(tr_v(v_v_min(y, x)), (v_v_min(y, x)));
r = sqrt(T.elements[my_round(1)-1][my_round(1)-1]);
my_print_s(r);
x = y;
}
print_sep();
my_print_v(x);
}
