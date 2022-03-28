#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h" 
#include "vector.h"
#include "ctype.h"
#include "math.h"
// authors: Esad Yusuf Atik, Orkun Mahir Kılıç
// matlang to c converter
// we are just parsing from matlang file and writing to c file line by line

/*
KURALLAR
    <var_name> -> alfanumerik kurallar klasik c kuralları
                    <var_name>[<num>]
                    <var_name>[<num>,<num>]
    <operator> -> (* + -)
    <num_list> -> <num> | <num> <num_list> // [ ] ları unutma
    DEFINITION KURALLARI
        scalar <var_name>
        vector <var_name>[<vector_length>]
        matrix <var_name>[<col_length>,<row_length>]

    ASSIGNMENT KURALLARI
        <assign-scalar> ->  scalar  <var_name> = (<num>|<expression>)
        <assign-vector> ->  vector  <var_name> = (<num_list>|<expression>)
        <assign-matrix> ->  matrix  <var_name> = (<num_list>|<expression)
    EXPRESSION KURALLARI
        expression  ->  <var_name>
                        <num>
                        <func <expression>>
                        <expression> <operator> <expression>
                        (<expression>)
    FUNCTION KURALLAR
        <func>  -> <tr> | <sqrt> | <choose> | <print> | <print_sep>
        <tr>    ->  tr(<expression>)

        <sqrt>  ->  sqrt(<expression>)
        
        <choose> -> choose(<expression>,<expression>,<expression>,<expression>)

        <print> ->  print(<var_name>)
        
        <print_sep> -> print_sep()
    FOR KURALLARI
        <for> -> for(<var_name> in <expression: <expression>: <expression>) -> from exp1 to exp2 by adding exp3

                 for(<var_name>, <var_name> in <expression>: <expression>: <expression>, <expression>: <expression>: <expression>)

*/

// TODO: implement proper var name function

static struct Variables {
    vector scalar_names;

    vector vector_names;
    vector vector_dimensions;

    vector matrix_names;
    vector matrix_dimensions;
} vars;

char* throw_error(){
    return "error";
}

char *trim(char *str){
    size_t len = 0;
    char *frontp = str;
    char *endp = NULL;

    if( str == NULL ) { return NULL; }
    if( str[0] == '\0' ) { return str; }

    len = strlen(str);
    endp = str + len;

    /* Move the front and back pointers to address the first non-whitespace
     * characters from each end.
     */
    while( isspace((unsigned char) *frontp) ) { ++frontp; }
    if( endp != frontp )
    {
        while( isspace((unsigned char) *(--endp)) && endp != frontp ) {}
    }

    if( frontp != str && endp == frontp )
            *str = '\0';
    else if( str + len - 1 != endp )
            *(endp + 1) = '\0';

    /* Shift the string so that it starts at str so that if it's dynamically
     * allocated, we can still free it on the returned pointer.  Note the reuse
     * of endp to mean the front of the string buffer now.
     */
    endp = str;
    if( frontp != str )
    {
            while( *frontp ) { *endp++ = *frontp++; }
            *endp = '\0';
    }

    return str;
}

bool prefix(const char *str, const char *pre){
    return strncmp(pre, str, strlen(pre)) == 0;
}

vector tokenize(char *str, char *delimeter){
    vector tokens = vec_make_vector();
    char *token = strtok(strdup(str), delimeter);
    while(token != NULL){
        vec_str_append(&tokens, token);
        token = strtok(NULL, delimeter);
    }
    return tokens;
}

char* get_var_type_and_index(char *var_name, int *index_out){
    size_t index = 0;
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
    // if(dot_count == 1){
    //     *caster = "(float *)";
    // }else{
    //     *caster = "(int *)";
    // }
    return true;
}

bool proper_varname_check(char *name){
    char first_char = name[0];
    if(!((first_char <= 122 && first_char >= 65) || first_char == '_')){
        return false;
    }
    for(int i = 1; i < strlen(name); i++){
        char c = name[i];
        if( (c <= 122 && c >= 65) || c == '_' || c == '-'){
            continue;
        }else{
            return false;
        }
    }
    return true;
}

