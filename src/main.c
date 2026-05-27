#include <stdio.h>
//#include <unistd.h>
#include <sys/wait.h>
//#include <fcntl.h>
//#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"
#include "builtins.h"
#include "prefix.h"

int main() {
    setenv("PATH", "/bin:/usr/bin:/sbin:/usr/sbin:/opt/homebrew/bin:/opt/homebrew/sbin:/usr/local/bin:/Users/danny/.local/bin:/Users/danny/.cargo/bin:/Users/danny/.dotnet/tools", 1);

    Prefix prefix;
    new_prefix(&prefix);
    if (new_prefix(&prefix) != 0) {return 1;}
    print_prefix(&prefix);

    while (1) {

        char *argvec[64];
        char* line = parse(argvec);
        if (line != NULL) {
            if (run_builtins(argvec, &prefix)) { break; }
            print_prefix(&prefix);
        }
        free(line);
    }
    free_prefix(&prefix);
    return 0;
}