/*** includes ***/
#include "exec.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <libgen.h>
#include <pwd.h>
#include <string.h>

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

/*** build_in_commands */
char *buildin_str[] = {
    "cd",
    "exit"
};

void (*builin_func[]) (char**) = {
    &shell_cd,
    &shell_exit
};

void shell_cd(char** tokens) {
    return;
}

void shell_exit(char** tokens) {
    exit(EXIT_SUCCESS);
}

int num_of_builtins(void) {
  return sizeof(buildin_str) / sizeof(char *);
}

/*** execution***/
void shell_execution(char** tokens) {
    if (tokens[0] == NULL) return;
    //build ins
    
    int num = num_of_builtins();
    for (int i = 0; i < num; i++) {
        if (strcmp(tokens[0], buildin_str[i]) == 0) {
            (*builin_func[i])(tokens);
            return;
        }
    }

    //sys calls
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) fatal("Invalid process");

    if (pid == 0) {
        execvp(tokens[0], tokens);
        fatal(tokens[0]);
    }
    if (waitpid(pid, &status, 0) == -1) 
        fatal("Waitpid failed");

}