#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>

int main() {
    setenv("PATH", "/bin:/usr/bin:/sbin:/usr/sbin:/opt/homebrew/bin:/opt/homebrew/sbin:/usr/local/bin:/Users/danny/.local/bin:/Users/danny/.cargo/bin:/Users/danny/.dotnet/tools", 1);
    uid_t uid = getuid();
   struct passwd *pw = getpwuid(uid);
    if (pw == NULL) {
        return 1;
    }

    char hostname[64];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        char *dot = strchr(hostname, '.');
        if (dot)
            *dot = '\0';
    } else {
        return 1;
    }

    int should_run = 1;
    printf("%s@%s &: ", pw->pw_name, hostname);
    while (should_run) {
        char *line = NULL;
        size_t size = 0;

        const ssize_t len = getline(&line, &size, stdin);

        if (len == -1) {
            free(line);
        } else {
            char *argvec[32];
            int argnum = 0;
            char *tok = strtok(line, " \n");
            while (tok != NULL) {
                argvec[argnum++] = tok;
                tok = strtok(NULL, " \n");
            }
            argvec[argnum] = NULL;

            //builtins
            if (strcmp(argvec[0], "exit") == 0) {
                should_run = 0;
            } else {
                char * const *args = (const char * const *)argvec;
                pid_t pid = fork();
                if (pid == 0) {
                    execvp(args[0],args);
                }
                wait(NULL);
                printf("%s@%s &: ", pw->pw_name, hostname);
            }
            free(line);
        }
    }
    return 0;
}