#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "ctype.h"
#include "math.h"

#include "headers/vector.h"
#include "headers/vars.h"
#include "headers/matchers.h"
#include "headers/utils.h"
#include "headers/makers.h"
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

// global var for printing } or }} outside a matlang for loop
bool double_for;

/*
    Finds out if a line is a comment, print statement, declaration, 
    assignment statement or a for loop start/end
*/
char* convert_line(char *line){
    trim(line);
    vector tokens = tokenize(line, " \n\t");

    if(tokens.size == 0){
        vec_str_append(&tokens, line);
    }

    if(strncmp(line, "#", 1) == 0){
       return "comment";
    }else if(strstr(line, "printsep(")){
        return make_print_sep_func();
    }else if(prefix(line, "for") && strstr(line, ":") && strstr(line, "{")){
        return make_for(line, &double_for);
    }else if (strcmp(line, "}") == 0){
        if(double_for){
            return "}}";
        }else{
            return "}";
        }
    }else if(strcmp(tokens.elements[0], "scalar") == 0){
        return make_scalar(line);
    }else if(strcmp(tokens.elements[0], "vector") == 0){
        return make_vector(line);
    }else if(strcmp(tokens.elements[0], "matrix") == 0){
        return make_matrix(line);
    }else if(strchr(line, '=')){
        return make_assignment(line);
    }else if(strstr(line, "print(")){
       return make_print_func(line);
    }else{
        return "not imp";
    }
}

int main(int argc, char *argv[]){
    // Initialize var name and dimension holders
    vars.scalar_names = vec_make_vector();
    vars.vector_names = vec_make_vector();
    vars.vector_dimensions = vec_make_vector();
    vars.matrix_names = vec_make_vector();
    vars.matrix_dimensions = vec_make_vector();

    double_for = false;

    // open up file to read and write to.
    FILE *matFile1;
    FILE *fp;
    fp = fopen("output.c", "w");
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

    // read line and hold them in a string vector.
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    ssize_t read;

    vector lines = vec_make_vector();
    while ((read = getline(&line_buf, &line_buf_size, matFile1)) != -1) {
        if(strcmp(line_buf, "\n") != 0)
            vec_str_append(&lines, line_buf);
    }


    // convert each line to C
    for (int i = 0; i < lines.size; i++){
        printf("%s", (char*)(lines.elements[i]));
        char *line_in_c = convert_line((char*)(lines.elements[i]));

        if(strcmp(line_in_c, "error") == 0){
            printf("error at line (%d): \n", i);
            break;
        }else if(strcmp(line_in_c, "comment") == 0 ){

        }else{
            fprintf(fp, "%s\n", line_in_c); // if line is not a comment or doesn't contain syntax errors. Write to output file.
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