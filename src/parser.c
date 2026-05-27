#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char* parse(char **argvec) {
    char *line = NULL;
    size_t size = 0;

    const ssize_t len = getline(&line, &size, stdin);

    if (len == -1) {
        free(line);
        return NULL;
    }

    int argnum = 0;
    char *tok = strtok(line, " \n");
    while (tok != NULL) {
        argvec[argnum++] = tok;
        tok = strtok(NULL, " \n");
    }
    argvec[argnum] = NULL;

    return line;
}
