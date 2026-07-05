#include "parsers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*
    RESP encodes bulk strings in the following way:
    $<length>\r\n<data>\r\n
    Caller must free.
 */
static resp_value *parse_bulk_string(const char *payload) {
    if (payload[0] != '$') {
        return NULL;
    }

    resp_value *parsed_value = calloc(1, sizeof(resp_value));
    if(parsed_value == NULL) {
        return NULL;
    }
    parsed_value->type = RESP_BULK_STRING;

    string_value string_data;
    char *endptr;
    long string_len = strtol(payload + 1, &endptr, 10);

    if(string_len == -1) {
        parsed_value->is_null = 1;
        return parsed_value;
    }

    if(string_len == 0) {
        string_data.len = string_len;
        return parsed_value;
    }

    if(endptr[0] != '\r' || endptr[1] != '\n') {
        free(parsed_value);
        return NULL;
    }
    
    const char *data = endptr + 2;

    char *value = (char *)malloc(string_len + 1);
    if(value == NULL) {
        free(parsed_value);
        return NULL;
    }
    
    memcpy(value, data, string_len);
    value[string_len] = '\0';

    
    string_data.str = value;
    string_data.len = string_len;

    parsed_value->data.string = string_data;

    return parsed_value;
}

/*
    The encoding of this Simple String is the following 5 bytes:
    +OK\r\n
    Caller must free
 */
static resp_value *parse_simple_string(const char *payload) {
    if(payload[0] != '+') {
        return NULL;
    }

    resp_value *parsed_value = calloc(1, sizeof(resp_value));
    if(parsed_value == NULL) {
        return NULL;
    }
    parsed_value->type = RESP_SIMPLE_STRING;

    string_value string_data;


    const char *data = payload + 1;
    int i = 0;
    while(data[i] != '\0' && data[i] != '\r') {
        i++;
    }

    if(data[i] != '\r' || data[i + 1] != '\n') {
        free(parsed_value);
        return NULL;
    }

    char *value = (char *)malloc(i + 1);
    if(value == NULL) {
        free(parsed_value);
        return NULL;
    }

    memcpy(value, data, i);
    value[i] = '\0';

    string_data.str = value;
    string_data.len = i;

    parsed_value->data.string = string_data;

    return parsed_value;
}

/*
    RESP Arrays' encoding uses the following format:
    *<number-of-elements>\r\n<element-1>...<element-n> i
*/
static resp_value *parse_array(const char* payload) {
    if (payload[0] != '*') {
        return NULL;
    }

   resp_value *parsed_value = calloc(1, sizeof(resp_value));
    if(parsed_value == NULL) {
        return NULL;
    }
    parsed_value->type = RESP_ARRAY;

    char* endptr;

    long array_length = strtol(payload + 1, &endptr, 10);
    if(array_length == -1) {
        parsed_value->is_null = 1;
        return parsed_value;
    }
    if(endptr[0] != '\r' || endptr[1] != '\n'){
        return NULL;
    }


    resp_value **elements = calloc(array_length, sizeof(resp_value*));

    const char *start_of_elements = endptr + 2;
    
    int i = 0;
    int current_element = 0;
    while(start_of_elements[i] != '\0') {
        resp_value *element = parse_resp(start_of_elements + i);
        if(element != NULL) {
            elements[current_element] = element;
            current_element++;
        }
        i++;
    }

    if(current_element != array_length) {
        return NULL;
    }

    parsed_value->data.array.elements = elements;
    parsed_value->data.array.count = array_length;
    
   return parsed_value; 
}


void free_resp(resp_value *resp) {
    if(resp == NULL) return;

    if(!resp->is_null) {
        switch(resp->type) {
            case RESP_ARRAY: {
                size_t count = resp->data.array.count;
                for(size_t i = 0; i < count; i++) {
                    free_resp(resp->data.array.elements[i]);
                }
                free(resp->data.array.elements);
                break;
            }
            case RESP_SIMPLE_STRING:
            case RESP_BULK_STRING:
                free(resp->data.string.str);
                break;
        }
    }
    free(resp);
}

resp_value *parse_resp(const char *payload) {
    char firstChar = payload[0];

    switch (firstChar) {
        case '+': {
            return parse_simple_string(payload);
        }
        case '$': {
            return parse_bulk_string(payload);
        } 
        case '*': {
            return parse_array(payload);
        }
    }

    return NULL;
}