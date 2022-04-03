#ifndef UTILS_H
#define UTILS_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "ctype.h"
#include "math.h"

#include "vector.h"
#include "vars.h"

/*
    function we use for errors
*/
char* throw_error(){
    return "error";
}

/*
    Converts "matrix" e.g. to type integer
    0 -> scalar
    1 -> vector
    2 -> matrix
*/
int type_to_int(char *type){
    if(strcmp(type, "matrix") == 0){
        return 2;
    }else if(strcmp(type, "vector") == 0){
        return 1;
    }else if(strcmp(type, "scalar") == 0){
        return 0;
    }else{
        return -1;
    }
}

/*
    Count chars in a string
*/
int countChars( char* s, char c ){
    return *s == '\0'
              ? 0
              : countChars( s + 1, c ) + (*s == c);
}

/*
    Trims whitespace before and after the string by shifting pointers.
*/
char *trim(char *str){
    size_t len = 0;
    char *frontp = str;
    char *endp = NULL;

    if( str == NULL ) { return NULL; }
    if( str[0] == '\0' ) { return str; }

    len = strlen(str);
    endp = str + len;

    while( isspace((unsigned char) *frontp) ) { ++frontp; }
    if( endp != frontp )
    {
        while( isspace((unsigned char) *(--endp)) && endp != frontp ) {}
    }

    if( frontp != str && endp == frontp )
            *str = '\0';
    else if( str + len - 1 != endp )
            *(endp + 1) = '\0';

    
    endp = str;
    if( frontp != str )
    {
            while( *frontp ) { *endp++ = *frontp++; }
            *endp = '\0';
    }

    return str;
}

/*
    Checks if string starts with another string
*/
bool prefix(const char *str, const char *pre){
    return strncmp(str, pre, strlen(pre)) == 0;
}

/*
    Tokenize the string with the given delimeters.
*/
vector tokenize(char *str, char *delimeter){
    vector tokens = vec_make_vector();
    char *token = strtok(strdup(str), delimeter);
    while(token != NULL){
        vec_str_append(&tokens, token);
        token = strtok(NULL, delimeter);
    }
    return tokens;
}

/*
    Returns var type and index of the var in the vars struct vector.
*/
char* get_var_type_and_index(char *var_name, int *index_out){
    size_t index = -1;
    if((index = vec_str_find(vars.scalar_names, var_name)) != -1){
        *index_out = index;
        return "scalar";
    }else if((index = vec_str_find(vars.vector_names, var_name)) != -1){
        *index_out = index;
        return "vector";
    }else if((index = vec_str_find(vars.matrix_names, var_name)) != -1){
        *index_out = index;
        return "matrix";
    }else{
        *index_out = -1;
        return "not_found";
    }
}

/*
    Returns if a number is syntatically correct.
*/
bool proper_number_check(char *num_str){
    int dot_count = 0;
    // check if rhs is proper number
    for(int i=0; i < strlen(num_str); i++){
        if (num_str[i] < 48  || num_str[i] > 57){
            if(num_str[i] == 46){
                dot_count++;
                if(dot_count > 1)
                    return false;
            }else{
                // non-numeric char
                return false;
            }
        }
    }
    
    return true;
}

/*
    Returns if a variable name is syntatically correct.
*/
bool proper_varname_check(char *name){
    char first_char = name[0];
    if(!((first_char <= 'Z' && first_char >= 'A') || (first_char <= 'z' && first_char >= 'a') || first_char == '_')){
        return false;
    }
    for(int i = 1; i < strlen(name); i++){
        char c = name[i];
        if((c <= 'Z' && c >= 'A') || (c <= 'z' && c >= 'a') || c == '_' || (c <= '9' && c >= '0')){
            continue;
        }else{
            return false;
        }
    }
    return true;
}


#endif