#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "output_header.h"


int main(){
    out_matrix x = make_out_matrix(2,2);
    ((int *)(x.elements[0]))[0] = 5;
    printf("%d", ((int *)(x.elements[0]))[0]);

}