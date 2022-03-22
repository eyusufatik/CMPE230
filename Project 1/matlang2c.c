#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h" 
#include "vector.h"
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
    char **scalar_names;
    int *scalar_values; // [1, 2, 33]
    int scalar_last_index;
    // bool *scalar_assigned; TODO: might implement later 

    char **vector_names;
    int **vector_values; // [[1,2,3],[4,5,6]]
    int vector_last_index;

    char **matrix_names;
    int ***matrix_values; // [[[1,2,3],[4,5,6]],[[7,8,9],[10,11,12]]]*
    int matrix_last_index;
} vars;

void make_scalar(char token[]) {
    vars.scalar_names[vars.scalar_last_index] = malloc(sizeof(token));
    strcpy(vars.scalar_names[vars.scalar_last_index], token);
    vars.scalar_values[vars.scalar_last_index] = 0;
    vars.scalar_last_index++;
}

// TODO: boyut işi 
void make_vector(char token[]) {
    char *found = strchr(token, '[');
    char *var_name = malloc(found-token);
    strncpy(var_name, token, found-token);
    vars.vector_names[vars.vector_last_index] = malloc(found-token);
    strcpy(vars.vector_names[vars.vector_last_index], var_name);

    // size_t start = found-token + 1;
    // found = strchr(token, ']');
    // size_t end = found-token;

    // size_t num_len = end-start;

    // char *num_str = malloc(num_len);
    // strncpy(num_str, token+start, num_len);
    // int vector_size = atoi(num_str);
    
    
    vars.vector_last_index++;
}

// TODO: boyut işi 
void make_matrix(char token[]) {
    vars.matrix_names[vars.matrix_last_index] = malloc(sizeof(token));
    strcpy(vars.matrix_names[vars.matrix_last_index], token);
    //vars.matrix_values[vars.matrix_last_index] = {0}; // or just 0?
    vars.matrix_last_index++;
}

/* void make_assignments(char *tokens[]){
    for(int i = 0; i < vars.scalar_last_index; i++){
        if(strcmp(tokens[0], vars.scalar_names[i]) == 0){
            check_scalar_format(tokens[2]); // assignment parser
        }
    }
} */

// a * sqrt(b*c)

bool check_expression (char tokens[], char prev) {
    bool skip = false;
    if(strlen(tokens) == 0) {
        return true;
    }
    if(prev == ' ') {
        skip = true;
    }


    if(!skip){
        bool error = false;
        //now check expression
        //make sure look at the prev, so to <int><int> etc. cannot occur
        if(tokens[1] == '&'){
            error=true;
        }
        if(error) {
            return false;
        }
    }

    // shift left
    char new_tokens[strlen(tokens)-1];
    for(int i=0;i<strlen(tokens);i++){ // a ** b
        // işlem önceliği amk
        if(tokens[i] == '(') { // (a * (a+b))
            for(int j=i;j<strlen(tokens);j++){
                if(tokens[j] == ')') {
                    // burada flagle ve bu kısmı komple böl
                }
            }
        }
        if(tokens[i] == '*' || tokens[i] == '+' || tokens[i] == '-' )
        new_tokens[i] = tokens[i+1];
    }
    //free(tokens);
    check_expression(new_tokens, prev);
}

/* void check_scalar_format(char token[]) {
    int after_eq = 0;
    for(int i=0; i<sizeof(token)/sizeof(token[0]); i++){
        if(token[i] == '='){
            after_eq = i;
            break;
        }
    }
    for(int j=after_eq; j<sizeof(token)/sizeof(token[0]); j++) {
        if((strcmp(token[j], " ") !=0 || strcmp(token[j], "+") != 0 || strcmp(token[j], "*") != 0 || strcmp(token[j], "-") != 0)){
            if(isdigit(token[j])){
                continue;
            }
        }

        
    }
}

void check_assignment_format(char token[]) {
}
*/  


