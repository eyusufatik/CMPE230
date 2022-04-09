#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "vector.h"
#include "makers.h"
#include "matchers.h"
#include "utils.h"
#include "vars.h"
#include "expressions.h"

/*
    Returns function name, return type and order of the vars in the funciton.
*/
char* get_func_for_arithmetic_op(int type_left, int type_right, char operator, int *ret_type, bool *in_reverse){
    // matrix operator matrix
    if(type_left == 2 && type_right ==2){
            *ret_type = 2;
            if (operator == '*')
                return "m_m_mul";
            else if (operator == '+')
                return "m_m_sum";
            else
                return "m_m_min";
    }
    // matrix operator vector or vector operator matrix
    else if((type_left == 2 && type_right == 1) ||
                    (type_left == 1 && type_right == 2)){
        *in_reverse = (type_left == 1);
        if(in_reverse)
            *ret_type = 2;
        else
            *ret_type = 1;

        if(*in_reverse && operator == '*'){
            *in_reverse = false;
            return "v_m_mul";
        }
        
        if (operator == '*')
            return "m_v_mul";
        else if (operator == '+')
            return "m_v_sum";
        else
            return "m_v_min";
        
    }
    // vector operator vector
    else if(type_left == 1 && type_right == 1){
        *ret_type = 1;
        if (operator == '*'){
            *ret_type = -1;
            return throw_error();
        }
        else if (operator == '+')
            return "v_v_sum";
        else
            return "v_v_min";
    }
    // scalar operator scalar
    else if(type_left== 0 && type_right == 0){
        *ret_type = 0;
        if (operator == '*')
            return "s_s_mul";
        else if (operator == '+')
            return "s_s_sum";
        else
            return "s_s_min";
    }
    // matrix operator scalar or scalar operator matrix
    else if(operator == '*' && ((type_left == 2 && type_right == 0) ||
                                    (type_left == 0 && type_right == 2))){
        *in_reverse = type_left == 0;
        *ret_type = 2;
        return "m_s_mul";
    }
    // vector operator scalar or scalar operator vector
    else if(operator == '*' && ((type_left == 1 && type_right == 0) ||
                                    (type_left == 0 && type_right == 1))){
        *in_reverse = type_left == 0;
        *ret_type = 1;
        return "v_s_mul";
    }
    // couldn't match var types
    else{
        *ret_type = -1;
        return "";
    }
}


/*
    Returns pointer at end of a expression in a choose func.
*/
char* get_inside_choose_expr_close(char *str){
    char *ptr = str;
    int p_level = 0; // paranthesis
    int s_level = 0; // square bracket
    while(*ptr != '\0'){
        if (*ptr == '(')
            ++p_level;
        else if(*ptr == ')')
            --p_level;
        else if(*ptr == '[')
            ++s_level;
        else if(*ptr == ']')
            --s_level;

        if(s_level == 0 && p_level == 0 && *ptr == ',')
            return ptr;
        
        ++ptr;
    }
}

