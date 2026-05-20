/*** includes ***/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"

/*** declarations ***/
void main_loop();
char* shell_read_line();
char** shell_split_line(const char*, int*);
int fatal(char*);

/*** init ***/
int main(void) {

    main_loop();

    return 0; 
}

/*** implementation ***/
void main_loop() {
    int status = 1;
    char* line = NULL;
    char** tokens = NULL;
    int arg = 0;

    printf("> ");
    line = shell_read_line();
    tokens = shell_split_line(line, &arg);

    for (int i = 0; i < arg; i++) {
        printf("%s\n", tokens[i]);
        free(tokens[i]);
    }

    free(line);
    free(tokens);
}

char* shell_read_line() {
    size_t buf_size = LINE_LEN_BUFSIZE;
    char* buffer = malloc(buf_size * sizeof(char));
    if (buffer == NULL) fatal("Allocation error.");

    getline(&buffer, &buf_size, stdin);

    return buffer;
}

char** shell_split_line(const char* line, int* arg) {
    const size_t buf_size = TOKENS_BUFSIZE;
    const size_t len = strlen(line);
    int position = 0;

    char** buffer_tok =  malloc(buf_size * sizeof(char*));
    if (buffer_tok == NULL) fatal("Allocation error");
    
    while (position < len) {
        while (position < len && 
            (line[position] == ' ' || 
            line[position] == '\t' || 
            line[position] == '\n')) position++;
        
        if (position >= len) break;

        int i = 0;
        while (position + i < len && 
            line[position + i] != ' ' && 
            line[position + i] != '\t' && 
            line[position + i] != '\n') i++;
        
        buffer_tok[*arg] = malloc((i + 1) * sizeof(char));
        if (buffer_tok[*arg] == NULL) fatal("Allocation error");

        strncpy(buffer_tok[*arg], &line[position], i);
        position += i;
        buffer_tok[*arg][i] = '\0'; 

        if (++(*arg) >= buf_size) 
            fatal("Too many arguments");
    }
    buffer_tok[*arg] = NULL;
    return buffer_tok;
}


/*** error ***/
int fatal(char* s) {
    perror(s);
    exit(EXIT_FAILURE);
}