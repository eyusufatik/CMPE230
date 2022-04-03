#include "output_header.h"
int main(){
float i = 0;
float n = 0;
out_vector x = make_out_vector(2);
out_vector y = make_out_vector(2);
out_matrix A = make_out_matrix(2, 2);
out_matrix B = make_out_matrix(1, 2);
n = 10;
x.elements[0] = 1;x.elements[1] = 1;
A.elements[0][0] = 1;A.elements[0][1] = 1.67;A.elements[1][0] = 1;A.elements[1][1] = 0;
B.elements[0][0] = 1;B.elements[0][1] = 0;
my_print_v(x);
for(i = 1; i < n; i += 1) {
A = v_m_mul(x, B);
my_print_s(y.elements[round(1)]);
}
}