bool match_indexed_matrix(char *expr){
    trim(expr);
    vector tokens = tokenize(expr, " [,]\t");
    if(tokens.size == 3){
        int dummy = 0;
        char *type = get_var_type_and_index(tokens.elements[0], &dummy);
        return strcmp(type, "matrix") == 0 && 
        (proper_number_check(tokens.elements[1]) || strcmp("scalar", get_var_type_and_index(tokens.elements[1], &dummy))
        && (proper_number_check(tokens.elements[2])) || strcmp("scalar", get_var_type_and_index(tokens.elements[2], &dummy)));
    }else{
        return false;
    }
}

bool match_indexed_vector(char *expr){
    trim(expr);
    vector tokens = tokenize(expr, " []\t");
    if(tokens.size == 2){
        int dummy = 0;
        char *type = get_var_type_and_index(tokens.elements[0], &dummy);
        return strcmp(type, "vector") == 0 && 
        (proper_number_check(tokens.elements[1]) || strcmp("scalar", get_var_type_and_index(tokens.elements[1], &dummy))
        && (proper_number_check(tokens.elements[2])) || strcmp("scalar", get_var_type_and_index(tokens.elements[2], &dummy)));
    }else{
        return false;
    }
}

bool match_m_m_mult(char *expr){
    int dummy = 0;
    vector tokens = tokenize(expr, " *\n\t");
    if (tokens.size == 2){
        if(strcmp("matrix", get_var_type_and_index(tokens.elements[0], &dummy)) == 0 && strcmp("matrix", get_var_type_and_index(tokens.elements[1], &dummy)) == 0){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool match_m_m_sum(char *expr){
    int dummy = 0;
    vector tokens = tokenize(expr, " +\n\t");
    if (tokens.size == 2){
        if(strcmp("matrix", get_var_type_and_index(tokens.elements[0], &dummy)) == 0 && strcmp("matrix", get_var_type_and_index(tokens.elements[1], &dummy)) == 0){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool match_m_v_sum(char *expr){
    int dummy = 0;
    vector tokens = tokenize(expr, " +\n\t");
    if (tokens.size == 2){
        if((strcmp("matrix", get_var_type_and_index(tokens.elements[0], &dummy)) == 0 && strcmp("vector", get_var_type_and_index(tokens.elements[1], &dummy)) == 0) || (strcmp("vector", get_var_type_and_index(tokens.elements[0], &dummy)) == 0 && strcmp("matrix", get_var_type_and_index(tokens.elements[1], &dummy)) == 0)){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool match_m_v_mult(char *expr){
    int dummy = 0;
    vector tokens = tokenize(expr, " *\n\t");
    if (tokens.size == 2){
        if((strcmp("matrix", get_var_type_and_index(tokens.elements[0], &dummy)) == 0 && strcmp("vector", get_var_type_and_index(tokens.elements[1], &dummy)) == 0) || (strcmp("vector", get_var_type_and_index(tokens.elements[0], &dummy)) == 0 && strcmp("matrix", get_var_type_and_index(tokens.elements[1], &dummy)) == 0)){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool match_m_s_mult(char *expr){
    int dummy = 0;
    vector tokens = tokenize(expr, " *\n\t");
    if (tokens.size == 2){
        if((strcmp("matrix", get_var_type_and_index(tokens.elements[0], &dummy)) == 0 && strcmp("scalar", get_var_type_and_index(tokens.elements[1], &dummy)) == 0) || (strcmp("scalar", get_var_type_and_index(tokens.elements[0], &dummy)) == 0 && strcmp("matrix", get_var_type_and_index(tokens.elements[1], &dummy)) == 0)){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool match_v_v_sum(char *expr){
    int dummy = 0;
    vector tokens = tokenize(expr, " +\n\t");
    if (tokens.size == 2){
        if((strcmp("vector", get_var_type_and_index(tokens.elements[0], &dummy)) == 0 && strcmp("vector", get_var_type_and_index(tokens.elements[1], &dummy)) == 0) || (strcmp("vector", get_var_type_and_index(tokens.elements[0], &dummy)) == 0 && strcmp("vector", get_var_type_and_index(tokens.elements[1], &dummy)) == 0)){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool match_v_s_mult(char *expr){
    int dummy = 0;
    vector tokens = tokenize(expr, " +\n\t");
    if (tokens.size == 2){
        if((strcmp("scalar", get_var_type_and_index(tokens.elements[0], &dummy)) == 0 && strcmp("scalar", get_var_type_and_index(tokens.elements[1], &dummy)) == 0) || (strcmp("scalar", get_var_type_and_index(tokens.elements[0], &dummy)) == 0 && strcmp("scalar", get_var_type_and_index(tokens.elements[1], &dummy)) == 0)){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool match_var(char *expr){
    int index = 0;
    get_var_type_and_index(expr, &index);
    return index != -1;
}

bool match_num(char *expr){
    return proper_number_check(expr);
}

bool match_func(char *expr){
    if(prefix(expr, "tr(") || prefix(expr, "sqrt(")){
        int level = 0;
        for (int i = 0; i < strlen(expr); i++){
            char c = expr[i];
            if(c == '(')
                level++;
            else if(level == ')')
                level++;
            
            if(level == 0 && i != strlen(expr)-1)
                return false;
        }
        return true;
    }else{
        return false;
    }
}

bool match_choose(char *expr){
    if(prefix(expr, "choose(")){
        int level = 0;
        for (int i = 0; i < strlen(expr); i++){
            char c = expr[i];
            if(c == '(')
                level++;
            else if(level == ')')
                level++;
            
            if(level == 0 && i != strlen(expr)-1)
                return false;
        }
        return true;
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
            else if(level == ')')
                level++;
            
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

// sets out_expected type to 0 if type of expr is int, 1 if it's float
char* convert_complex_expr(char *expr){
    trim(expr);
    printf("Converting: %s\n", expr);
    if(match_m_m_mult(expr)){
        vector tokens = tokenize(expr, " *");
        int index1 = 0;
        int index2 = 0;
        get_var_type_and_index(tokens.elements[0], &index1);
        get_var_type_and_index(tokens.elements[1], &index2);

        //check if dimensions match
        if(((int**)vars.matrix_dimensions.elements)[index1][1] != ((int**)vars.matrix_dimensions.elements)[index2][0]){
            return throw_error();
        }

        // m_m_mult({var1}, {var2})
        char *ret = malloc(9 + strlen(tokens.elements[0]) + 2 + strlen(tokens.elements[1]) + 2);
        strcpy(ret, "m_m_mult(");
        strcat(ret, tokens.elements[0]);
        strcat(ret, ", ");
        strcat(ret, tokens.elements[1]);
        strcat(ret, ")\0");
        return ret;
    }else if(match_m_v_mult(expr)){
        vector tokens = tokenize(expr, " *");
        int index1 = 0;
        int index2 = 0;
        char *type_left = get_var_type_and_index(tokens.elements[0], &index1);
        get_var_type_and_index(tokens.elements[1], &index2);

        // m_v_mult({matrixvar}, {vectorvar})
        char *ret = malloc(9 + strlen(tokens.elements[0]) + 2 + strlen(tokens.elements[1]) + 2);
        strcpy(ret, "m_v_mult(");
        if(strcmp(type_left, "matrix") == 0){
            // matrix * vector
            if(((int**)vars.matrix_dimensions.elements)[index1][1] != ((int**)vars.vector_dimensions.elements)[index2][0]){
                return throw_error();
            }
            strcat(ret, tokens.elements[0]);
            strcat(ret, ", ");
            strcat(ret, tokens.elements[1]);
        }else{
            // vector * matrix
            if(1 != ((int**)vars.vector_dimensions.elements)[index2][0]){
                return throw_error();
            }
            strcat(ret, tokens.elements[1]);
            strcat(ret, ", ");
            strcat(ret, tokens.elements[0]);
        }
        strcat(ret, ")\0");
        return ret;
    }else if(match_m_s_mult(expr)){
        vector tokens = tokenize(expr, " *");
        int index1 = 0;
        int index2 = 0;
        char *type_left = get_var_type_and_index(tokens.elements[0], &index1);
        get_var_type_and_index(tokens.elements[1], &index2);
        // m_s_mult({matrix}, {scalar})
        char *ret = malloc(9 + strlen(tokens.elements[0]) + 3 +strlen(tokens.elements[1]) + 2);
        strcpy(ret, "m_s_mult(");
        if(strcmp(type_left, "matrix") == 0){
            // matrix * scalar
            strcat(ret, tokens.elements[0]);
            strcat(ret, ", ");
            strcat(ret, tokens.elements[1]);
            
        }else{
            // scalar * matrix
            strcat(ret, tokens.elements[1]);
            strcat(ret, ", ");
            strcat(ret, tokens.elements[0]);
        }
        strcat(ret, ")\0");
        return ret;
    }else if(match_v_s_mult(expr)){
        vector tokens = tokenize(expr, " *");
        int index1 = 0;
        int index2 = 0;
        char *type_left = get_var_type_and_index(tokens.elements[0], &index1);
        get_var_type_and_index(tokens.elements[1], &index2);
        // v_s_mult({matrix}, {scalar})
        char *ret = malloc(9 + strlen(tokens.elements[0]) + 3 +strlen(tokens.elements[1]) + 2);
        strcpy(ret, "v_s_mult(");
        if(strcmp(type_left, "vector") == 0){
            // matrix * scalar
            strcat(ret, tokens.elements[0]);
            strcat(ret, ", ");
            strcat(ret, tokens.elements[1]);
            
        }else{
            // scalar * matrix
            strcat(ret, tokens.elements[1]);
            strcat(ret, ", ");
            strcat(ret, tokens.elements[0]);
        }
        strcat(ret, ")\0");
        return ret;
    }else if(match_m_m_sum(expr)){
        vector tokens = tokenize(expr, " +");
        int index1 = 0;
        int index2 = 0;
        get_var_type_and_index(tokens.elements[0], &index1);
        get_var_type_and_index(tokens.elements[1], &index2);

        //check if dimensions match
        if((((int**)vars.matrix_dimensions.elements)[index1][0] != ((int**)vars.matrix_dimensions.elements)[index2][0]) || (((int**)vars.matrix_dimensions.elements)[index1][1] != ((int**)vars.matrix_dimensions.elements)[index2][1])){
            return throw_error();
        }

        // m_m_sum({var1}, {var2})
        char *ret = malloc(1 + strlen(tokens.elements[0]) + 2 + strlen(tokens.elements[1]) + 2);
        strcpy(ret, "m_m_sum(");
        strcat(ret, tokens.elements[0]);
        strcat(ret, ", ");
        strcat(ret, tokens.elements[1]);
        strcat(ret, ")\0");
        return ret;
    }else if(match_m_v_sum(expr)){
        vector tokens = tokenize(expr, " +");
        int index1 = 0;
        int index2 = 0;
        char *type_left = get_var_type_and_index(tokens.elements[0], &index1);
        get_var_type_and_index(tokens.elements[1], &index2);

        // m_v_mult({matrixvar}, {vectorvar})
        char *ret = malloc(8 + strlen(tokens.elements[0]) + 2 + strlen(tokens.elements[1]) + 2);
        strcpy(ret, "m_v_sum(");
        if(strcmp(type_left, "matrix") == 0){
            // matrix * vector
            if((((int**)vars.matrix_dimensions.elements)[index1][0] != ((int**)vars.vector_dimensions.elements)[index2][0]) || (((int**)vars.matrix_dimensions.elements)[index1][0] != 1)){
                return throw_error();
            }
            strcat(ret, tokens.elements[0]);
            strcat(ret, ", ");
            strcat(ret, tokens.elements[1]);
        }else{
            // vector * matrix
            if(1 != ((int**)vars.vector_dimensions.elements)[index2][0] || (((int**)vars.matrix_dimensions.elements)[index2][0] != ((int**)vars.vector_dimensions.elements)[index1][0])){
                return throw_error();
            }
            strcat(ret, tokens.elements[1]);
            strcat(ret, ", ");
            strcat(ret, tokens.elements[0]);
        }
        strcat(ret, ")\0");
        return ret;
    }else if(match_v_v_sum(expr)){
        vector tokens = tokenize(expr, " +");
        int index1 = 0;
        int index2 = 0;
        get_var_type_and_index(tokens.elements[0], &index1);
        get_var_type_and_index(tokens.elements[1], &index2);

        //check if dimensions match
        if(((int**)vars.matrix_dimensions.elements)[index1][0] != ((int**)vars.matrix_dimensions.elements)[index2][0]){
            return throw_error();
        }

        // v_v_sum({var1}, {var2})
        char *ret = malloc(1 + strlen(tokens.elements[0]) + 2 + strlen(tokens.elements[1]) + 2);
        strcpy(ret, "v_v_sum(");
        strcat(ret, tokens.elements[0]);
        strcat(ret, ", ");
        strcat(ret, tokens.elements[1]);
        strcat(ret, ")\0");
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
        return ret;
    }else if(match_indexed_vector(expr)){
        vector tokens = tokenize(expr, " [,]");
        // {var_name}.elements[{i}]
        char *ret = malloc(strlen(tokens.elements[0]) + 10 + strlen(tokens.elements[1]) + 1);
        strcpy(ret, tokens.elements[0]);
        strcat(ret, ".elements[");
        strcat(ret, tokens.elements[1]);
        strcat(ret, "]");
        return ret;
    }else if(match_var(expr)){
        return expr;
    }else if(match_num(expr)){
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
                char *left = malloc(255);
                char *right = malloc(255);
                strncpy(left, expr, i);
                strncpy(right, expr+i+1, strlen(expr)-i);
                left = convert_complex_expr(strdup(left));
                right = convert_complex_expr(strdup(right));
                char *ret = malloc(strlen(left) + 3 + strlen(right));
                strcpy(ret, left);
                strcpy(ret, &c);
                strcpy(ret, right);
                // printf("%s", ret);
                // free(left);
                // free(right);
                return ret;
            }
        }
    }else if(match_expr_in_paran(expr)){
        char *exprr = malloc(strlen(expr)-2);
        strncpy(exprr, expr+1, strlen(expr)-2);
        char *expr_in_c = convert_complex_expr(exprr);
        char *ret = malloc(strlen(expr_in_c) + 2);
        strcpy(ret, "(");
        strcat(ret, expr_in_c);
        strcat(ret, ")");
        // free(exprr);
        // free(expr_in_c);
        return ret;
    }else if(match_func(expr)){
        char *first = strchr(expr, '(');
        char *last = strchr(expr, ')');
        size_t first_index = first - expr;
        size_t last_index = last - expr;

        char *inside = malloc(last-first-1);
        strncpy(inside, first+1, last-first-1);
        char *inside_in_c = convert_complex_expr(inside);
        char *ret = malloc(first - expr + strlen(inside_in_c) + 1);
        strncpy(ret, expr, first-expr-1);
        strcat(ret, inside_in_c);
        strcat(ret, ")");
        // free(first);
        // free(last);
        // free(inside_in_c);
        // free(inside);
        return ret;
    }

    return "not imp of complex expr";
}

char* make_assignment(char *line){
    if(strchr(strdup(line), '{') != NULL){
        // can only be matrix or vector init.
        vector tokens = tokenize(line, " ={}\n\t");
        size_t index = 0;
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
        // // either basic scalar assignment, vector/matrix change element or complex expressions involved
        // vector tokens = tokenize(line, " =[,]\n\t");
    
        // size_t index = 0;
        // char *type = get_var_type_and_index(tokens.elements[0], &index);
        
        // if (index == -1){
        //     return throw_error(); // variable doesn't exist
        // }
        /*if(tokens.size == 3 && strcmp(type, "vector") == 0){
            // char *caster;
            // if(proper_number_check(tokens.elements[2], &caster) == false){
            //     free(caster);
            //     return throw_error();
            // }
            //  ({caster}{var_name}.elements)[{index}] = {value};
            //  {var_name}.elements[{i}] = {value};
            char *line_in_c = malloc(strlen(tokens.elements[0]) + 10 + strlen(tokens.elements[1]) + 4 + strlen(tokens.elements[2]) + 2);
            strcpy(line_in_c, tokens.elements[0]);
            strcat(line_in_c, ".elements[");
            strcat(line_in_c, tokens.elements[1]);
            strcat(line_in_c, "] = ");
            strcat(line_in_c, tokens.elements[2]);
            strcat(line_in_c, ";\0");
            return line_in_c;
            // type might change
        }else if(tokens.size == 4 && strcmp(type, "matrix") == 0){
            // char *caster;
            // if(proper_number_check(tokens.elements[3], &caster) == false){
            //     free(caster);
            //     return throw_error();
            // }
            // ({caster}({var_name}.elements[{i}]))[{j}] = 1.23;
            //  {var_name}.elements[{i}][{j}] = {value};
            char *line_in_c = malloc(strlen(tokens.elements[0]) + 10 + strlen(tokens.elements[1]) + 2 + strlen(tokens.elements[2]) + 4 + strlen(tokens.elements[3]) + 2);
            strcpy(line_in_c, tokens.elements[0]);
            strcat(line_in_c, ".elements[");
            strcat(line_in_c, tokens.elements[1]);
            strcat(line_in_c, "][");
            strcat(line_in_c, tokens.elements[2]);
            strcat(line_in_c, "] = ");
            strcat(line_in_c, tokens.elements[3]);
            strcat(line_in_c, ";\0");
            return line_in_c;
            // type migth change
        }else if(tokens.size == 2 && strcmp(type, "scalar") == 0){
            
            // char *caster;
            // if(proper_number_check(tokens.elements[1], &caster) == false){
            //     free(caster);
            //     return throw_error();
            // }
            //  *{caster}{var_name} = {value};
            //  {var_name} = {value};
            char *line_in_c = malloc(strlen(tokens.elements[0]) + 3 + strlen(tokens.elements[1]) + 2);
            
            strcpy(line_in_c, tokens.elements[0]);
            strcat(line_in_c, " = ");
            strcat(line_in_c, tokens.elements[1]);
            strcat(line_in_c, ";\0");
            return line_in_c;
        }else{*/
        char *rhs = strstr(line, "=") + 1; // right hand side of assignment
        size_t size = rhs-line-1;
        char *lhs = malloc(size);
        strncpy(lhs, line, size);
        char *lhs_in_c = convert_complex_expr(lhs);
        char *expr_in_c = convert_complex_expr(rhs);
        // {var_name} = {expr_in_c};
        char *line_in_c = malloc(strlen(lhs_in_c) + 3 + strlen(expr_in_c) + 2);
        strcpy(line_in_c, lhs_in_c);
        strcat(line_in_c, " = ");
        strcat(line_in_c, expr_in_c);
        strcat(line_in_c, ";\0");
        // free(expr_in_c);
        return line_in_c;
        // }
    }
}

char* make_print_sep_func(){
    return "----------";
}

char* make_print_func(char *line){
    trim(line);
    // if we are here we have print(<expr>)
    int start = 6;
    int end = strlen(line)-2; // both inclusive for <expr>
    char *expr = malloc(end-start+1+1);
    memcpy(expr, line+start, end-start+1);
    strcat(expr, "\0");
    char *expr_in_c = convert_complex_expr(expr);
    // my_print({expr_in_c});
    char *line_in_c = malloc(9 + strlen(expr)+3);

    strcpy(line_in_c, "my_print(");
    strcat(line_in_c, expr_in_c);
    strcat(line_in_c, ");\0");
    // free(expr);
    return line_in_c;

}
char *make_for(char *line){
    vector check_tokens = tokenize(line, ":");
    if(check_tokens.size == 3){
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
        char *expr1_in_c = convert_complex_expr(expr1);

        char *expr2_close = strchr(expr1_close+1, ':');
        size_t expr2_size = expr2_close - expr1_close - 1;
        char *expr2 = malloc(expr2_size);
        strncpy(expr2, expr1_close + 1, expr2_size);
        trim(expr2);
        char *expr2_in_c = convert_complex_expr(expr2);

        char *expr3_close = strchr(expr2_close+1, ')');
        size_t expr3_size = expr3_close - expr2_close - 1;
        char *expr3 = malloc(expr3_size);
        strncpy(expr3, expr2_close + 1, expr3_size);
        trim(expr3);
        char *expr3_in_c = convert_complex_expr(expr3);

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

    }else if(check_tokens.size == 6){
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
        char expr1_in_c = convert_complex_expr(expr1);

        char *expr2_close = strchr(expr1_close+1, ':');
        size_t expr2_size = expr2_close - expr1_close - 1;
        char *expr2 = malloc(expr2_size);
        strncpy(expr2, expr1_close + 1, expr2_size);
        trim(expr2);
        char *expr2_in_c = convert_complex_expr(expr2);

        char *expr3_close = strchr(expr2_close+1, ',');
        size_t expr3_size = expr3_close - expr2_close - 1;
        char *expr3 = malloc(expr3_size);
        strncpy(expr3, expr2_close + 1, expr3_size);
        trim(expr3);
        char *expr3_in_c = convert_complex_expr(expr3);

        char *expr4_close = strchr(expr3_close+1, ':');
        size_t expr4_size = expr4_close - expr3_close - 1;
        char *expr4 = malloc(expr4_size);
        strncpy(expr4, expr3_close + 1, expr4_size);
        trim(expr4);
        char *expr4_in_c = convert_complex_expr(expr4);

        char *expr5_close = strchr(expr4_close+1, ':');
        size_t expr5_size = expr5_close - expr4_close - 1;
        char *expr5 = malloc(expr5_size);
        strncpy(expr5, expr4_close + 1, expr5_size);
        trim(expr5);
        char *expr5_in_c = convert_complex_expr(expr5);

        char *expr6_close = strchr(expr5_close+1, ')');
        size_t expr6_size = expr6_close - expr5_close - 1;
        char *expr6 = malloc(expr6_size);
        strncpy(expr6, expr5_close + 1, expr6_size);
        trim(expr6);
        char *expr6_in_c = convert_complex_expr(expr6);

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
        strcpy(double_for, combined2);

        // TODO: free
        return double_for;
    }else{
        return throw_error();
    }
}

char* convert_line(char *line){
    trim(line);
    vector tokens = tokenize(line, " \n\t");

    if(tokens.size == 0){
        vec_str_append(&tokens, line);
    }

    if(strstr(line, "print(")){
        return make_print_func(line);
    }else if(strstr(line, "printsep(")){
        return make_print_sep_func();
    }else if(prefix(line, "for") && strstr(line, ":") && strstr(line, "{")){
        return make_for(line);
    }else if (strcmp(line, "}") == 0){
        return line;
    }else if(strcmp(tokens.elements[0], "scalar") == 0){
        return make_scalar(line);
    }else if(strcmp(tokens.elements[0], "vector") == 0){
        return make_vector(line);
    }else if(strcmp(tokens.elements[0], "matrix") == 0){
        return make_matrix(line);
    }else if(strcmp(tokens.elements[1], "=") == 0){
        return make_assignment(line);
    }else if(strncmp(line, "#", 1) == 0){
       return "comment";
    }else{
        return "not imp";
    }
}

int main(int argc, char *argv[]){
    vars.scalar_names = vec_make_vector();
    vars.vector_names = vec_make_vector();
    vars.vector_dimensions = vec_make_vector();
    vars.matrix_names = vec_make_vector();
    vars.matrix_dimensions = vec_make_vector();

    FILE *matFile1;
    FILE *fp;
    fp = fopen("file.c", "w");
    fprintf(fp, "%s\n%s\n", "#include \"output_header.h\"", "int main(){");
    if(argc > 1){
        matFile1 = fopen(argv[1], "r");
        if(matFile1 == NULL){
            printf("File cannot be opened.");
            return 0;
        }
    }else{
        printf("File not given.");
        return 0;
    }

    char *line_buf = NULL;
    size_t line_buf_size = 0;
    ssize_t read;
    
    vector lines = vec_make_vector();
    // variable allocating
    while ((read = getline(&line_buf, &line_buf_size, matFile1)) != -1) {
        if(strcmp(line_buf, "\n") != 0)
            vec_str_append(&lines, line_buf);
    }

    for (int i = 0; i < lines.size; i++){
        printf("%s", lines.elements[i]);
        char *line_in_c = convert_line(lines.elements[i]);

        if(strcmp(line_in_c, "error") == 0 || strcmp(line_in_c, "comment") == 0 ){
            // error in line bla bla
        }else{
            fprintf(fp, "%s\n", line_in_c);
        }
    }
    fprintf(fp, "%s\n", "}");
    //  Free the allocated line buffer
    free(line_buf);
    line_buf = NULL;

    //  Close the file now that we are done with it
    fclose(matFile1);

    return 1;
}