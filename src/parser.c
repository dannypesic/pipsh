#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    Default,
    DoubleQuotes,
} Mode;

char* parse(char **argvec) {
    char *line = NULL;
    size_t size = 0;

    const ssize_t len = getline(&line, &size, stdin);

    if (len == -1) {
        free(line);
        return NULL;
    }

    // set a home pointer and iter_ptr+write_ptr (starts at line)
    // set a argnum = 0
    // if the iter pointer doesn't point to NULL:
    //   switch mode DEFAULT:
    //     if char " -> enter dq mode and iter_ptr++
    //     elif char ' ' || \n -> {*write_ptr = \0, write_ptr++, iter_ptr++,
    //     argvec[argnum]=home, home=write_ptr and argnum++}
    //     else -> *write_ptr++ = *iter_ptr++
    //   switch mode DoubleQuotes:
    //     if char " -> enter default mode and iter++
    //     else -> *write_ptr++ = *iter_ptr++
    // else: *write_ptr = \0, write_ptr++,
    //     argvec[argnum]=home argvec[argnum++] = NULL && return (note, line needs to be freed after!)

    char *home = line, *write = line, *iter = line;
    int argnum = 0;

    Mode mode = Default;

    while (*iter != '\0') {
        switch (mode) {
            case Default:
                if (*iter == '"') {
                    mode = DoubleQuotes;
                    iter++;
                } else if (*iter == ' ' || *iter == '\n') {
                    *write = '\0';
                    write++;
                    iter++;
                    argvec[argnum] = home;
                    home = write;
                    argnum++;
                } else {
                    *write++ = *iter++;
                }
                break;
            case DoubleQuotes:
                if (*iter == '"') {
                    mode = Default;
                    iter++;
                } else {
                    *write++ = *iter++;
                }
                break;
        }
    }
    *write = '\0';
    argvec[argnum] = home;
    argvec[argnum++] = NULL;

    // int argnum = 0;
    // char *tok = strtok(line, " \n");
    // while (tok != NULL) {
    //     argvec[argnum++] = tok;
    //     tok = strtok(NULL, " \n");
    // }
    // argvec[argnum] = NULL;

    return line;
}
