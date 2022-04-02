#ifndef MAKERS_H
#define MAKERS_H

#include "vector.h"
#include "matchers.h"
#include "utils.h"
#include "vars.h"
#include "expressions.h"


char* make_scalar(char *line) {
    vector tokens = tokenize(line, " \n\t");
    if(tokens.size != 2){
        return throw_error();
    }

    // check if the var name is in line with c var name rules
    // TODO: check if not alphanumeric characters exist in var name;
    if(!proper_varname_check(tokens.elements[1])){
        return throw_error();
    }
    int dummy = -1;
    get_var_type_and_index(tokens.elements[1], &dummy);
    // TODO: check if this works
    if(dummy != -1){
        // var redeclared
        return throw_error();
    }

    vec_str_append(&vars.scalar_names, tokens.elements[1]);

    //   ??               "void *" + tokens.elements[1] + " = calloc(1, sizeof(int));" + "\0"
    //                  "float " + tokens.elements[1] + " = 0;" + "\0"
    char *ret_line = malloc(6 + strlen(tokens.elements[1]) + 6);
    strcpy(ret_line, "float ");
    strcat(ret_line, tokens.elements[1]);
    strcat(ret_line, " = 0;\0");
    return ret_line;
}

char* make_vector(char *line) {
    vector var_tokens = tokenize(line, " []\n\t");

    if(var_tokens.size != 3){
        return throw_error();
    }

    char *var_name = var_tokens.elements[1];
    if(!proper_varname_check(var_name)){
        return throw_error();
    }

    int dummy = -1;
    get_var_type_and_index(var_name, &dummy);
    // TODO: check if this works
    if(dummy != -1){
        // var redeclared
        return throw_error();
    }

    size_t var_dimensions = atoi(var_tokens.elements[2]);

    vec_str_append(&vars.vector_names, var_name);
    int dimension[] = {var_dimensions};
    vec_append(&vars.vector_dimensions, &dimension, sizeof(int));

    //                 out_vector {var_name} = make_out_vector({var_dimension});\0
    //                 int {var_name}[{var_dimension}] = {0};\0
    char *ret_line = malloc(strlen("out_vector ") + strlen(var_name) + strlen(" = make_out_vector(") + strlen(var_tokens.elements[2]) + strlen(");") + 1);
    strcpy(ret_line, "out_vector ");
    strcat(ret_line, var_name);
    strcat(ret_line, " = make_out_vector(");
    strcat(ret_line, var_tokens.elements[2]);
    strcat(ret_line, ");\0");
    return ret_line;
}

char* make_matrix(char *line) {
    vector var_tokens = tokenize(line, " [,]\n\t");

    if(var_tokens.size != 4){
        return throw_error();
    }

    char *var_name = var_tokens.elements[1];
    if(!proper_varname_check(var_name)){
        return throw_error();
    }

    int dummy = -1;
    get_var_type_and_index(var_name, &dummy);
    // TODO: check if this works
    if(dummy != -1){
        // var redeclared
        return throw_error();
    }

    size_t var_row_dim = atoi(var_tokens.elements[2]);
    size_t var_col_dim = atoi(var_tokens.elements[3]);

    vec_str_append(&vars.matrix_names, var_name);
    int dimension[] = {var_row_dim, var_col_dim};
    vec_append(&vars.matrix_dimensions, &dimension, sizeof(int)*2);

    //          out_matrix {var_name} = make_out_matrix({rows}, {cols});\0
    //          int {var_name}[{var_row_dim}][var_col_dim] = {0};\0
    char *ret_line = malloc(strlen("out_matrix ") + strlen(var_name) + strlen(" = make_out_matrix(") + strlen(var_tokens.elements[2]) + 2 + strlen(var_tokens.elements[3]) + strlen(");") + 1);
    strcpy(ret_line, "out_matrix ");
    strcat(ret_line, var_name);
    strcat(ret_line, " = make_out_matrix(");
    strcat(ret_line, var_tokens.elements[2]);
    strcat(ret_line, ", ");
    strcat(ret_line, var_tokens.elements[3]);
    strcat(ret_line, ");\0");
    return ret_line;
}

