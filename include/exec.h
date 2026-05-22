#ifndef EXEC_H
#define EXEC_H

int fatal(char*);
void shell_info(void);
void shell_cd(char**);
void shell_exit(char**);
int num_of_builtins(void);
void shell_execution(char**);

#endif //EXEC_H