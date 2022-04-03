#include "output_header.h"
int main(){
out_matrix A = make_out_matrix(4, 4);
out_matrix T = make_out_matrix(1, 1);
out_matrix x = make_out_matrix(4, 1);
out_matrix xy2 = make_out_matrix(4, 1);
float s = 0;
A.elements[0][0] = 0;A.elements[0][1] = 1;A.elements[0][2] = 2;A.elements[0][3] = 3;A.elements[1][0] = 4;A.elements[1][1] = 5;A.elements[1][2] = 6;A.elements[1][3] = 7;A.elements[2][0] = 8;A.elements[2][1] = 9;A.elements[2][2] = 1;A.elements[2][3] = 1;A.elements[3][0] = 1;A.elements[3][1] = 2;A.elements[3][2] = 3;A.elements[3][3] = 4;
x.elements[0][0] = 1;x.elements[1][0] = 1;x.elements[2][0] = 1;x.elements[3][0] = 1;
xy2.elements[0][0] = 2;xy2.elements[1][0] = 1;xy2.elements[2][0] = 3;xy2.elements[3][0] = 1;
T = m_m_mul(tr_v(x), m_v_mul(A, xy2));
s = T.elements[my_round(1)-1][my_round(1)-1];
my_print_s(s);
}
