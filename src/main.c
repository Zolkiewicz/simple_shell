/*** includes ***/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "exec.h"
#include <stdbool.h>
#include <errno.h>
#include <setjmp.h> 
#include <signal.h>

char special [] = {' ', '\t', '&', ';', '\n', '\0'};
char* line = NULL;
char** tokens = NULL;
sigjmp_buf position;

/*** declarations ***/
void clear_screen(void);
void main_loop(void);
void shell_read_line(void);
int shell_get_tok(size_t* len, char* input);
bool is_arg_char(char c);
void shell_split_line(void);
void sigint_handler(int);

/*** init ***/
int main(void) {
    clear_screen();
    static struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);


    if (sigsetjmp(position, 1) != 0) {
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

    while(1) {
        main_loop();
    }
        
    return 0; 
}

void clear_screen(void) {
  printf("\x1b[2J");
  printf("\x1b[H");
}

void sigint_handler(int sig) {
    printf("\n");
    siglongjmp(position, 1);
}



/*** main_loop ***/
void main_loop(void) {
    check_background_jobs();

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
        if (errno == EINTR) {
            line = malloc(1);
            if (line == NULL) fatal("Allocation error");
            line[0] = '\n';
            return; 
        }
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
        case '|':
            return PIPE;
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
    Pipeline pipeline;
    pipeline.cmd_count = 0;

    tokens = calloc(size, sizeof(char*));
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
            case PIPE:
                if (arg != 0) {
                    tokens[arg] = NULL;
                    pipeline.commands[pipeline.cmd_count++] = tokens;
                    size = TOKENS_SIZE;
                    tokens = calloc(size, sizeof(char*));
                    if (tokens == NULL) fatal("Allocation error");
                    arg = 0;
                }
                pos++;
                break;
            case SEMICOLON:
            case AMPERSAND:
            case EOL:
                if (arg != 0) {
                    tokens[arg] = NULL;
                    pipeline.commands[pipeline.cmd_count++] = tokens;
                }

                if (pipeline.cmd_count > 0) {
                    int exec_type = (tok_type == AMPERSAND) ? BACKGROUND : FOREGROUND;

                    if (pipeline.cmd_count == 1) {
                        shell_execution(pipeline.commands[0], exec_type);
                    } else {
                        shell_execution_pipe(pipeline, exec_type);
                    }
                    for (int c = 0; c < pipeline.cmd_count; c++) {
                        for (int i = 0; pipeline.commands[c][i] != NULL; i++) {
                            free(pipeline.commands[c][i]);
                        }
                        free(pipeline.commands[c]);
                    }
                    pipeline.cmd_count = 0;
                }
                tokens = NULL; 
                arg = 0;

                if (tok_type == EOL) pos = NULL;
                else pos++;
                break;
        }
    }
}


