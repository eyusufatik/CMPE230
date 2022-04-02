#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "vector.h"
#include "makers.h"
#include "matchers.h"
#include "utils.h"
#include "vars.h"


char* get_func_for_arithmetic_op(int type_left, int type_right, char operator, int *ret_type, bool *in_reverse){
    if(type_left == 2 && type_right ==2){
            *ret_type = 2;
            if (operator == '*')
                return "m_m_mul";
            else if (operator == '+')
                return "m_m_sum";
            else
                return "m_m_min";
    }else if((type_left == 2 && type_right == 1) ||
                    (type_left == 1 && type_right == 2)){
        *in_reverse = type_left == 1;
        if(in_reverse)
            *ret_type = 2;
        else
            *ret_type = 1;
        
        if (operator == '*')
            return "m_v_mul";
        else if (operator == '+')
            return "m_v_sum";
        else
            return "m_v_min";
        
    }else if(type_left == 1 && type_right == 1){
        *ret_type = 1;
        if (operator == '*'){
            *ret_type = -1;
            return throw_error();
        }
        else if (operator == '+')
            return "v_v_sum";
        else
            return "v_v_min";
    }else if(type_left== 0 && type_right == 0){
        *ret_type = 0;
        if (operator == '*')
            return "s_s_mul";
        else if (operator == '+')
            return "s_s_sum";
        else
            return "s_s_min";
    }else if(operator == '*' && ((type_left == 2 && type_right == 0) ||
                                    (type_left == 0 && type_right == 2))){
        *in_reverse = type_left == 0;
        *ret_type = 2;
        return "m_s_mul";
    }else if(operator == '*' && ((type_left == 1 && type_right == 0) ||
                                    (type_left == 0 && type_right == 1))){
        *in_reverse = type_left == 0;
        *ret_type = 1;
        return "v_s_mul";
    }else{
        *ret_type = -1;
        return "";
    }
}

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
// out ret type: 0 scalar, 1 vector, 2 matrix
char* convert_complex_expr(char *expr, int *ret_type){
    trim(expr);
    printf("Converting: %s\n", expr);
    
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
            return throw_error();
        }

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
                        break;
                    case 1:
                        if(((int *)vars.matrix_dimensions.elements[index_left])[1] != ((int *)vars.vector_dimensions.elements[index_right])[0])
                            return throw_error();
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
                        break;
                    case 1:
                        if(((int **)vars.vector_dimensions.elements)[index_left][0] != ((int **)vars.vector_dimensions.elements)[index_right][0])
                            return throw_error();
                        break;
                    default:
                        break;
                }
                break;
            default:
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
    }else if(match_indexed_matrix(expr)){
        vector tokens = tokenize(expr, " [,]");
        // {var_name}.elements[{i}][{j}]
        char *ret = malloc(strlen(tokens.elements[0]) + 10 + strlen(tokens.elements[1]) + 2 + strlen(tokens.elements[2]) + 1);
        strcpy(ret, tokens.elements[0]);
        strcat(ret, ".elements[");
        strcat(ret, tokens.elements[1]);
        strcat(ret, "][");
        strcat(ret, tokens.elements[2]);
        strcat(ret, "]");
        *ret_type = 0;
        return ret;
    }else if(match_indexed_vector(expr)){
        vector tokens = tokenize(expr, " [,]");
        // {var_name}.elements[{i}]
        char *ret = malloc(strlen(tokens.elements[0]) + 10 + strlen(tokens.elements[1]) + 1);
        strcpy(ret, tokens.elements[0]);
        strcat(ret, ".elements[");
        strcat(ret, tokens.elements[1]);
        strcat(ret, "]");
        *ret_type = 0;
        return ret;
    }else if(match_var(expr)){
        int dummy = 0;
        char *type = get_var_type_and_index(expr, &dummy);
        *ret_type = type_to_int(type);
        return expr;
    }else if(match_num(expr)){
        *ret_type = 0;
        return expr;
    }else if(match_expr_op_expr(expr)){
        int level = 0;
        for(int i = 0; i < strlen(expr); i++){
            char c = expr[i];
            if(c == '(')
                ++level;
            else if(c == ')')
                --level;
            if(level == 0 && (c == '+' || c == '-' || c == '*')){
                char *left = malloc(i);
                char *right = malloc(strlen(expr) - i);
                strncpy(left, expr, i);
                strncpy(right, expr+i+1, strlen(expr)-i);

                int left_type = -1;
                char *left_in_c = malloc(255);
                strcpy(left_in_c, convert_complex_expr(left, &left_type));
                int right_type = -1;
                char *right_in_c = malloc(255);
                strcpy(right_in_c, convert_complex_expr(right, &right_type));
                int out_type = -1;
                bool in_reverse = false;
                char *func_name = malloc(8);
                strcpy(func_name, get_func_for_arithmetic_op(left_type, right_type, c, &out_type, &in_reverse));
                printf("func name: %s\n", get_func_for_arithmetic_op(left_type, right_type, c, &out_type, &in_reverse));
                *ret_type = out_type;
                char *ret = malloc(8 + 1 + strlen(left_in_c) + 2 + strlen(right_in_c) + 2);
                strcpy(ret, func_name);
                strcat(ret, "(");
                strcat(ret, left_in_c);
                strcat(ret, ", ");
                strcat(ret, right_in_c);
                strcat(ret, ")");
                // printf("%s", ret);
                // free(left);
                // free(right);
                return ret;
            }
        }
    }else if(match_expr_in_paran(expr)){
        char *exprr = malloc(strlen(expr)-2);
        strncpy(exprr, expr+1, strlen(expr)-2);
        int inside_type = 0;
        char *expr_in_c = convert_complex_expr(exprr, &inside_type);
        *ret_type = inside_type;
        char *ret = malloc(strlen(expr_in_c) + 2);
        strcpy(ret, "(");
        strcat(ret, expr_in_c);
        strcat(ret, ")");
        // free(exprr);
        // free(expr_in_c);
        return ret;
    }else if(match_func(expr)){
        char *first = strchr(expr, '(');
        char *last = strrchr(expr, ')');
        size_t first_index = first - expr+1;
        size_t last_index = last - expr-1;

        char *inside = malloc(last_index-first_index+1);
        strncpy(inside, first+1, last-first-1);
        int inside_type = 0;
        char *inside_in_c = convert_complex_expr(inside, &inside_type);
        *ret_type = inside_type;
        if(strncmp(expr, "tr(", 3) == 0){
            if(inside_type == 1)
                *ret_type = 2;
            if(inside_type == 0){
                return inside_in_c;
            }
        }
        if(strncmp(expr, "sqrt(", 5) == 0 && inside_type != 0)
            return throw_error();
        char *ret = malloc(first - expr + strlen(inside_in_c) + 1);
        strncpy(ret, expr, first-expr);
        strcat(ret, "(");
        strcat(ret, inside_in_c);
        printf("inside in c: %s\n", inside_in_c);
        strcat(ret, ")");
        printf("match_func: %s\n", ret);
        // strcat(ret, ")");
        // free(first);
        // free(last);
        // free(inside_in_c);
        // free(inside);
        return ret;
    }else if(match_choose(expr)){
        char *expr1_start = strchr(expr, '(')+1;
        char *expr1_close = get_inside_choose_expr_close(expr1_start);
        size_t expr1_size = expr1_close - expr1_start;
        char *expr1 = malloc(expr1_size);
        strncpy(expr1, expr1_start, expr1_size);

        int type = -1;
        char *expr1_in_c = convert_complex_expr(expr1, &type);
        if(type != 0)
            return throw_error();


        char *expr2_close = get_inside_choose_expr_close(expr1_close+1);
        size_t expr2_size = expr2_close - expr1_close-1;
        char *expr2 = malloc(expr2_size);
        strncpy(expr2, expr1_close+1, expr2_size);

        type = -1;
        char *expr2_in_c = convert_complex_expr(expr2, &type);
        if(type != 0)
            return throw_error();


        char *expr3_close = get_inside_choose_expr_close(expr2_close+1);
        size_t expr3_size = expr3_close - expr2_close-1;
        char *expr3 = malloc(expr3_size);
        strncpy(expr3, expr2_close+1, expr3_size);

        type = -1;
        char *expr3_in_c = convert_complex_expr(expr3, &type);
        if(type != 0)
            return throw_error();

        
        char *expr4_close = strrchr(expr3_close, ')')-1;
        size_t expr4_size = expr4_close - expr3_close;
        char *expr4 = malloc(expr4_size);
        strncpy(expr4, expr3_close+1, expr4_size);

        type = -1;
        char *expr4_in_c = convert_complex_expr(expr4, &type);
        if(type != 0)
            return throw_error();

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
        return ret;
    }
    printf("cannot match: %s\n", expr);
    return "not imp of complex expr";
}

#endif