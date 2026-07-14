#include "writer.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>


void write_simple_string(int fd, char *str) {
    char output[1024];
    sprintf(output, "+%s\r\n", str);

    write(fd, output, strlen(output));
}

void write_bulk_string(int fd, char *str, int len) {
    char output[1024];
    if (len < 0) {
        sprintf(output, "$-1\r\n");
    } else {
        sprintf(output, "$%d\r\n%s\r\n", len, str);
    }
    write(fd, output, strlen(output));
}

void write_error(int fd, char *error_msg) {
    char output[1024];
    sprintf(output, "-%s\r\n", error_msg);
    write(fd, output, strlen(output));
}
