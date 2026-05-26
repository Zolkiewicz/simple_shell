/*** includes ***/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "exec.h"
#include <stdbool.h>

char* line = NULL;
char** tokens = NULL;

/*** declarations ***/
void clear_screen(void);
void main_loop(void);
void shell_read_line(void);
int shell_get_tok(size_t* len, char* input);
bool is_arg_char(char c);
void shell_split_line(void);

/*** init ***/
int main(void) {
    clear_screen();
    
    while(1) {
        main_loop();
    }
        
    return 0; 
}

void clear_screen(void) {
  printf("\x1b[2J");
  printf("\x1b[H");
}

/*** main_loop ***/
void main_loop(void) {
    shell_info();
    printf("> ");
    fflush(stdout);

    shell_read_line();

    if (!line) {
        printf("\n");
        exit(EXIT_SUCCESS);
    }

    shell_split_line();

    if (tokens != NULL) {
        for (int i = 0; tokens[i] != NULL; i++) {
            free(tokens[i]);
        }
        free(tokens);
        tokens = NULL;
    }
    
    free(line);
    line = NULL;
}

/*** input ***/

void shell_read_line(void) {
    size_t buf_size = LINE_LEN_BUFSIZE;

    if (getline(&line, &buf_size, stdin) == -1) {
        free(line);
        line = NULL;
    }
}

int shell_get_tok(size_t* len, char* input) {
    *len = 0;

    switch (*input) {
        case '\n':
            return EOL;
        case '&':
            return AMPERSAND;
        case ';':
            return SEMICOLON;
        default:
            (*len)++;
            while (is_arg_char(input[*len])) 
                (*len)++;
            break;

    }
    return ARG;
}

bool is_arg_char(char c) {
    char *wrk;
    for (wrk = special; *wrk; wrk++)
        if (c == *wrk)
            return false;

    return true;
}

void shell_split_line(void) {
    size_t len = 0;
    char* pos = line; 
    size_t size = TOKENS_SIZE;
    size_t arg = 0;
    int type;
    int tok_type;

    tokens =  malloc(size * sizeof(char*));
    if (tokens == NULL) 
        fatal("Allocation error");
    tokens[0] = NULL;

    while (pos != NULL) {
        if (*pos == ' ' || *pos == '\t') {
            pos++;
            continue;
        }

        switch ((tok_type = shell_get_tok(&len, pos))) {
            case ARG:
                if (arg >= size - 1) {
                    size += TOKENS_SIZE;
                    tokens = realloc(tokens, size * sizeof(char*));
                    if (tokens == NULL) fatal("Allocation error");
                } 
                tokens[arg] =  malloc((len + 1) * sizeof(char));
                if (tokens[arg] == NULL) fatal("Allocation error");

                memcpy(tokens[arg], pos, len);
                tokens[arg][len] = '\0';
                pos += len;
                arg++;
                tokens[arg] = NULL;
                break;
            case AMPERSAND:
                if (tok_type == AMPERSAND) type = BACKGROUND;
                else type = FOREGROUND;
            case EOL:
                if (EOL) pos = NULL;
                else pos++;
            case SEMICOLON:
                if (arg != 0) {
                    tokens[arg] = NULL;
                    shell_execution(tokens);
                }

                arg = 0;
                break;
        }
    }
}


