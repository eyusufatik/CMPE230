#include "output_header.h"
int main(){
out_matrix A = make_out_matrix(4, 4);
float count = 0;
float incr = 0;
float i = 0;
float j = 0;
A.elements[0][0] = 0;A.elements[0][1] = 1;A.elements[0][2] = 2;A.elements[0][3] = 3;A.elements[1][0] = 4;A.elements[1][1] = 5;A.elements[1][2] = 6;A.elements[1][3] = 7;A.elements[2][0] = 8;A.elements[2][1] = 9;A.elements[2][2] = 1;A.elements[2][3] = 1;A.elements[3][0] = 1;A.elements[3][1] = 2;A.elements[3][2] = 3;A.elements[3][3] = 4;
count = 0;
for(i = 1; i <= 4; i += 1) {for(int j = 1; j <= 4; j += 1) {
}
