#include <stdio.h>
//#include <unistd.h>
#include <sys/wait.h>
//#include <fcntl.h>
//#include <signal.h>
//#include <string.h>
#include <stdlib.h>

#include "parser.h"
#include "builtins.h"
#include "prefix.h"
#include "magic_nums.h"
#include "pipe_list.h"

int main() {
    Prefix prefix;
    new_prefix(&prefix);
    if (new_prefix(&prefix) != 0) {return 1;}
    print_prefix(&prefix);

    while (1) {
        Command cmds[MAX_COMMANDS];
        char* argv_pool[ARGVEC_LENGTH];
        char* line = parse(cmds, argv_pool);
        if (line != NULL) {
            if (run_builtins(cmds, &prefix)) { break; }
            print_prefix(&prefix);
        }
        free(line);
    }
    free_prefix(&prefix);
    return 0;
}