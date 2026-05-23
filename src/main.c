/*** includes ***/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "exec.h"

/*** declarations ***/
void clear_screen(void);
void main_loop(void);
char* shell_read_line(void);
char** shell_split_line(const char*, int*);

/*** init ***/
int main(void) {
    clear_screen();
    while(1)
        main_loop();

    return 0; 
}

void clear_screen(void) {
  printf("\x1b[2J");
  printf("\x1b[H");
}

/*** main_loop ***/
void main_loop(void) {
    char* line = NULL;
    char** tokens = NULL;
    int arg = 0;
    shell_info();
    printf("> ");
    line = shell_read_line();
    if (!line) {
        printf("\n");
        exit(EXIT_SUCCESS);
    }
    tokens = shell_split_line(line, &arg);
    shell_execution(tokens);

    for (int i = 0; i < arg; i++) free(tokens[i]);

    free(line);
    free(tokens);
}

/*** input ***/
char* shell_read_line(void) {
    size_t buf_size = LINE_LEN_BUFSIZE;
    char* buffer = malloc(buf_size * sizeof(char));
    if (buffer == NULL) fatal("Allocation error.");

    if (getline(&buffer, &buf_size, stdin) == -1) {
        free(buffer);
        return NULL;
    }

    return buffer;
}

char** shell_split_line(const char* line, int* arg) {
    int buf_size = TOKENS_BUFSIZE;
    int i = 0;

    char** buffer_tok =  malloc(buf_size * sizeof(char*));
    if (buffer_tok == NULL) fatal("Allocation error");

    char* line_copy = strdup(line);
    if (line_copy == NULL) fatal("Allocation error");

    char delimiter[] = " \t\r\n\a";

    char* token = strtok(line_copy, delimiter);
    while (token) {
        buffer_tok[i] = strdup(token);

        if (++i >= buf_size) {
            buf_size += TOKENS_BUFSIZE;
            buffer_tok = realloc(buffer_tok, buf_size * sizeof(char*));
            if (buffer_tok == NULL) fatal("Allocation error");
        }
        token = strtok(NULL, delimiter);
    }
    buffer_tok[i] = NULL;
    *arg = i;
    free(line_copy);
    return buffer_tok;
}