char* make_assignment(char *line){
    if(strchr(strdup(line), '{')){
        // can only be matrix or vector init.
        vector tokens = tokenize(line, " ={}\n\t");
        int index = 0;
        vector float_test_tokens = tokenize(line, ". ={}\n\t");
        // char *caster;

        // if(tokens.size != float_test_tokens.size){
        //     caster = "(float *)";
        // }else{
        //     caster = "(int *)";
        // }
        char *type = get_var_type_and_index(tokens.elements[0], &index);
        if(strcmp(type, "vector") == 0){
            // found in vectors
            if(((int **)vars.vector_dimensions.elements)[index][0] != tokens.size-1){
                return throw_error();
            }
            vector set_ops = vec_make_vector();
            for(int i = 0; i < tokens.size-1; i++){

                char i_str[(int)((ceil(log10(i+1))+1)*sizeof(char))];
                sprintf(i_str, "%d", i);

                char *value = tokens.elements[i+1];
                                 //({caster}x.elements)[0] = 2.3;
                                 // {var_name}.elements[{i}] = {value};
                char *op = malloc(strlen(tokens.elements[0]) + 10 +strlen(i_str) + 4 + strlen(value)+2);
                strcpy(op, tokens.elements[0]);
                strcat(op, ".elements[");
                strcat(op, i_str);
                strcat(op, "] = ");
                strcat(op, value);
                strcat(op, ";\0");
                vec_str_append(&set_ops, op);
                // free(op);
            }

            size_t len = 0;
            for (int i = 0; i < set_ops.size; i++){
                len += strlen(set_ops.elements[i]);
            }
            char *ret_line = malloc(len + 1);
            strcpy(ret_line, set_ops.elements[0]);
            for (int i = 1; i < set_ops.size; i++){
                strcat(ret_line, set_ops.elements[i]);
            }
            return ret_line;
        }else if(strcmp(type, "matrix") == 0){
            // found in matrices;
            size_t rows = ((int**)vars.matrix_dimensions.elements)[index][0];
            size_t cols = ((int**)vars.matrix_dimensions.elements)[index][1];
            if(rows * cols != tokens.size - 1){
                return throw_error();
            }

            vector set_ops = vec_make_vector();
            for(int i = 0; i < rows; i++){
                for(int j = 0; j < cols; j++){
                    char i_str[(int)((ceil(log10(i+1))+1)*sizeof(char))];
                    char j_str[(int)((ceil(log10(j+1))+1)*sizeof(char))];

                    sprintf(i_str, "%d", i);
                    sprintf(j_str, "%d", j);

                    char *value = tokens.elements[1 + i*cols + j];
                                    // ({caster}({var_name}.elements[0]))[0] = 23..;
                                    // {var_name}.elements[{i}][{j}] = {value};
                    char *op = malloc(strlen(tokens.elements[0]) + 10 +strlen(i_str) + 2 + strlen(j_str)+4+strlen(value)+2);
                    strcpy(op, tokens.elements[0]);
                    strcat(op, ".elements[");
                    strcat(op, i_str);
                    strcat(op, "][");
                    strcat(op, j_str);
                    strcat(op, "] = ");
                    strcat(op, value);
                    strcat(op, ";\0");
                    vec_str_append(&set_ops, op);
                    // free(op);
                }
            }
            size_t len = 0;
            for (int i = 0; i < set_ops.size; i++){
                len += strlen(set_ops.elements[i]);
            }
            char *ret_line = malloc(len + 1);
            strcpy(ret_line, set_ops.elements[0]);
            for (int i = 1; i < set_ops.size; i++){
                strcat(ret_line, set_ops.elements[i]);
            }
            return ret_line;
        }else{
            return throw_error();
        }
    }else{
        char *rhs = strstr(line, "=") + 1; // right hand side of assignment
        size_t size = rhs-line-1;
        char *lhs = malloc(size);
        strncpy(lhs, line, size);
        int left_type = -1;
        int left_rows = -1;
        int left_cols = -1;
        char *lhs_in_c = convert_complex_expr(lhs, &left_type, &left_rows, &left_cols);

        int right_type = -1;
        int right_rows = -1;
        int right_cols = -1;
        char *expr_in_c = convert_complex_expr(rhs, &right_type, &right_rows, &right_cols);
        printf("%d %d %d %d\n", left_rows, left_cols, right_rows, right_cols);
        if(left_rows != right_rows || left_cols != right_cols){
            printf("here? %s\n", line);

            return throw_error();
        }
        printf("%s \n %d %d", line, left_type, right_type);
        // if (right_type != left_type){
        //     printf("cannot assing, expression doesnt return variable's type\n");
        //     return throw_error();
        // }
        // {var_name} = {expr_in_c};
        char *line_in_c = malloc(strlen(lhs_in_c) + 3 + strlen(expr_in_c) + 2);
        strcpy(line_in_c, lhs_in_c);
        strcat(line_in_c, " = ");
        strcat(line_in_c, expr_in_c);
        strcat(line_in_c, ";\0");
        // free(expr_in_c);git
        return line_in_c;
    }
}

