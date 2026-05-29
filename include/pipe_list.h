#ifndef PIPSH_AST_H
#define PIPSH_AST_H


typedef struct Command {
    char ** argv;
    struct Command * next;
} Command;

#endif //PIPSH_AST_H