// match tokens from first index of array and process different function for each case
void match(char *tokens[]){
    if(strcmp(tokens[0], "scalar") == 0){
        make_scalar(tokens[1]);
    }
    else if(strcmp(tokens[0], "vector") == 0){
        // printf("%s", tokens[1]);
        make_vector(tokens[1]);
    }
    else if(strcmp(tokens[0], "matrix") == 0){
        make_matrix(tokens[1]);
    }
    // else if(strcmp(tokens[1], "=") == 0){
    //     make_assigment(tokens); // Give all the tokens to make_assigment function as assigments can include space
    // }
    // else if(strcmp(tokens[0], "printsep()") == 0){
    //     make_printsep();
    // }
    // else if(strstr(tokens[0], "print(") == 0){
    //     make_print(tokens); // parse pharantesis inside make_print function
    // }
    // else if(strstr(tokens[0], "for(") == 0){
    //     make_for(tokens);
    // }
    // else if(strstr(tokens[0], "tr(") == 0){
    //     make_tr(tokens);
    // }
    // else if(strstr(tokens[0], "sqrt(") == 0){
    //     make_sqrt(tokens);
    // }
    // else if(strstr(tokens[0], "choose(") == 0){
    //     make_choose(tokens);
    // }
    // else if(strcmp(tokens[0], "}") == 0){
    //     make_end(); // end for for example
    // }
    // else if(strcmp(tokens[0], ";") == 0){ // copilot -> think can be removed
    //     // do nothing
    // }
    // else{
    //     // if not a keyword, then it must be an identifier
    //     identifier(tokens);
    // }
    
}



int main(int argc, char *argv[]){
    vector ints = vec_make_vector();
    vec_append(&ints, 2);
    vec_append(&ints, 0);
    vec_append(&ints, 77);

    for(int i = 0; i < ints.size; i++){
        printf("%d\n", ints.elements[i]);
    }

    vec_remove(&ints, 0);

    for(int i = 0; i < ints.size; i++){
        printf("%d\n", ints.elements[i]);
    }

    FILE *matFile1;
    if(argc >= 1){
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
    
    int number_of_scalars = 0;
    int number_of_vectors = 0;
    int number_of_matrices = 0;
    // variable allocating
    while ((read = getline(&line_buf, &line_buf_size, matFile1)) != -1) {

        //parse line to array of strings
        char *line_array[15];
        int i = 0;
        char *token = strtok(line_buf, " ");
        while(token != NULL){
            line_array[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        //printf("%c\n", line_array[0][0]);
        //check if line is a comment
        if(strcmp(line_array[0], "scalar") == 0){
            number_of_scalars++;
        }
        //check if line is a blank line
        if(strcmp(line_array[0], "vector") == 0){
            number_of_vectors++;
    
        }
        //check if line is a blank line
        if(strcmp(line_array[0], "matrix") == 0){
            number_of_matrices++;
        }
    }
    vars.scalar_names = malloc(number_of_scalars * sizeof(char *));
    vars.scalar_values = malloc(number_of_scalars * sizeof(int));
    
    vars.vector_names = malloc(number_of_vectors * sizeof(char *));
    vars.vector_values = malloc(number_of_vectors * sizeof(int *));

    vars.matrix_names = malloc(number_of_matrices * sizeof(char *));
    vars.matrix_values = malloc(number_of_vectors * sizeof(int **));
    

    // TODO: reopen file
    FILE *matFile2;
    if(argc >= 1){
        matFile2 = fopen(argv[1], "r");
        if(matFile2 == NULL){
            printf("File cannot be opened.");
            return 0;
        }
    }else{
        printf("File not given.");
        return 0;
    }
    
    char *line_buf2 = NULL;
    size_t line_buf_size2 = 0;
    ssize_t read2;
    char *token;
    // parser
    while ((read2 = getline(&line_buf2, &line_buf_size2, matFile2)) != -1) {

        //parse line to array of strings
        char *line_array[15];
        int i = 0;
        token = strtok(line_buf2, " ");
        while(token != NULL){
            line_array[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        //printf("%c\n", line_array[0][0]);
        //check if line is a comment
        if(line_array[0][0] == '#'){
            continue;
        }
        //check if line is a blank line
        if(line_array[0][0] == '\n'){
            continue;
        }
     

        match(line_array);
    }

    for(int i = 0; i<vars.scalar_last_index; i++){
        printf("scalar index %d -> %s -> %d\n", i,  vars.scalar_names[i], vars.scalar_values[0]);
    }

    for(int i = 0; i<vars.vector_last_index; i++){
        printf("vector index %d -> %s -> ", i,  vars.vector_names[i]);
        // for(int j = 0; j<sizeof(vars.vector_values[i])/sizeof(vars.vector_values[i][0]); j++){
        //     printf("%d ", vars.vector_values[i][j]);
        // }
        printf("\n");
    }
    
    
    //  Free the allocated line buffer
    free(line_buf);
    line_buf = NULL;

    //  Close the file now that we are done with it
    fclose(matFile1);

    //  Free the allocated line buffer
    free(line_buf2);
    line_buf2 = NULL;

    //  Close the file now that we are done with it
    fclose(matFile2);
    return 1;
}