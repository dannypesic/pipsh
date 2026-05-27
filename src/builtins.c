
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include "prefix.h"

int run_builtins(char ** argvec, Prefix * prefix) {

    if (strcmp(argvec[0], "exit") == 0) {
        return 1;
    }

    if (strcmp(argvec[0], "cd") == 0) {

        chdir(argvec[1]);
        char dirbuf[256];
        getcwd(dirbuf, sizeof(dirbuf));
        setenv("PWD", dirbuf, 1);
        update_dir(prefix, dirbuf);

    } else {
        pid_t pid = fork();
        if (pid == 0) {
            execvp(argvec[0],argvec);
        }
        wait(NULL);
    }

    return 0;
}
