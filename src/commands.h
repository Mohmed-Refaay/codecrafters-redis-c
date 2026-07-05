#ifndef COMMANDS_H
#define COMMANDS_H

#include "parsers.h"

typedef struct command_context{
    int client_fd;
} command_context;

void command_dispatch(command_context *ctx,resp_value *parsed_value);


#endif