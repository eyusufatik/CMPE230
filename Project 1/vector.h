#define VEC_START_SIZE 10
//#define VEC_LAST_ELEMENT "\0"
#include "stdlib.h"
#include "stdio.h"

typedef struct vector{
    size_t size;
    size_t first_empty_index;
    size_t real_size;
    void **elements;
}vector;

vector make_str_vector(){
    vector vector;
    vector.elements = (void**)malloc(sizeof(void*)*VEC_START_SIZE);
    //vector.elements[VEC_START_SIZE] = VEC_LAST_ELEMENT;
    vector.size = 0;
    vector.first_empty_index = 0;
    vector.real_size = VEC_START_SIZE;
    return vector;
}

void append(vector *vector, char new_element[]){
    // time to resize array
    if(vector->first_empty_index == vector->real_size){
        vector->real_size *= 2;
        vector->elements = (void**)realloc(vector->elements, sizeof(void*)*vector->real_size);
        if(vector->elements == NULL){
            printf("%s", "Error while reallocating memory for vector.");
        }
    }
    vector->elements[vector->first_empty_index] = new_element;
    ++vector->size;
    ++vector->first_empty_index;
}

void free_str_vector(vector *vector){
    free(vector->elements);
}

