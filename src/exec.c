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
#include "config.h"

Job bg_jobs[MAX_JOBS];
int job_count = 0;
int next_job_id = 1;

/*** error ***/
int fatal(char* s) {
    perror(s);
    exit(EXIT_FAILURE);
}

 /*** output ***/
void shell_info(void) {
    struct passwd *p;
    char dir_name[1024];
    char host_name[256];

    if ((p = getpwuid(getuid())) == NULL) {
        perror("getpwuid error");
        return;
    }
    if (gethostname(host_name, 200) == -1) {
        perror("gethostname error");
        return;
    }
    if (getcwd(dir_name, 200) == NULL) {
        perror("getcwd error");
        return;
    }
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
    if (!tokens[1]) {
        if (chdir(getenv("HOME")) != 0) 
            perror("chdir"); 
        
        return;
    }
    if (tokens[2]) {
        printf("cd: too many arguments\n");
        return;       
    }
    if (chdir(tokens[1]) != 0) {
        perror("chdir");
        return;
    }
}

void shell_exit(char** tokens) {
    exit(EXIT_SUCCESS);
}

int num_of_builtins(void) {
  return sizeof(buildin_str) / sizeof(char *);
}

/*** execution***/
void shell_execution(char** tokens, int type) {
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
    if (type == FOREGROUND) {
        if (waitpid(pid, &status, 0) == -1) {
            fatal("Waitpid failed");
        }
    } else if (type == BACKGROUND) {
        printf("[%d] %d\n", next_job_id, pid);
        char full_command[100] = "";
        for (int i = 0; tokens[i] != NULL; i++) {
            strncat(full_command, tokens[i], sizeof(full_command) - strlen(full_command) - 1);
            if (tokens[i+1] != NULL) 
                strncat(full_command, " ", sizeof(full_command) - strlen(full_command) - 1);
            
        }
        if (job_count < MAX_JOBS) {
            bg_jobs[job_count].id = next_job_id;
            bg_jobs[job_count].pid = pid;
            strncpy(bg_jobs[job_count].command, full_command, sizeof(bg_jobs[job_count].command) - 1);
            job_count++;
            next_job_id++;
        } else {
            printf("Shell error: Too many background jobs\n");
        }
    }
}

void check_background_jobs(void) {
    int status;
    for (int i = 0; i < job_count; i++) {
        pid_t return_pid = waitpid(bg_jobs[i].pid, &status, WNOHANG);
        
        if (return_pid > 0) {
            printf("[%d]  Done                    %s\n", bg_jobs[i].id, bg_jobs[i].command);
            fflush(stdout);
            for (int j = i; j < job_count - 1; j++) {
                bg_jobs[j] = bg_jobs[j + 1];
            }
            job_count--;
            i--; 
        }
    }
}