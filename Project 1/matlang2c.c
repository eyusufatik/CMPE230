#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h" 

// authors: Esad Yusuf Atik, Orkun Mahir Kılıç
// matlang to c converter
// we are just parsing from matlang file and writing to c file line by line

// match tokens from first index of array and process different function for each case
void match(char *tokens[]){
    if(strcmp(tokens[0], "scalar") == 0){
        make_scalar(tokens[1]);
    }
    else if(strcmp(tokens[0], "vector") == 0){
        make_vector(tokens[1]);
    }
    else if(strcmp(tokens[0], "matrix") == 0){
        make_matrix(tokens[1]);
    }
    else if(strcmp(tokens[1], "=") == 0){
        make_assigment(tokens); // Give all the tokens to make_assigment function as assigments can include space
    }
    else if(strcmp(tokens[0], "printsep()") == 0){
        make_printsep();
    }
    else if(strstr(tokens[0], "print(") == 0){
        make_print(tokens); // parse pharantesis inside make_print function
    }
    else if(strstr(tokens[0], "for(") == 0){
        make_for(tokens);
    }
    else if(strstr(tokens[0], "tr(") == 0){
        make_tr(tokens);
    }
    else if(strstr(tokens[0], "sqrt(") == 0){
        make_sqrt(tokens);
    }
    else if(strstr(tokens[0], "choose(") == 0){
        make_choose(tokens);
    }
    else if(strcmp(tokens[0], "}") == 0){
        make_end(); // end for for example
    }
    else if(strcmp(tokens[0], ";") == 0){ // copilot -> think can be removed
        // do nothing
    }
    else{
        // if not a keyword, then it must be an identifier
        identifier(tokens);
    }
    
}


int main(int argc, char *argv[]){
    FILE *matFile;
    if(argc >= 1){
        matFile = fopen(argv[1], "r");
        if(matFile == NULL){
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
    
    // Read lines & make operations on them.
    while ((read = getline(&line_buf, &line_buf_size, matFile)) != -1) {
        //for checker
        bool is_block = false;

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
        if(line_array[0][0] == '#'){
            continue;
        }
        //check if line is a blank line
        if(line_array[0][0] == '\n'){
            continue;
        }
     

        match(line_array);

        // print line_array
        for(int j = 0; j < i; j++){
            printf("%s ", line_array[j]);
        }
    }

     /* Free the allocated line buffer */
    free(line_buf);
    line_buf = NULL;

    /* Close the file now that we are done with it */
    fclose(matFile);
    return 1;
}