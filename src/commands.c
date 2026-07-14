#include "commands.h"
#include "parsers.h"
#include "store.h"
#include "writer.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>

typedef void (*command_handler)(command_context *ctx, const resp_value *parsed_value);
typedef struct command_entry {
    const char *name;
    command_handler handler;    
    int min_args; /* include the command itself */
    int max_args;
} command_entry;

static int is_number_string(const char *str) {
    if (str == NULL || *str == '\0') return 0;

    // Allow a leading negative or positive sign
    if (*str == '-' || *str == '+') str++;

    // Ensure there is at least one digit after the sign
    if (*str == '\0') return 0;

    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}
long currentMillis() {
	struct timeval tp;

	gettimeofday(&tp, NULL);
	return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

static void command_ping(command_context *ctx, const resp_value *parsed_value) {
    write_simple_string(ctx->client_fd, "PONG");
}

static void command_echo(command_context *ctx, const resp_value *parsed_value) {
    resp_value **elements = parsed_value->data.array.elements;
    if(elements[1]->type != RESP_BULK_STRING) {
        write_error(ctx->client_fd, "expects bulk string in 2nd arg");
        return;
    }
    string_value arg1 = elements[1]->data.string;
    write_bulk_string(ctx->client_fd, arg1.str, arg1.len);
}

static void command_set(command_context *ctx, const resp_value *parsed_value) {
    resp_value **elements = parsed_value->data.array.elements;
    char parameter[3] = {0};
    long expiry_time = -1;
    if(elements[1]->type != RESP_BULK_STRING 
    || elements[2]->type != RESP_BULK_STRING) {
        write_error(ctx->client_fd, "expects atleast Key and Value");
        return;
    }

    // if passing parameter without a value
    if(parsed_value->data.array.count > 3) {
        if (strcasecmp(elements[3]->data.string.str, "PX") == 0) {
            strcpy(parameter, "PX");
        }
        if (strcasecmp(elements[3]->data.string.str, "EX") == 0) {
            strcpy(parameter, "EX");
        }

        if (parameter[0] == 0 || parsed_value->data.array.count != 5) {
            write_error(ctx->client_fd, "Wrong format! Expects SET [KEY] [VALUE] [EX|PX] [TIME]");
            return;
        }

        char *time_string = elements[4]->data.string.str;

        if(!is_number_string(time_string)) {
            write_error(ctx->client_fd, "Wrong format! Expects time to be in a number");
            return;
        }

        long ttl = strtol(time_string, NULL, 10);
        long now = currentMillis();

        if(parameter[0] == 'P') {
            expiry_time = now + ttl;
        } else {
            expiry_time = now + (ttl * 1000);
        }
    }

    if(set_string_value(elements[1]->data.string.str, elements[2]->data.string.str, expiry_time)) {
        write_simple_string(ctx->client_fd, "OK");
    } else {
        write_error(ctx->client_fd, "expects bulk string in 2nd and 3rd arg");
    }
}

static void command_get(command_context *ctx, const resp_value *parsed_value) {
    resp_value **elements = parsed_value->data.array.elements;
    if(elements[1]->type != RESP_BULK_STRING) {
        write_error(ctx->client_fd, "expects bulk string in 2nd arg");
        return;
    }

    store_item *item = get_string_value(elements[1]->data.string.str);

    if(item == NULL || item->expiry_time < currentMillis()) {
        write_bulk_string(ctx->client_fd, "", -1);
        return;
    } 

    write_bulk_string(ctx->client_fd, item->value, strlen(item->value));
}

static const command_entry commands[] = {
    {"PING", command_ping, 1, 1},
    {"ECHO", command_echo, 2, 2},
    {"GET", command_get, 2, 2},
    {"SET", command_set, 3, 5},
    {NULL, NULL, 0, 0}
};

void command_dispatch(command_context *ctx, resp_value *parsed_value) {
    if(parsed_value->type != RESP_ARRAY 
    || parsed_value->data.array.count <= 0 
    || parsed_value->data.array.elements[0]->type != RESP_BULK_STRING)  {
        write_error(ctx->client_fd, "Expects array of bulk string!");
        return;
    }
    resp_value **elements = parsed_value->data.array.elements;
    char *command = elements[0]->data.string.str;


    for (const command_entry *e = commands; e->name; e++) {
        if(strcasecmp(command, e->name) != 0)  continue;

        if(parsed_value->data.array.count > e->max_args
        || parsed_value->data.array.count < e->min_args) {

            write_error(ctx->client_fd, "Wrong number of args");
            return;
        }    

        e->handler(ctx, parsed_value);
        return;
    }

    write_error(ctx->client_fd, "Unknown command");
}


