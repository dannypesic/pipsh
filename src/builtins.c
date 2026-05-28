#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#include "magic_nums.h"
#include "prefix.h"
#include "pipe_list.h"


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

    } else {
        pid_t pid = fork();
        if (pid == 0) {
            if (execvp(argvec[0],argvec) != 0) {
                printf("%s: command not found\n", argvec[0]);
            }
        }
    }
    return 0;
}

int run_builtins(Command * cmds, Prefix * prefix) {
    int forks = 0;
    int cmd_idx = 0;
    int rval = 0;
    while (cmds[cmd_idx].next != NULL) {

        if (exec_cmd(cmds[cmd_idx].argv, prefix) != 0) { rval = 1; break;}
        forks++;
        cmd_idx++;
    }
    if (exec_cmd(cmds[cmd_idx].argv, prefix) != 0) { rval = 1; }
    forks++;
    while (forks--) wait(NULL);

    return rval;
}