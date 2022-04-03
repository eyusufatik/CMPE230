#include "output_header.h"
int main(){
out_matrix A = make_out_matrix(3, 3);
out_matrix T = make_out_matrix(1, 1);
out_vector x = make_out_vector(3);
out_vector y = make_out_vector(3);
float r = 0;
float i = 0;
A.elements[0][0] = 0.5;A.elements[0][1] = 0;A.elements[0][2] = 0.5;A.elements[1][0] = 0;A.elements[1][1] = 0;A.elements[1][2] = 0.5;A.elements[2][0] = 0.5;A.elements[2][1] = 1;A.elements[2][2] = 0;
x.elements[0] = 1;x.elements[1] = 1;x.elements[2] = 1;
for(i = 1; i <= 10; i += 1) {
y = m_v_mul(A, x);
T = m_v_mul(tr_v(v_v_min(y, x)), (v_v_min(y, x)));
r = sqrt(T.elements[my_round(1)][my_round(1)]);
my_print_s(r);
x = y;
}
print_sep();
my_print_v(x);
}
