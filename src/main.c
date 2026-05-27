#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
//#include <fcntl.h>
//#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>

#include "parser.h"
#include "builtins.h"

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

    printf("%s@%s &: ", pw->pw_name, hostname);
    while (1) {

        char *argvec[64];
        char* line = parse(argvec);
        if (line != NULL) {

            if (run_builtins(argvec)) { break; }
            printf("%s@%s &: ", pw->pw_name, hostname);

        }
        free(line);
    }
    return 0;
}