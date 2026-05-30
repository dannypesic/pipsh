#ifndef PIPSH_AST_H
#define PIPSH_AST_H


typedef struct Command {
    char ** argv;
    int back_read;
    struct Command * next;
} Command;

#endif //PIPSH_AST_H

