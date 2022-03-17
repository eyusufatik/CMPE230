#include "stdio.h"
#include "stdlib.h"

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
    
    while ((read = getline(&line_buf, &line_buf_size, matFile)) != -1) {
        // printf("%s", line_buf);
    }

    //  Free the allocated line buffer
    free(line_buf);
    line_buf = NULL;

    //  Close the file now that we are done with it
    fclose(matFile);
    return 1;
}