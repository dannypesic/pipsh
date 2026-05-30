#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pipe_list.h"

typedef enum {
    DEFAULT,
    DOUBLE_QUOTES,
} Mode;


char* parse(Command* cmds, char ** argv_pool) {
    char *line = NULL;
    size_t size = 0;

    const ssize_t len = getline(&line, &size, stdin);

    if (len == -1) {
        free(line);
        return NULL;
    }


    char *home = line, *write = line, *iter = line;
    int argnum = 0;
    int cmd_count = 0;
    int cmd_idx = 0;


    Mode mode = DEFAULT;

    while (*iter != '\0') {
        switch (mode) {
            case DEFAULT:
                if (*iter == '"') {
                    mode = DOUBLE_QUOTES;
                    iter++;
                } else if (*iter == ' ' || *iter == '\n') {
                    *write = '\0';
                    write++;
                    iter++;
                    argv_pool[argnum] = home;
                    home = write;
                    argnum++;
                } else if (*iter == '|') {
                    if (write > home) {
                        *write = '\0';
                        write++;
                        argv_pool[argnum] = home;
                        argnum++;
                    }
                    cmds[cmd_count] = (Command){
                        .argv = &argv_pool[cmd_idx],
                        .back_read = -1,
                        .next = NULL
                    };
                    if (cmd_count != 0) {
                        cmds[cmd_count-1].next = &cmds[cmd_count];
                    }
                    cmd_count++;
                    iter++;
                    while (*iter == ' ') iter++;
                    home = write;
                    argv_pool[argnum] = NULL;
                    argnum++;
                    cmd_idx = argnum;

                } else {
                    *write++ = *iter++;
                }
                break;
            case DOUBLE_QUOTES:
                if (*iter == '"') {
                    mode = DEFAULT;
                    iter++;
                } else {
                    *write++ = *iter++;
                }
                break;
        }
    }
    *write = '\0';
    if (write > home) {
        argv_pool[argnum] = home;
        argnum++;
    }
    argv_pool[argnum] = NULL;
    cmds[cmd_count] = (Command){
        .argv = &argv_pool[cmd_idx],
        .back_read = -1,
        .next = NULL
    };
    if (cmd_count != 0) {
        cmds[cmd_count-1].next = &cmds[cmd_count];
    }


    return line;
}