/*
    Returns expression in C, return type of expr and dimensions.
    out ret type: 0 scalar, 1 vector, 2 matrix
*/
char* convert_complex_expr(char *expr, int *ret_type, int *rows, int *cols){
    trim(expr);
    
    // a(* + -)b
    if(match_binary_arithmetic_expr(expr)){
        vector tokens = tokenize(expr, " *+-\n\t");

        int index_left = -1;
        int index_right = -1;
        char *type_left = get_var_type_and_index(tokens.elements[0], &index_left);
        char *type_right = get_var_type_and_index(tokens.elements[1], &index_right);

        if(index_left == -1 || index_right == -1){
            return throw_error();
        }
    
        char operator;

        if(strchr(expr, '*'))
            operator = '*';
        else if(strchr(expr, '+'))
            operator = '+';
        else
            operator = '-';

        

        // olabilecekler alttakiler
        // m (* + -) m
        // m (* + -) v
        // v (* + -) m
        // v (+ -) v
        // m * s
        // s * m
        // v * s
        // v * s
        // s (* + -) s
        
        // either m_m_sum m_m_mul m_m_min or v s
        char *func_name = malloc(8);
        bool in_reverse = false;
        int out_type = -1;
        int type_left_int = type_to_int(type_left);
        int type_right_int = type_to_int(type_right);
        strcpy(func_name, get_func_for_arithmetic_op(type_left_int, type_right_int, operator, &out_type, &in_reverse));
        *ret_type = out_type;
        if(out_type == -1){
            free(func_name);
            return throw_error();
        }

        // special controls (mostly dimension checks) for different types of arithemtic operations.
        switch(type_left_int){
            case 2:
                switch(type_right_int){
                    case 2:
                        if((operator == '+' || operator == '-') && ( ((int *)vars.matrix_dimensions.elements[index_left])[0] != ((int *)vars.matrix_dimensions.elements[index_right])[0] || 
                                ((int *)vars.matrix_dimensions.elements[index_left])[1] != ((int *)vars.matrix_dimensions.elements[index_right])[1])){                            
                            return throw_error();
                        }else if(((int *)vars.matrix_dimensions.elements[index_left])[1] != ((int *)vars.matrix_dimensions.elements[index_right])[0]){
                            return throw_error();
                        }
                        *rows = ((int *)vars.matrix_dimensions.elements[index_left])[0];
                        *cols = ((int *)vars.matrix_dimensions.elements[index_right])[1];
                        break;
                    case 1:
                        if(((int *)vars.matrix_dimensions.elements[index_left])[1] != ((int *)vars.vector_dimensions.elements[index_right])[0])
                            return throw_error();
                        *rows = ((int *)vars.matrix_dimensions.elements[index_left])[0];
                        *cols = 1;
                        break;
                    case 0:
                        *rows = ((int *)vars.matrix_dimensions.elements[index_left])[0];
                        *cols = ((int *)vars.matrix_dimensions.elements[index_left])[1];
                        break;
                    default:
                        break;
                }
                break;
            case 1:
                switch(type_right_int){
                    case 2:
                        if(((int **)vars.matrix_dimensions.elements)[index_right][0] != 1)
                            return throw_error();
                        *rows = ((int *)vars.vector_dimensions.elements[index_left])[0];
                        *cols = ((int *)vars.matrix_dimensions.elements[index_right])[1];
                        break;
                    case 1:
                        if(((int **)vars.vector_dimensions.elements)[index_left][0] != ((int **)vars.vector_dimensions.elements)[index_right][0] || operator == '*')
                            return throw_error();
                        *rows = ((int *)vars.vector_dimensions.elements[index_left])[0];
                        *cols = 1;
                        break;
                    case 0:
                        *rows = ((int *)vars.vector_dimensions.elements[index_left])[0];
                        *cols = 1;
                        break;
                    default:
                        break;
                }
                break;
            default:
                *rows = 1;
                *cols = 1;
                break;
        }

        // {func_name}({var1}, {var2})
        char *ret = malloc(strlen(func_name) + 1 + strlen(tokens.elements[0]) + 2 + strlen(tokens.elements[1]) + 2);
        strcpy(ret, func_name);
        strcat(ret, "(");
        if(in_reverse){
            strcat(ret, tokens.elements[1]);
            strcat(ret, ", ");
            strcat(ret, tokens.elements[0]);
        }else{
            strcat(ret, tokens.elements[0]);
            strcat(ret, ", ");
            strcat(ret, tokens.elements[1]);
        }
        strcat(ret, ")");
        strcat(ret, "\0");
        return ret;
    }
    // A[i,j]
    else if(match_indexed_matrix(expr)){
        vector tokens = tokenize(expr, " [,]");
        // {var_name}.elements[{i}][{j}]
        char *ret = malloc(strlen(tokens.elements[0]) + 21 + strlen(tokens.elements[1]) + 14 + strlen(tokens.elements[2]) + 3);
        strcpy(ret, tokens.elements[0]);
        strcat(ret, ".elements[my_round(");
        strcat(ret, tokens.elements[1]);
        strcat(ret, ")-1][my_round(");
        strcat(ret, tokens.elements[2]);
        strcat(ret, ")-1]\0");
        *ret_type = 0;
        *rows = 1;
        *cols = 1;
        return ret;
    }
    // v[i]
    else if(match_indexed_vector(expr)){
        vector tokens = tokenize(expr, " [,]");
        // {var_name}.elements[{i}]
        char *ret = malloc(strlen(tokens.elements[0]) + 19 + strlen(tokens.elements[1]) + 7);
        strcpy(ret, tokens.elements[0]);
        strcat(ret, ".elements[my_round(");
        strcat(ret, tokens.elements[1]);
        strcat(ret, ")-1][0]\0");
        *ret_type = 0;
        *rows = 1;
        *cols = 1;
        return ret;
    }
    // x
    else if(match_var(expr)){
        int index = -1;
        char *type = get_var_type_and_index(expr, &index);

        if(index == -1){
            // var not found.
            return throw_error();
        }

        if(strcmp(type, "matrix") == 0){
            *rows = ((int *)vars.matrix_dimensions.elements[index])[0];
            *cols = ((int *)vars.matrix_dimensions.elements[index])[1];
        }else if(strcmp(type, "vector") == 0){
            *rows = ((int *)vars.vector_dimensions.elements[index])[0];
            *cols = 1;
        }else{
            *rows = 1;
            *cols = 1;
        }
        *ret_type = type_to_int(type);
        return expr;
    }
    // <number>
    else if(match_num(expr)){
        *ret_type = 0;
        *rows = 1;
        *cols = 1;
        return expr;
    }
    // <expr> <op> <expr>
    else if(match_expr_op_expr(expr)){
        int level = 0;
        for(int i = 0; i < strlen(expr); i++){
            char c = expr[i];

            // so that we don't evaluate inside of paranthesis
            if(c == '(')
                ++level;
            else if(c == ')')
                --level;
            if(level == 0 && (c == '+' || c == '-' || c == '*')){
                char *left = malloc(i);
                char *right = malloc(strlen(expr) - i);
                strncpy(left, expr, i);
                strncpy(right, expr+i+1, strlen(expr)-i);

                int left_rows = -1;
                int left_cols = -1;
                int left_type = -1;
                char *left_in_c = malloc(255);
                strcpy(left_in_c, convert_complex_expr(left, &left_type, &left_rows, &left_cols));

                int right_rows = -1;
                int right_cols = -1;
                int right_type = -1;
                char *right_in_c = malloc(255);
                strcpy(right_in_c, convert_complex_expr(right, &right_type, &right_rows, &right_cols));

                // if any of the expressions is vector/matrix check dimensions
                if(left_type != 0 || right_type != 0){
                    if((c == '+' || c == '-') && (left_rows != right_rows || left_cols != right_cols)){
                        free(left_in_c);
                        free(right_in_c);
                        // free(left);
                        // free(right);
                        return throw_error();
                    }else if(c == '*' && left_cols != right_rows){
                        free(left_in_c);
                        free(right_in_c);
                        // free(left);
                        // free(right);
                        return throw_error();
                    }
                }

                if(c == '*'){
                    *rows = left_rows;
                    *cols = right_cols;
                }else{
                    *rows = left_rows;
                    *cols = right_cols;
                }

                int out_type = -1;
                bool in_reverse = false;
                char *func_name = malloc(8);
                strcpy(func_name, get_func_for_arithmetic_op(left_type, right_type, c, &out_type, &in_reverse));

                *ret_type = out_type;
                char *ret = malloc(8 + 1 + strlen(left_in_c) + 2 + strlen(right_in_c) + 2);
                strcpy(ret, func_name);
                strcat(ret, "(");
                strcat(ret, left_in_c);
                strcat(ret, ", ");
                strcat(ret, right_in_c);
                strcat(ret, ")");
                // printf("%s", ret);
                free(left); free(right); free(left_in_c); free(right_in_c); free(func_name);
                return ret;
            }
        }
    }
    // (<expr>)
    else if(match_expr_in_paran(expr)){
        char *exprr = malloc(strlen(expr)-2);
        strncpy(exprr, expr+1, strlen(expr)-2);
        int inside_type = 0;
        char *expr_in_c = convert_complex_expr(exprr, &inside_type, rows, cols);
        *ret_type = inside_type;
        char *ret = malloc(strlen(expr_in_c) + 2);
        strcpy(ret, "(");
        strcat(ret, expr_in_c);
        strcat(ret, ")");
        // free(exprr);
        // free(expr_in_c);
        return ret;
    }
    // <func>(<expr>)
    else if(match_func(expr)){
        char *first = strchr(expr, '(');
        char *last = strrchr(expr, ')');
        size_t first_index = first - expr+1;
        size_t last_index = last - expr-1;

        char *inside = malloc(last_index-first_index+1);
        strncpy(inside, first+1, last-first-1);
        int inside_type = 0;
        char *inside_in_c = convert_complex_expr(inside, &inside_type, rows, cols);
        *ret_type = inside_type;
        if(strncmp(expr, "tr(", 3) == 0){
            int temp = *rows;
            *rows = *cols;
            *cols = temp;
            if(inside_type == 1)
                *ret_type = 2;
            if(inside_type == 0){
                return inside_in_c;
            }
        }
        if(strncmp(expr, "sqrt(", 5) == 0 && inside_type != 0){
            // free(inside);
            return throw_error();
        }
        char *ret = malloc(first - expr + strlen(inside_in_c) + 3);
        strncpy(ret, expr, first-expr);
        if(inside_type == 1){
            strcat(ret, "_v");
        }
        strcat(ret, "(");
        strcat(ret, inside_in_c);
        strcat(ret, ")");
        // strcat(ret, ")");
        // free(first);
        // free(last);
        // free(inside_in_c);
        // free(inside);
        return ret;
    }
    // choose(<expr_list>)
    else if(match_choose(expr)){
        char *expr1_start = strchr(expr, '(')+1;

        char *expr1_close = get_inside_choose_expr_close(expr1_start);

        size_t expr1_size = expr1_close - expr1_start;
        char *expr1 = malloc(expr1_size+10);
        strncpy(expr1, expr1_start, expr1_size);
        strcat(expr1, "\0");
        printf("%s\n", expr1);
        int dummy = -1;
        int type = -1;
        char *expr1_in_c = convert_complex_expr(expr1, &type, &dummy, &dummy);
        
        if(type != 0){
            // free(expr1);s
            printf("exp1 check\n");
            return throw_error();
        }
        


        char *expr2_close = get_inside_choose_expr_close(expr1_close+1);
        printf("expr2 start: %s\n", expr1_close+1);
        printf("expr2 close: %s\n", expr2_close);
        size_t expr2_size = expr2_close - expr1_close-1;
        printf("expr2 size: %d\n", expr2_size);
        char *expr2 = malloc(expr2_size+10);
        *expr2 = '\0';
        strncpy(expr2, expr1_close+1, expr2_size);
        strcat(expr2, "\0");
        printf("%s\n", expr2);
        type = -1;
        char *expr2_in_c = convert_complex_expr(expr2, &type, &dummy, &dummy);
        if(type != 0){
            // free(expr1);
            // free(expr2);
            printf("exp2 check\n");

            return throw_error();
        }


        char *expr3_close = get_inside_choose_expr_close(expr2_close+1);
        size_t expr3_size = expr3_close - expr2_close;
        char *expr3 = malloc(expr3_size+10);
        strncpy(expr3, expr2_close+1, expr3_size-1);
        strcat(expr3, "\0");

        type = -1;
        char *expr3_in_c = convert_complex_expr(expr3, &type, &dummy, &dummy);
        if(type != 0){
            // free(expr1);
            // free(expr2);
            // free(expr3);
            printf("exp3 check\n");

            return throw_error();
        }

        
        char *expr4_close = strrchr(expr3_close, ')')-1;
        size_t expr4_size = expr4_close - expr3_close;
        char *expr4 = malloc(expr4_size+10);
        strncpy(expr4, expr3_close+1, expr4_size);
        strcat(expr4, "\0");

        type = -1;
        char *expr4_in_c = convert_complex_expr(expr4, &type, &dummy, &dummy);
        if(type != 0){
            // free(expr1);
            // free(expr2);
            // free(expr3);
            // free(expr4);
            printf("exp4 check\n");

            return throw_error();
        }

        // choose({expr1c}, {expr2c}, {expr3c}, {expr4c})
        char *ret = malloc(7 + strlen(expr1_in_c) + 2 + strlen(expr2_in_c) + 2 + strlen(expr3_in_c) + 2 + strlen(expr4_in_c) + 2);
        strcpy(ret, "choose(");
        strcat(ret, expr1_in_c);
        strcat(ret, ", ");
        strcat(ret, expr2_in_c);
        strcat(ret, ", ");
        strcat(ret, expr3_in_c);
        strcat(ret, ", ");
        strcat(ret, expr4_in_c);
        strcat(ret, ")\0");
        *rows = 1;
        *cols = 1;
        // free(expr1);
        // free(expr2);
        // free(expr3);
        // free(expr4);
        return ret;
    }
    return "not imp of complex expr";
}

#endif