#ifndef VECTOR_H
#define VECTOR_H

#define VEC_START_SIZE 10
//#define VEC_LAST_ELEMENT "\0"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

// Array list like type.
typedef struct vector{
    size_t size;
    size_t real_size;
    void **elements;
}vector;

// create verctor
vector vec_make_vector(){
    vector vector;
    vector.elements = (void**)malloc(sizeof(void*)*VEC_START_SIZE);
    //vector.elements[VEC_START_SIZE] = VEC_LAST_ELEMENT;
    vector.size = 0;
    vector.real_size = VEC_START_SIZE;
    return vector;
}

// append element to vector
void vec_append(vector *vector, void *new_element, size_t size){
    // time to resize array
    if(vector->size == vector->real_size){
        vector->real_size *= 2;
        vector->elements = (void**)realloc(vector->elements, sizeof(void*)*vector->real_size);
        if(vector->elements == NULL){
            printf("%s", "Error while reallocating memory for vector.");
        }
    }
    vector->elements[vector->size] = malloc(size);
    memcpy(vector->elements[vector->size], new_element, size);
    ++vector->size;
}

// append str to vector (the other append function can be used as well, this one's just easier)
void vec_str_append(vector *vector, char *new_element){
    // time to resize array
    if(vector->size == vector->real_size){
        vector->real_size *= 2;
        vector->elements = (void**)realloc(vector->elements, sizeof(void*)*vector->real_size);
        if(vector->elements == NULL){
            printf("%s", "Error while reallocating memory for vector.");
        }
    }
    vector->elements[vector->size] = malloc(strlen(new_element) + 1);
    vector->elements[vector->size] = strdup(new_element);
    ++vector->size;
}

// remove element from vector
void vec_remove(vector *vector, size_t index){
    if (index <= vector->size){
        for(int i = index; i < vector->size - 1; i++){
            vector->elements[i] = vector->elements[i+1];
        }
        vector->elements[vector->size-1] = NULL;
        --vector->size;
    }
}

// find str in vector (the other find function can be used as well)
size_t vec_str_find(vector vector, void* element){
    size_t index = -1;
    for(int i = 0; i < vector.size; i++){
        if (strcmp((char *)vector.elements[i], (char *)element) == 0){
            index = i;
            break;
        }
    }
    return index;
}

// do we even need this?
void free_str_vector(vector *vector){
    free(vector->elements);
}
#endif