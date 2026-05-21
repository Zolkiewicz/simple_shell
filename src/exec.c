/*** includes ***/
#include "exec.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <libgen.h>
#include <pwd.h>

/*** error ***/
int fatal(char* s) {
    perror(s);
    exit(EXIT_FAILURE);
}
 /*** output ***/
void shell_info(void) {
    struct passwd *p;
    char dir_name[200];
    char host_name[200];

    if ((p = getpwuid(getuid())) == NULL) fatal("getpwuid error");
    if (gethostname(host_name, 200) == -1) fatal("gethostname error");
    if (getcwd(dir_name, 200) == NULL) fatal("getcwd error");
    
    printf("[%s@%s %s]",p->pw_name, host_name, basename(dir_name));
}

/*** implementation ***/
void shell_execution(char** tokens) {
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) fatal("Invalid process");

    if (pid == 0) {
        execvp(tokens[0], tokens);
        fatal("execlp");
    }
    if ((pid = wait(&status)) == -1) fatal("Wait failed");
}