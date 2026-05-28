#ifndef PIPSH_AST_H
#define PIPSH_AST_H


typedef struct Command {
    char ** argv;
    char * redirect_in;   // < file
    char * redirect_out;  // > file
    char * redirect_app;  // >> file
    struct Command * next;
} Command;

#endif //PIPSH_AST_H