char* make_print_sep_func(){
    return "print_sep();";
}

char* make_print_func(char *line){
    trim(line);
    // if we are here we have print(<expr>)
    int start = 6;
    int end = strlen(line)-2; // both inclusive for <expr>
    char *expr = malloc(end-start+1+1);
    memcpy(expr, line+start, end-start+1);
    strcat(expr, "\0");
    int type = -1;
    int dummy = -1;
    char *expr_in_c = convert_complex_expr(expr, &type, &dummy, &dummy);
    // my_print({expr_in_c});
    char *line_in_c = malloc(10 + strlen(expr)+3);
    if(type == 0)
        strcpy(line_in_c, "my_print_s(");
    else if(type == 1)
        strcpy(line_in_c, "my_print_v(");
    else if(type == 2)
        strcpy(line_in_c, "my_print_m(");
    strcat(line_in_c, expr_in_c);
    strcat(line_in_c, ");\0");
    // free(expr);
    return line_in_c;

}

char *make_for(char *line, bool *double_for){
    vector check_tokens = tokenize(line, ":");
    int dummy = -1;
    if(check_tokens.size == 3){
        *double_for = false;
        // single for
        /* find first (
            then get id
            get to in
            find first nonspace char
            expr till :
            expr till :
            expr till ) or space
        */

        char *first_paran = strchr(line, '(');
        char *in = strstr(line, "in");
        size_t id_size = in - first_paran - 1;
        char *var_name = malloc(id_size);
        strncpy(var_name, first_paran+1, id_size);
        trim(var_name);

        char *expr1_close = strchr(in+2, ':');
        size_t expr1_size = expr1_close - in - 3;
        char *expr1 = malloc(expr1_size);
        strncpy(expr1, in+3, expr1_size);
        trim(expr1);
        int type = -1;
        char *expr1_in_c = convert_complex_expr(expr1, &type, &dummy, &dummy);
        
        if(type != 0)
            return throw_error();

        char *expr2_close = strchr(expr1_close+1, ':');
        size_t expr2_size = expr2_close - expr1_close - 1;
        char *expr2 = malloc(expr2_size);
        strncpy(expr2, expr1_close + 1, expr2_size);
        trim(expr2);
        char *expr2_in_c = convert_complex_expr(expr2, &type, &dummy, &dummy);

        if(type != 0)
            return throw_error();

        char *expr3_close = strchr(expr2_close+1, ')');
        size_t expr3_size = expr3_close - expr2_close - 1;
        char *expr3 = malloc(expr3_size);
        strncpy(expr3, expr2_close + 1, expr3_size);
        trim(expr3);
        char *expr3_in_c = convert_complex_expr(expr3, &type, &dummy, &dummy);

        if(type != 0)
            return throw_error();
        
        // int {var_name} = {exp1};
        char *for_var_part = malloc(strlen(var_name) + 3 + strlen(expr1_in_c) + 2);
        strcpy(for_var_part, var_name);
        strcat(for_var_part, " = ");
        strcat(for_var_part, expr1_in_c);
        strcat(for_var_part, ";\0");

        // bound part
        // {var_name} < {expr2};
        char *bound_part = malloc(strlen(var_name) + 3 + strlen(expr2_in_c) + 2);
        strcpy(bound_part, var_name);
        strcat(bound_part, " < ");
        strcat(bound_part, expr2_in_c);
        strcat(bound_part, ";\0");

        // increment part
        // {var_name} += {expr3}
        char *incr_part = malloc(strlen(var_name) + 4 + strlen(expr3_in_c));
        strcpy(incr_part, var_name);
        strcat(incr_part, " += ");
        strcat(incr_part, expr3_in_c);
        strcat(incr_part, "\0");

        // combine
        // for({for_var_part} {bound part} {incr_part}) {
        char *combined = malloc(4 + strlen(for_var_part) + 5 + strlen(bound_part) + strlen(incr_part));
        strcpy(combined, "for(");
        strcat(combined, for_var_part);
        strcat(combined, " ");
        strcat(combined, bound_part);
        strcat(combined, " ");
        strcat(combined, incr_part);
        strcat(combined, ") {");

        // TODO: find whats wrong here;
        // free(var_name);free(expr1);free(expr1_in_c);free(expr2);free(expr2_in_c);
        // free(expr3);free(expr3_in_c);free(for_var_part);free(bound_part);free(incr_part);
        return combined;

    }else if(check_tokens.size == 5){
        *double_for = true;
        // double for
        /*
            get to first (
            get id1 till ,
            get id2 till in
            after in get expr1 till :
            get expr2 till :
            get expr3 till ,
            get expr4 till :
            get expr5 till :
            get expr6 till )
        */
        char *first_paran = strchr(line, '(');
        char *first_comma = strchr(line, ',');
        size_t var1_size = first_comma - first_paran - 1;
        char *var1 = malloc(var1_size);
        strncpy(var1, first_paran+1, var1_size);
        trim(var1);

        char *in = strstr(line, "in");
        size_t var2_size = in - first_comma - 1;
        char *var2 = malloc(var2_size);
        strncpy(var2, first_comma+1, var2_size);
        trim(var2);

        char *expr1_close = strchr(in+2, ':');
        size_t expr1_size = expr1_close - in -3;
        char *expr1 = malloc(expr1_size);
        strncpy(expr1, in+3, expr1_size);
        trim(expr1);
        int type = -1;
        char *expr1_in_c = convert_complex_expr(expr1, &type, &dummy, &dummy);

        if(type != 0)
            return throw_error();

        char *expr2_close = strchr(expr1_close+1, ':');
        size_t expr2_size = expr2_close - expr1_close - 1;
        char *expr2 = malloc(expr2_size);
        strncpy(expr2, expr1_close + 1, expr2_size);
        trim(expr2);
        char *expr2_in_c = convert_complex_expr(expr2, &type, &dummy, &dummy);

        if(type != 0)
            return throw_error();
        
        char *expr3_close = strchr(expr2_close+1, ',');
        size_t expr3_size = expr3_close - expr2_close - 1;
        char *expr3 = malloc(expr3_size);
        strncpy(expr3, expr2_close + 1, expr3_size);
        trim(expr3);
        char *expr3_in_c = convert_complex_expr(expr3, &type, &dummy, &dummy);

        if(type != 0)
            return throw_error();

        char *expr4_close = strchr(expr3_close+1, ':');
        size_t expr4_size = expr4_close - expr3_close - 1;
        char *expr4 = malloc(expr4_size);
        strncpy(expr4, expr3_close + 1, expr4_size);
        trim(expr4);
        char *expr4_in_c = convert_complex_expr(expr4, &type, &dummy, &dummy);

        if(type != 0)
            return throw_error();
        
        char *expr5_close = strchr(expr4_close+1, ':');
        size_t expr5_size = expr5_close - expr4_close - 1;
        char *expr5 = malloc(expr5_size);
        strncpy(expr5, expr4_close + 1, expr5_size);
        trim(expr5);
        char *expr5_in_c = convert_complex_expr(expr5, &type, &dummy, &dummy);

        if(type != 0)
            return throw_error();
        
        char *expr6_close = strchr(expr5_close+1, ')');
        size_t expr6_size = expr6_close - expr5_close - 1;
        char *expr6 = malloc(expr6_size);
        strncpy(expr6, expr5_close + 1, expr6_size);
        trim(expr6);
        char *expr6_in_c = convert_complex_expr(expr6, &type, &dummy, &dummy);

        if(type != 0)
            return throw_error();

         // int {var_name} = {exp1};
        char *for_var_part = malloc(strlen(var1) + 3 + strlen(expr1_in_c) + 2);
        strcpy(for_var_part, var1);
        strcat(for_var_part, " = ");
        strcat(for_var_part, expr1_in_c);
        strcat(for_var_part, ";\0");

        // bound part
        // {var_name} < {expr2};
        char *bound_part = malloc(strlen(var1) + 3 + strlen(expr2_in_c) + 2);
        strcpy(bound_part, var1);
        strcat(bound_part, " < ");
        strcat(bound_part, expr2_in_c);
        strcat(bound_part, ";\0");

        // increment part
        // {var_name} += {expr3}
        char *incr_part = malloc(strlen(var1) + 4 + strlen(expr3_in_c));
        strcpy(incr_part, var1);
        strcat(incr_part, " += ");
        strcat(incr_part, expr3_in_c);
        strcat(incr_part, "\0");

        // outside for
        // for({var1} {bounc_part} {incr_part}) {
        char *combined = malloc(4 + strlen(for_var_part) + 5 + strlen(bound_part) + strlen(incr_part));
        strcpy(combined, "for(");
        strcat(combined, for_var_part);
        strcat(combined, " ");
        strcat(combined, bound_part);
        strcat(combined, " ");
        strcat(combined, incr_part);
        strcat(combined, ") {");

         // int {var_name} = {exp4};
        char *for_var_part2 = malloc(4 + strlen(var2) + 3 + strlen(expr4_in_c) + 2);
        strcpy(for_var_part2, "int ");
        strcat(for_var_part2, var2);
        strcat(for_var_part2, " = ");
        strcat(for_var_part2, expr4_in_c);
        strcat(for_var_part2, ";\0");

        // bound part
        // {var_name} < {expr5};
        char *bound_part2 = malloc(strlen(var2) + 3 + strlen(expr5_in_c) + 2);
        strcpy(bound_part2, var2);
        strcat(bound_part2, " < ");
        strcat(bound_part2, expr5_in_c);
        strcat(bound_part2, ";\0");

        // increment part
        // {var_name} += {expr6}
        char *incr_part2 = malloc(strlen(var2) + 4 + strlen(expr6_in_c));
        strcpy(incr_part, var2);
        strcat(incr_part, " += ");
        strcat(incr_part, expr6_in_c);
        strcat(incr_part, "\0");

        // inside for
        // for({var2} {bound_part2} {incr_part2}) {
        char *combined2 = malloc(4 + strlen(for_var_part2) + 5 + strlen(bound_part2) + strlen(incr_part2));
        strcpy(combined2, "for(");
        strcat(combined2, for_var_part2);
        strcat(combined2, " ");
        strcat(combined2, bound_part2);
        strcat(combined2, " ");
        strcat(combined2, incr_part2);
        strcat(combined2, ") {");

        char *double_for = malloc(strlen(combined) + 1 + strlen(combined2));
        strcpy(double_for, combined);
        strcat(double_for, combined2);

        // TODO: free
        return double_for;
    }else{
        return throw_error();
    }
}

#endif