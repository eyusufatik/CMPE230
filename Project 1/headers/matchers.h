#ifndef MATCHERS_H
#define MATCHERS_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "ctype.h"
#include "math.h"

#include "vector.h"
#include "utils.h"
#include "vars.h"

bool match_indexed_matrix(char *expr){ 
    trim(expr);
    int s_open = countChars(expr, '[');
    int s_close = countChars(expr, ']');
    int commas = countChars(expr, ',');



    if(s_open < 1 || s_close < 1 || s_open != s_close || commas < 1 || expr[strlen(expr)-1] != ']')
        return false;
    
    char *var_name_end = strchr(expr, '[') - 1;
    char *var_name = malloc(var_name_end - expr + 1);
    strncpy(var_name, expr, var_name_end - expr + 1);

    int dummy = 0;
    char *type = get_var_type_and_index(var_name, &dummy);
    return strcmp(type, "matrix") == 0;
    // vector tokens = tokenize(expr, "[,]\t");
    // if(tokens.size == 3){
    //     int dummy = 0;
    //     char *type = get_var_type_and_index(tokens.elements[0], &dummy);
    //     return strcmp(type, "matrix") == 0 &&
    //     (proper_number_check(tokens.elements[1]) || strcmp("scalar", get_var_type_and_index(tokens.elements[1], &dummy))
    //     && (proper_number_check(tokens.elements[2])) || strcmp("scalar", get_var_type_and_index(tokens.elements[2], &dummy)));
    // }else{
    //     return false;
    // }
}

bool match_indexed_vector(char *expr){
    trim(expr);

    int s_open = countChars(expr, '[');
    int s_close = countChars(expr, ']');

    if(s_open < 1 || s_close < 1 || s_open != s_close || expr[strlen(expr)-1] != ']')
        return false;
    
    char *var_name_end = strchr(expr, '[') - 1;
    char *var_name = malloc(var_name_end - expr + 1);
    strncpy(var_name, expr, var_name_end - expr + 1);

    int dummy = 0;
    char *type = get_var_type_and_index(var_name, &dummy);
    return strcmp(type, "vector") == 0;
}

bool match_var(char *expr){
    int index = -1;
    get_var_type_and_index(expr, &index);
    return index != -1;
}

bool match_num(char *expr){
    return proper_number_check(expr);
}

bool match_func(char *expr){
    if(prefix(expr, "tr(") || prefix(expr, "sqrt(")){
        int level = 0;
        bool entered_once = false;
        for (int i = 0; i < strlen(expr); i++){
            char c = expr[i];
            if(c == '('){
              level++;
              entered_once = true;
            }
            else if(c == ')'){
              level--;
            }
            if(entered_once && level == 0 && i != strlen(expr)-1)
                return false;
        }
        return true;
    }else{
        return false;
    }
}

bool match_choose(char *expr){
    if(prefix(expr, "choose(")){
        int commaCount = 0; 
        int level = 0;
        bool entered_once = false;
        for (int i = 1; i < strlen(expr); i++){
            char c = expr[i];

            if(c == ',')
                ++commaCount;

            if(c == '('){
                level++;
                entered_once = true;
            }
            else if(c == ')')
                level--;

            if(entered_once && level == 0 && i != strlen(expr)-1)
                return false;
        }
        return commaCount >= 3;
    }else{
        return false;
    }
}

bool match_expr_in_paran(char *expr){
    if(expr[0] == '(' && expr[strlen(expr)-1] == ')'){
        int level = 0;
        for (int i = 0; i < strlen(expr); i++){
            char c = expr[i];
            if(c == '(')
                level++;
            else if(c == ')')
                level--;

            if(level == 0 && i != strlen(expr)-1)
                return false;
        }
        return true;
    }else{
        return false;
    }
}

bool match_expr_op_expr(char *expr){
    int level = 0;
    for (int i = 0; i < strlen(expr); i++){
        char c = expr[i];
        if(c == '(')
            level++;
        else if(c == ')')
            level--;
        // printf("%d ", level);
        if(level == 0 && (c == '*' || c == '+' || c == '-'))
            return true;
    }
    return false;

}

bool match_binary_arithmetic_expr(char *expr){
    if(countChars(expr, '*') + countChars(expr, '+') + countChars(expr, '-') > 1) // throw error maybe?
        return false;

    vector tokens = tokenize(expr, " *+-\n\t");
    if(tokens.size != 2)
        return false;

    return proper_varname_check(tokens.elements[0]) && proper_varname_check(tokens.elements[1]);
    // size_t index_left = -1;
    // size_t index_right = -1;
    // char *type_left = get_var_type_and_index(tokens.elements[0], &index_left);
    // char *type_right = get_var_type_and_index(tokens.elements[1], &index_right);

    // if(index_left == -1 || index_right == -1)
    //     return false;

    // m (* + -) m
    // m (* + -) v
    // v (* + -) m
    // v (+ -) v
    // m * s
    // v * s

}
#endif