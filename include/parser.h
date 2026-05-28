#ifndef PIPSH_PARSER_H
#define PIPSH_PARSER_H

#include "pipe_list.h"

char* parse(Command* cmds, char ** argv_pool);

#endif //PIPSH_PARSER_H
