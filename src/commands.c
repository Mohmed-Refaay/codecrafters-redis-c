#include "commands.h"
#include "parsers.h"
#include "writer.h"
#include <_strings.h>
#include <_time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const char*respone = "+PONG\r\n";
const char *error = "-Error\r\n";

void command_dispatch(command_context *ctx, resp_value *parsed_value) {
    resp_value **elements = parsed_value->data.array.elements;
    char *command = elements[0]->data.string.str;
    if(!command) {
        write_error(ctx->client_fd, "Command is missing!");
        return;
    }

    printf("command: %s\n", command);

    if(strcasecmp(command, "PING") == 0) {
        write_simple_string(ctx->client_fd, "PONG");
    } else if (strcasecmp(command, "ECHO") == 0) {
        char *arg1 = elements[1]->data.string.str;
        printf("arg1: %s\n", arg1);
        if(!arg1) {
            write_error(ctx->client_fd, "arg is missing!");
        } else {
            write_bulk_string(ctx->client_fd, arg1, strlen(arg1));
        }
    } else {
        write_error(ctx->client_fd, "Unkown command!");
    }
}


