#ifndef CONFIG_H
#define CONFIG_H

#define LINE_LEN_BUFSIZE 1024
#define TOKENS_SIZE 16

#define FOREGROUND 0
#define BACKGROUND 1

#define EOL 1
#define ARG 2
#define AMPERSAND 3
#define SEMICOLON 4
#define PIPE 5


#define MAX_PIPE_CMDS 10

typedef struct {
    char** commands[MAX_PIPE_CMDS];
    int cmd_count;
} Pipeline;

#define MAX_JOBS 64
#endif //CONFIG_H