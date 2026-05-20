#include "exec.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>

int fatal(char* s) {
    perror(s);
    exit(EXIT_FAILURE);
}

void execution(char** tokens) {
    pid_t pid;
    if ((pid = fork()) < 0) fatal("Invalid process");

    if (pid == 0) {
        execlp(tokens[0], tokens[0], (char *)NULL);
        fatal("execlp");
    }
    wait((int *)0);
}