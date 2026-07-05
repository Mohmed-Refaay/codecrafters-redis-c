#ifndef PARSERS_H
#define PARSERS_H

#include <stddef.h>

typedef enum resp_type {
    RESP_SIMPLE_STRING = '+',
    RESP_BULK_STRING = '$',
    RESP_ARRAY = '*'
} resp_type;

typedef struct string_value {
    char* str; 
    size_t len;
} string_value;

typedef struct array_value {
    struct resp_value **elements; 
    size_t count; 
} array_value;

typedef struct resp_value {
    resp_type type;
    int is_null; 

    union {
        // Strings
        string_value string;
        // Array
        array_value array;
    } data;
} resp_value;

resp_value *parse_resp(const char* payload);
void free_resp(resp_value *resp);

#endif
