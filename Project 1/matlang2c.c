#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h" 
#include "vector.h"
#include "math.h"
// authors: Esad Yusuf Atik, Orkun Mahir Kılıç
// matlang to c converter
// we are just parsing from matlang file and writing to c file line by line

/*
KURALLAR
    <var_name> -> alfanumerik kurallar klasik c kuralları
                    <var_name>[<int>]
                    <var_name>[<int>,<int>]
    <operator> -> (* + -)
    <int_list> -> <int> | <int> <int_list> // [ ] ları unutma
    DEFINITION KURALLARI
        scalar <var_name>
        vector <var_name>[<vector_length>]
        matrix <var_name>[<col_length>,<row_length>]

    ASSIGNMENT KURALLARI
        assign scalar ->    <var_name> = <int>
                                        <expression>
        assign vector ->    <var_name> = <int_list>
                                        <expression
        assign matrix ->    <var_name> = <int_list>
                                        <expression>
    EXPRESSION KURALLARI
        expression  ->  <var_name>
                        <int>
                        <func <expression>>
                        <int> <operator> <expression>
                        <var_name> <operator> <expression>
                        <expression> <operator> <expression>
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
vector tokenize(char *str, char *delimeter){
    vector tokens = vec_make_vector();
    char *token = strtok(strdup(str), delimeter);
    while(token != NULL){
        vec_str_append(&tokens, token);
        token = strtok(NULL, delimeter);
    }
    return tokens;
}

char* make_scalar(char *line) {
    vector tokens = tokenize(line, " \n\t");
    if(tokens.size != 2){
        return throw_error();
    }

    char first_char = ((char**)tokens.elements)[1][0];
    // check if the var name is in line with c var name rules
    // TODO: check if not alphanumeric characters exist in var name;
    if(!(first_char <= 122 && first_char >= 65 || first_char == '_')){
        return throw_error();
    }
    vec_str_append(&vars.scalar_names, tokens.elements[1]);

    //   ??               "void *" + tokens.elements[1] + " = calloc(1, sizeof(int));" + "\0"
    //                  "int " + tokens.elements[1] + " = 0;" + "\0"
    char *ret_line = malloc(6 + sizeof(char) * strlen(tokens.elements[1]) + 27);
    strcpy(ret_line, "void *");
    strcat(ret_line, tokens.elements[1]);
    strcat(ret_line, " = calloc(1, sizeof(int));\0");
    return ret_line;
}

// TODO: boyut işi 
char* make_vector(char *line) {
    vector var_tokens = tokenize(line, " []\n\t");

    if(var_tokens.size != 3){
        return throw_error();
    }

    char *var_name = var_tokens.elements[1];
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

char* convert_expression(char *expr){
    return "not_imp";
}

char* make_assignment(char *line){
    if(strchr(strdup(line), '{') != NULL){
        // can only be matrix or vector init.
        vector tokens = tokenize(line, " ={}\n\t");
        size_t index;
        vector float_test_tokens = tokenize(line, ". ={}\n\t");
        char *caster = malloc(strlen("(float *)") + 1);
        strcpy(caster, "(int *)");

        if(tokens.size != float_test_tokens.size){

            // type should be float
            strcpy(caster, "(float *)");
        }

        index = vec_str_find(vars.vector_names, tokens.elements[0]);
        if(index != -1){
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
                char *op = malloc(1 + strlen(caster) + strlen(tokens.elements[0]) + 11 +strlen(i_str) + 4 + strlen(value)+2);
                strcpy(op, "(");
                strcat(op, caster);
                strcat(op, tokens.elements[0]);
                strcat(op, ".elements)[");
                strcat(op, i_str);
                strcat(op, "] = ");
                strcat(op, value);
                strcat(op, ";\0");
                vec_str_append(&set_ops, op);
                free(op);
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
        }else if((index = vec_str_find(vars.matrix_names, tokens.elements[0])) != -1){
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
                                    //({caster}({var_name}.elements[0]))[0] = 23..;
                    char *op = malloc(1 + strlen(caster) + 1 +strlen(tokens.elements[0]) + 10 +strlen(i_str) + 4 + strlen(j_str)+4+strlen(value)+2);
                    strcpy(op, "(");
                    strcat(op, caster);
                    strcat(op, "(");
                    strcat(op, tokens.elements[0]);
                    strcat(op, ".elements)[");
                    strcat(op, i_str);
                    strcat(op, "]))[");
                    strcat(op, j_str);
                    strcat(op, "] = ");
                    strcat(op, value);
                    strcat(op, ";\0");
                    vec_str_append(&set_ops, op);
                    free(op);
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
        // either basic scalar assignment or complex expressions involved
        return "not imp of assig";
    }

    return "not imp of assignment";
}



char* convert_line(char *line){
    vector tokens = tokenize(line, " \n\t");

    if(tokens.size == 0){
        vec_str_append(&tokens, line);
    }

    if(strcmp(tokens.elements[0], "scalar") == 0){
        return make_scalar(line);
    }else if(strcmp(tokens.elements[0], "vector") == 0){
        return make_vector(line);
    }else if(strcmp(tokens.elements[0], "matrix") == 0){
        return make_matrix(line);
    }else if(strcmp(tokens.elements[1], "=") == 0){
        return make_assignment(line);
    }else if(strncmp(line, "#", sizeof(char)) == 0){
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
        char *line_in_c = convert_line(lines.elements[i]);

        if(strcmp(line_in_c, "error") == 0){
            // error in line bla bla
        }
        printf("%s\n", line_in_c);

    }
    printf("[%d,%d]", ((int **)vars.matrix_dimensions.elements)[0][0],((int **)vars.matrix_dimensions.elements)[0][1]);
    //  Free the allocated line buffer
    free(line_buf);
    line_buf = NULL;

    //  Close the file now that we are done with it
    fclose(matFile1);

    return 1;
}