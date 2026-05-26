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

static char special [] = {' ', '\t', '&', ';', '\n', '\0'};

#endif //CONFIG_H