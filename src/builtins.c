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


int exec_cmd(Command * cmd, Prefix * prefix, int front_read, int front_write) {
    char ** argvec = cmd->argv;
    int back_read = cmd->back_read;

    if (strcmp(argvec[0], "exit") == 0) {
        return 1;
    }
    

    if (strcmp(argvec[0], "echo") == 0 && argvec[1][0] == '$') {
        const char* env_var = getenv(& argvec[1][1]);
        if (env_var == NULL) {
            printf("Error: environment variable not specified.\n");
        } else {
            printf("%s\n", env_var);
        }
        return 2;
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
        if (back_read != -1) {
            dup2(back_read, STDIN_FILENO);
            close(back_read);
        }

        if (front_write != -1 && front_read != -1) {
            dup2(front_write, STDOUT_FILENO);
            close(front_read); close(front_write);
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

        int front_pipe[2];
        if (pipe(front_pipe) == -1) {
            perror("pipe");
            rval = 1;
            break;
        }

        const int cmd_rval = exec_cmd(&cmds[cmd_idx], prefix, front_pipe[0], front_pipe[1]);
        if (cmd_rval == 0) {
            forks++;
            cmds[cmd_idx + 1].back_read = front_pipe[0];
            close(front_pipe[1]);
        }
        if (cmd_rval == 1) { rval = 1; break; }
        cmd_idx++;
    }

    if (rval != 1) {
        const int cmd_rval = exec_cmd(&cmds[cmd_idx], prefix, -1, -1);
        if (cmds[cmd_idx].back_read != -1) {
            close(cmds[cmd_idx].back_read);
        }
        if (cmd_rval == 0) { forks++;}
        if (cmd_rval == 1) { rval = 1; }
    }
    while (forks--) wait(NULL);

    return rval;
}