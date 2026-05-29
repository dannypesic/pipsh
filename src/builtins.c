#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include "magic_nums.h"
#include "prefix.h"
#include "pipe_list.h"


typedef enum {
    OUTPUT_REDIR,
    APPEND_REDIR,
    INPUT_REDIR,
    NONE
} RedirectMode;


int exec_cmd(char ** argvec, Prefix * prefix) {
    if (strcmp(argvec[0], "exit") == 0) {
        return 1;
    }

    if (strcmp(argvec[0], "cd") == 0) {

        chdir(argvec[1]);
        char dirbuf[PATH_LENGTH];
        getcwd(dirbuf, sizeof(dirbuf));
        setenv("PWD", dirbuf, 1);
        update_dir(prefix, dirbuf);

        return 2;

    }
    pid_t pid = fork();
    if (pid == 0) {
        int idx = 0;
        RedirectMode mode = NONE;
        while (argvec[idx] != NULL) {
            if (strcmp(argvec[idx], ">") == 0) {
                mode = OUTPUT_REDIR;
                argvec[idx] = NULL;
                break;
            }
            if (strcmp(argvec[idx], ">>") == 0) {
                mode = APPEND_REDIR;
                argvec[idx] = NULL;
                break;
            }
            if (strcmp(argvec[idx], "<") == 0) {
                mode = INPUT_REDIR;
                argvec[idx] = NULL;
                break;
            }
            idx++;
        }
        if (mode != NONE) {
            idx++;
            if (argvec[idx] == NULL) {
                printf("FATAL: Did not specify file path for redirector.");
                exit(127);
            }
        }

        int fd;
        switch (mode) {
            case OUTPUT_REDIR:
                fd = open(argvec[idx], O_WRONLY|O_CREAT|O_TRUNC, 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
                break;
            case APPEND_REDIR:
                fd = open(argvec[idx], O_WRONLY|O_CREAT|O_APPEND, 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
                break;
            case INPUT_REDIR:
                fd = open(argvec[idx], O_RDONLY, 0644);
                dup2(fd, STDIN_FILENO);
                close(fd);
                break;
            case NONE:
                break;
        }

        execvp(argvec[0],argvec);
        printf("%s: command not found\n", argvec[0]);
        exit(127);

    }
    return 0;
}

int run_builtins(Command * cmds, Prefix * prefix) {
    int forks = 0;
    int cmd_idx = 0;
    int rval = 0;
    while (cmds[cmd_idx].next != NULL) {

        const int cmd_rval = exec_cmd(cmds[cmd_idx].argv, prefix);
        if (cmd_rval == 0) { forks++;}
        if (cmd_rval == 1) { rval = 1; break; }
        cmd_idx++;
    }
    const int cmd_rval = exec_cmd(cmds[cmd_idx].argv, prefix);
    if (cmd_rval == 0) { forks++;}
    if (cmd_rval == 1) { rval = 1; }
    while (forks--) wait(NULL);

    return rval;
}