#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "stdbool.h"
#include "output_header.h"

char *trim(char *str)
{
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

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

bool prefix(const char *str, const char *pre){
    return strncmp(str, pre, strlen(pre)) == 0;
}

bool match_func(char *expr){
    if(prefix(expr, "tr(") || prefix(expr, "sqrt(")){
        int level = 0;
        bool enteredOnce = false;
        for (int i = 0; i < strlen(expr); i++){
            char c = expr[i];
            if(c == '('){
              level++;
              enteredOnce = true;
            }
            else if(c == ')'){
              level--;
            }
            if(enteredOnce && level == 0 && i != strlen(expr)-1)
                return false;
        }
        return true;
    }else{
        return false;
    }
}

char* get_choose_expr_close(char *str){
  
    char *ptr = str;
    int p_level = 0; // paranthesis
    int s_level = 0; // square bracket
    while(*ptr != '\0'){
      printf("%s\n", ptr);
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
int main(){
    char *expr = "bla[2,3], blo";
    printf("%s\n", get_choose_expr_close(expr));
    printf("%s\n", expr);

    // char *line = "a*b*c+c*c";
    // char *left = malloc(255);
    // char *right = malloc(255);
    // strncpy(left, line, 6-1);
    // strncpy(right, line+6, strlen(line)-6);
    // printf("left: %s\nright: %s\n", left, right);

    // char *a = malloc(5);
    // strcpy(a, "==");

    // char *token = strtok(a, "=");

    // while(token != NULL){
    //   printf("%s\n", token);
    //   token = strtok(NULL, "=");
    // }

    // char *str = "hello";

    // printf("%s\n", str);

    // str[0] = 'd';
    // printf("%s\n", str);


    // void *i = calloc(1, sizeof(int));
    // *(float *)i = 5.67;
    // printf("%f", *(float *)i);

    // out_vector v = make_out_vector(3);
    // ((int *)v.elements)[0] = 5;
    // out_matrix x = make_out_matrix(2,2);
    // ((int *)(x.elements[0]))[0] = 5;
    // ((float *)(x.elements[0]))[0] = 1.23;
    // printf("%f", ((float *)(x.elements[0]))[0]);

}