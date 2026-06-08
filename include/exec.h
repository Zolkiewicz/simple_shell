#ifndef EXEC_H
#define EXEC_H
#include <sys/types.h> 
#include <config.h>

typedef struct {
    int id;
    pid_t pid;
    char command[100];
} Job;

int fatal(char*);
void shell_info(void);
void shell_cd(char**);
void shell_exit(char**);
int num_of_builtins(void);
void shell_execution(char**, int);
void shell_execution_pipe(Pipeline, int);
void check_background_jobs(void);

#endif //EXEC_H